EFIFLAGS=-fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args
CFLAGS=-ffreestanding -fshort-wchar
LDFLAGS=-T linker.ld -static -Bsymbolic -nostdlib

SRCDIR=src
RESOURCESDIR=resources
OUTDIR=out

CSRC=$(shell find $(SRCDIR) -name '*.c' ! -wholename 'src/efi/main.c')
CPPSRC=$(shell find $(SRCDIR) -name '*.cpp')
RESOURCES=$(shell find $(RESOURCESDIR) -name '*.*')

.DEFAULT_GOAL = build
build: clean efi compile_c compile_cpp link create_img

efi: $(SRCDIR)/efi/main.c
	mkdir -p $(OUTDIR)
	gcc -I/usr/include/efi -I/usr/include/efi/x86_64 $(EFIFLAGS) -c $< -o $(OUTDIR)/efimain.o
	ld -shared -Bsymbolic -L/usr/lib -T/usr/lib/elf_x86_64_efi.lds /usr/lib/crt0-efi-x86_64.o $(OUTDIR)/efimain.o -o $(OUTDIR)/efimain.so -lgnuefi -lefi
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 $(OUTDIR)/efimain.so $(OUTDIR)/BOOTX64.EFI
	rm $(OUTDIR)/efimain.o $(OUTDIR)/efimain.so

compile_c: $(CSRC)
	mkdir -p $(OUTDIR)
	$(foreach file, $^, gcc $(CFLAGS) -c $(file) -o $(OUTDIR)/$(basename $(notdir $(file))).o;)

compile_cpp: $(CPPSRC)
	mkdir -p $(OUTDIR)
	$(foreach file, $^, g++ $(CFLAGS) -c $(file) -o $(OUTDIR)/$(basename $(notdir $(file))).o;)

link:
	ld $(LDFLAGS) -o $(OUTDIR)/kernel.elf $(wildcard $(OUTDIR)/*.o)

create_img:
	dd if=/dev/zero of=$(OUTDIR)/snow.img bs=1k count=1440
	mformat -i $(OUTDIR)/snow.img -f 1440 ::
	mmd -i $(OUTDIR)/snow.img ::/EFI
	mmd -i $(OUTDIR)/snow.img ::/EFI/BOOT
	mcopy -i $(OUTDIR)/snow.img $(OUTDIR)/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i $(OUTDIR)/snow.img $(OUTDIR)/kernel.elf ::

	$(foreach file, $(RESOURCES), mcopy -i $(OUTDIR)/snow.img $(file) ::;)

clean:
	rm -rf out/*

run:
	qemu-system-x86_64 -drive file=out/snow.img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/OVMF/OVMF_CODE.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/OVMF_VARS.fd -net none
