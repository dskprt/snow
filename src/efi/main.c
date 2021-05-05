#include <efi.h>
#include <efilib.h>
#include <elf.h>

typedef unsigned long long size_t;

typedef struct {
    unsigned int* Address;
    size_t BufferSize;
    unsigned int Width;
    unsigned int Height;
    unsigned int PixelsPerScanLine;
    unsigned int BytesPerPixel;
} Framebuffer;

typedef struct {
    unsigned char magic[2];
    unsigned char mode;
    unsigned char charsize;
} PSF1_HEADER;

typedef struct {
    PSF1_HEADER* header;
    void* glyphBuffer;
} PSF1_FONT;

typedef struct {
    Framebuffer* framebuffer;
    PSF1_FONT* font;
    EFI_MEMORY_DESCRIPTOR* memMap;
    UINTN memMapSize;
    UINTN memMapDescriptorSize;
} BootInfo;

int check(EFI_STATUS status) {
    if(EFI_ERROR(status)) {
        Print(L"Unexpected error occured.\r\n");
        return 1;
    }

    return 0;
}

int memcmp(const void* one, const void* two, size_t n) {
    const unsigned char* a = one;
    const unsigned char* b = two;

    for(size_t i = 0; i < n; i++) {
        if(a[i] < b[i]) return -1;
        else if(a[i] > b[i]) return 1;
    }

    return 0;
}

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_FILE* File;
    EFI_LOADED_IMAGE_PROTOCOL* Image;
    
    if(check(uefi_call_wrapper(SystemTable->BootServices->HandleProtocol, 3,
        ImageHandle, &gEfiLoadedImageProtocolGuid, (void**) &Image))) return NULL;

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
    
    if(check(uefi_call_wrapper(SystemTable->BootServices->HandleProtocol, 3,
        Image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**) &FileSystem))) return NULL;

    if(Directory == NULL) {
        if(check(uefi_call_wrapper(FileSystem->OpenVolume, 2, FileSystem, &Directory))) return NULL;
    }

    if(check(uefi_call_wrapper(Directory->Open, 5,
        Directory, &File, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY))) return NULL;

    return File;
}

Elf64_Addr LoadKernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_FILE* Kernel = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);

    if(Kernel == NULL) {
        Print(L"Unable to load the kernel.\r\n");
        return 0;
    }

    Elf64_Ehdr header;
    {
        UINTN FileInfoSize;
        EFI_FILE_INFO* FileInfo;

        EFI_STATUS status = uefi_call_wrapper(Kernel->GetInfo, 4,
        Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL); // for some reason check() does not want to work properly here

        if(EFI_ERROR(status)) {
            Print(L"Unable to load the kernel\r\n");
            return 0;
        }
        
        if(check(uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                        EfiLoaderData, FileInfoSize, (void**) &FileInfo))) return 0;
        if(check(uefi_call_wrapper(Kernel->GetInfo, 4,
                        Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**) &FileInfo))) return 0;

        UINTN size = sizeof(header);
        
        if(check(uefi_call_wrapper(Kernel->Read, 3, Kernel, &size, &header))) return 0;
    }

    if(memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0
            || header.e_ident[EI_CLASS] != ELFCLASS64
            || header.e_ident[EI_DATA] != ELFDATA2LSB
            || header.e_type != ET_EXEC
            || header.e_machine != EM_X86_64
            || header.e_version != EV_CURRENT) {

        Print(L"Bad kernel image format.\r\n");
        return 0;       
    }

    Elf64_Phdr* phdrs;
    {
        if(check(uefi_call_wrapper(Kernel->SetPosition, 2, Kernel, header.e_phoff))) return 0;

        UINTN size = header.e_phnum * header.e_phentsize;
        
        if(check(uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                        EfiLoaderData, size, (void**) &phdrs))) return 0;
        if(check(uefi_call_wrapper(Kernel->Read, 3, Kernel, &size, phdrs))) return 0;
    }

    for(Elf64_Phdr* phdr = phdrs;
        (char*) phdr < (char*) phdrs + header.e_phnum * header.e_phentsize;
        phdr = (Elf64_Phdr*) ((char*) phdr + header.e_phentsize)) {

        switch(phdr->p_type) {
            case PT_LOAD: {
                int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
                Elf64_Addr segment = phdr->p_paddr;

                uefi_call_wrapper(SystemTable->BootServices->AllocatePages, 4, AllocateAddress, EfiLoaderData, pages, &segment);
                if(check(uefi_call_wrapper(Kernel->SetPosition, 2, Kernel, phdr->p_offset))) return 0;

                UINTN size = phdr->p_filesz;
                
                if(check(uefi_call_wrapper(Kernel->Read, 3, Kernel, &size, (void*) segment))) return 0;
                break;
            }
        }
    }

    Print(L"Kernel loaded.\r\n");
    return header.e_entry;
}

