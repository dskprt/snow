#include <efi.h>
#include <efilib.h>
#include <elf.h>

typedef unsigned long long size_t;

int h = 0;

int check(EFI_STATUS status) {
    if(EFI_ERROR(status)) {
        Print(L"Unable to load the kernel.\r\n");
        return 1;
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

int memcmp(const void* one, const void* two, size_t n) {
    const unsigned char* a = one;
    const unsigned char* b = two;

    for(size_t i = 0; i < n; i++) {
        if(a[i] < b[i]) return -1;
        else if(a[i] > b[i]) return 1;
    }

    return 0;
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    InitializeLib(ImageHandle, SystemTable);

    EFI_FILE* Kernel = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);

    if(Kernel == NULL) {
        Print(L"AAAA Unable to load the kernel.\r\n");
        return EFI_SUCCESS;
    }

    Elf64_Ehdr header;
    {
        UINTN FileInfoSize;
        EFI_FILE_INFO* FileInfo;

        EFI_STATUS status = uefi_call_wrapper(Kernel->GetInfo, 4,
            Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL); // for some reason check() does not want to work properly here

        if(EFI_ERROR(status)) {
            Print(L"Unable to load the kernel\r\n");
            return EFI_SUCCESS;
        }
        
        if(check(uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
            EfiLoaderData, FileInfoSize, (void**) &FileInfo))) return EFI_SUCCESS;
        if(check(uefi_call_wrapper(Kernel->GetInfo, 4,
            Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**) &FileInfo))) return EFI_SUCCESS;

        UINTN size = sizeof(header);
        
        if(check(uefi_call_wrapper(Kernel->Read, 3, Kernel, &size, &header))) return EFI_SUCCESS;
    }

    if(memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0
        || header.e_ident[EI_CLASS] != ELFCLASS64
        || header.e_ident[EI_DATA] != ELFDATA2LSB
        || header.e_type != ET_EXEC
        || header.e_machine != EM_X86_64
        || header.e_version != EV_CURRENT) {

        Print(L"Bad kernel image format.\r\n");
        return EFI_SUCCESS;        
    }

    Elf64_Phdr* phdrs;
    {
        if(check(uefi_call_wrapper(Kernel->SetPosition, 2, Kernel, header.e_phoff))) return EFI_SUCCESS;

        UINTN size = header.e_phnum * header.e_phentsize;
        
        if(check(uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
            EfiLoaderData, size, (void**) &phdrs))) return EFI_SUCCESS;
        if(check(uefi_call_wrapper(Kernel->Read, 3, Kernel, &size, phdrs))) return EFI_SUCCESS;
    }

    for(Elf64_Phdr* phdr = phdrs;
        (char*) phdr < (char*) phdrs + header.e_phnum * header.e_phentsize;
        phdr = (Elf64_Phdr*) ((char*) phdr + header.e_phentsize)) {

        switch(phdr->p_type) {
            case PT_LOAD: {
                int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
                Elf64_Addr segment = phdr->p_paddr;

                uefi_call_wrapper(SystemTable->BootServices->AllocatePages, 4, AllocateAddress, EfiLoaderData, pages, &segment);
                if(check(uefi_call_wrapper(Kernel->SetPosition, 2, Kernel, phdr->p_offset))) return EFI_SUCCESS;

                UINTN size = phdr->p_filesz;
                
                if(check(uefi_call_wrapper(Kernel->Read, 3, Kernel, &size, (void*) segment))) return EFI_SUCCESS;
                break;
            }
        }
    }

    Print(L"Kernel loaded.\r\n");
    
    int (*kmain)() = ((__attribute__((sysv_abi)) int (*)()) header.e_entry);
    Print(L"%d\r\n", kmain());

    return EFI_SUCCESS;
}
