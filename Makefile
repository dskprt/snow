EFIFLAGS=-fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args

SRCDIR = src
RESOURCESDIR = resources
OUTDIR = out

.DEFAULT_GOAL = build
build: efi create_img

efi: $(SRCDIR)/efi/main.c
	mkdir -p $(OUTDIR)
	gcc -I/usr/include/efi -I/usr/include/efi/x86_64 $(EFIFLAGS) -c $< -o $(OUTDIR)/efimain.o
	ld -shared -Bsymbolic -L/usr/lib -T/usr/lib/elf_x86_64_efi.lds /usr/lib/crt0-efi-x86_64.o $(OUTDIR)/efimain.o -o $(OUTDIR)/efimain.so -lgnuefi -lefi
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 $(OUTDIR)/efimain.so $(OUTDIR)/BOOTX64.EFI

create_img:
	dd if=/dev/zero of=$(OUTDIR)/snow.img bs=1k count=1440
	mformat -i $(OUTDIR)/snow.img -f 1440 ::
	mmd -i $(OUTDIR)/snow.img ::/EFI
	mmd -i $(OUTDIR)/snow.img ::/EFI/BOOT
	mcopy -i $(OUTDIR)/snow.img $(OUTDIR)/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i $(OUTDIR)/snow.img startup.nsh ::

clean:
	rm -rf out/*

run:
	qemu-system-x86_64 -drive file=out/snow.img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/OVMF/OVMF_CODE.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/OVMF_VARS.fd -net none