PSF1_FONT* LoadPSF1Font(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_FILE* font = LoadFile(Directory, Path, ImageHandle, SystemTable);

    if(font == NULL) {
        Print(L"Unable to load the font.\r\n");
        return NULL;
    }

    PSF1_HEADER* fontHeader;

    if(check(uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
        EfiLoaderData, sizeof(PSF1_HEADER), (void**) &fontHeader))) return NULL;

    UINTN size = sizeof(PSF1_HEADER);
    if(check(uefi_call_wrapper(font->Read, 3, font, &size, fontHeader))) return NULL;

    if(fontHeader->magic[0] != 0x36 || fontHeader->magic[1] != 0x04) {
        Print(L"Invalid PSF1 magic.\r\n");
        return NULL;
    }

    UINTN glyphBufferSize = fontHeader->charsize * 256;

    if(fontHeader->mode == 1) {
        glyphBufferSize = fontHeader->charsize * 512;
    }

    void* glyphBuffer;

    if(check(uefi_call_wrapper(font->SetPosition, 2, font, sizeof(PSF1_HEADER)))) return NULL;
    if(check(uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
        EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer))) return NULL;
    if(check(uefi_call_wrapper(font->Read, 3, font, &glyphBufferSize, glyphBuffer))) return NULL;

    PSF1_FONT* finalFont;

    if(check(uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
        EfiLoaderData, sizeof(PSF1_FONT), (void**)&finalFont))) return NULL;

    finalFont->header = fontHeader;
    finalFont->glyphBuffer = glyphBuffer;

    return finalFont;
}

Framebuffer framebuffer;

Framebuffer* SetupGOP(EFI_SYSTEM_TABLE* SystemTable) {
    EFI_STATUS Status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;

    Status = uefi_call_wrapper(SystemTable->BootServices->LocateProtocol, 3, &gEfiGraphicsOutputProtocolGuid, NULL, (void**) &GOP);

    if(EFI_ERROR(Status)) {
        Print(L"Graphics Output Protocol (GOP) support is required.\r\n");
        return NULL;
    }

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    UINTN SizeOfInfo, NumModes, NativeMode;

    Status = uefi_call_wrapper(GOP->QueryMode, 4, GOP, (GOP->Mode == NULL ? 0 : GOP->Mode->Mode), &SizeOfInfo, &Info);

    if(Status == EFI_NOT_STARTED) {
        Status = uefi_call_wrapper(GOP->SetMode, 2, GOP, 0);
    }

    if(EFI_ERROR(Status)) {
        Print(L"Unable to get native mode\r\n");
        return NULL;
    } else {
        NativeMode = GOP->Mode->Mode;
        NumModes = GOP->Mode->MaxMode;
    }

    int mode = -1;
    
    for(int i = 0; i < NumModes; i++) {
        Status = uefi_call_wrapper(GOP->QueryMode, 4, GOP, i, &SizeOfInfo, &Info);

        if(EFI_ERROR(Status)) {
            Print(L"Unable to get mode %d.\r\n", i);
            return NULL;
        }

        if(Info->HorizontalResolution == 960 && Info->VerticalResolution == 640) {
            if(Info->PixelFormat != PixelRedGreenBlueReserved8BitPerColor
                && Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {

                Print(L"Pixel format not supported: %d\r\n", Info->PixelFormat);
                return NULL;
            }

            mode = i;
            break;
        }
    }

    if(mode == -1) {
        Print(L"Mode with resolution 960x640 not supported.\r\n");
        return NULL;
    }

    Status = uefi_call_wrapper(GOP->SetMode, 2, GOP, mode);

    if(EFI_ERROR(Status)) {
        Print(L"Unable to set mode %d.\r\n", mode);
        return NULL;
    }

    framebuffer.Address = (unsigned int*) (void*) GOP->Mode->FrameBufferBase;
    framebuffer.BufferSize = GOP->Mode->FrameBufferSize;
    framebuffer.Width = GOP->Mode->Info->HorizontalResolution;
    framebuffer.Height = GOP->Mode->Info->VerticalResolution;
    framebuffer.PixelsPerScanLine = GOP->Mode->Info->PixelsPerScanLine;
    framebuffer.BytesPerPixel = 4;

    return &framebuffer;
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    InitializeLib(ImageHandle, SystemTable);

    Elf64_Addr kernelEntry = LoadKernel(ImageHandle, SystemTable);
    if(kernelEntry == 0) return EFI_SUCCESS;

    PSF1_FONT* font = LoadPSF1Font(NULL, L"Unifont-APL8x16-13.0.06.psf", ImageHandle, SystemTable);
    if(!font) return EFI_SUCCESS;

    Framebuffer* framebuffer = SetupGOP(SystemTable);
    if(!framebuffer) return EFI_SUCCESS;

    EFI_MEMORY_DESCRIPTOR* Map = NULL;
    UINTN MapSize, MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
 
    uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5,
        &MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
    uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
        EfiLoaderData, MapSize, (void**)&Map);
    uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5,
        &MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);

    void (*kmain)(BootInfo*) = ((__attribute__((sysv_abi)) void (*)(BootInfo*)) kernelEntry);

    BootInfo bootInfo;
    bootInfo.framebuffer = framebuffer;
    bootInfo.font = font;
    bootInfo.memMap = Map;
    bootInfo.memMapSize = MapSize;
    bootInfo.memMapDescriptorSize = DescriptorSize;

    if(check(uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2, ImageHandle, MapKey))) return EFI_SUCCESS;

    kmain(&bootInfo);
    return EFI_SUCCESS;
}
