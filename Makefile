EFIFLAGS=-fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args
CFLAGS=-ffreestanding -fshort-wchar
LDFLAGS=-T linker.ld -static -Bsymbolic -nostdlib

SRCDIR=src
RESOURCESDIR=resources
OUTDIR=bin

CPPSRC=$(shell find $(SRCDIR) -name '*.cpp')
ASMSRC=$(shell find $(SRCDIR) -name '*.asm')
RESOURCES=$(shell find $(RESOURCESDIR) -name '*.*')

OBJECTS := $(patsubst %.cpp, %.o, $(CPPSRC))
OBJECTS += $(patsubst %.asm, %.so, $(ASMSRC))

.DEFAULT_GOAL = build
build: $(SRCDIR)/efi/main.o $(OBJECTS) $(SOBJECTS) link create_img
debug: dbg_flags build

dbg_flags:
	$(eval CFLAGS += -g)

$(SRCDIR)/efi/main.o: $(SRCDIR)/efi/main.c
	mkdir -p $(OUTDIR)
	gcc -I/usr/include/efi -I/usr/include/efi/x86_64 $(EFIFLAGS) -c $< -o $@
	ld -shared -Bsymbolic -L/usr/lib -T/usr/lib/elf_x86_64_efi.lds /usr/lib/crt0-efi-x86_64.o $@ -o $(basename $@).so -lgnuefi -lefi
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 $(basename $@).so $(OUTDIR)/BOOTX64.EFI

$(SRCDIR)/kernel/idt/interrupts.o: $(SRCDIR)/kernel/idt/interrupts.cpp
	g++ -mno-red-zone -mgeneral-regs-only -ffreestanding -c $^ -o $@

%.o: %.cpp
	g++ $(CFLAGS) -c $< -o $@

%.so: %.asm
	nasm $< -f elf64 -o $@

interrupts: $(SRCDIR)/kernel/idt/interrupts.cpp
	gcc -mno-red-zone -mgeneral-regs-only -ffreestanding -c $^ -o $(OUTDIR)/interrupts.o

link:
	ld $(LDFLAGS) -o $(OUTDIR)/kernel.elf $(OBJECTS)

create_img:
	dd if=/dev/zero of=$(OUTDIR)/snow.img bs=1k count=1440
	mformat -i $(OUTDIR)/snow.img -f 1440 ::
	mmd -i $(OUTDIR)/snow.img ::/EFI
	mmd -i $(OUTDIR)/snow.img ::/EFI/BOOT
	mcopy -i $(OUTDIR)/snow.img $(OUTDIR)/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i $(OUTDIR)/snow.img $(OUTDIR)/kernel.elf ::

	$(foreach file, $(RESOURCES), mcopy -i $(OUTDIR)/snow.img $(file) ::;)

clean:
	find $(SRCDIR) -name "*.o" -type f -delete
	find $(SRCDIR) -name "*.so" -type f -delete
	rm bin/*.*

run:
	qemu-system-x86_64 -drive file=$(OUTDIR)/snow.img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/OVMF/OVMF_CODE.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/OVMF_VARS.fd -net none

run_dbg:
	qemu-system-x86_64 -s -drive file=$(OUTDIR)/snow.img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/OVMF/OVMF_CODE.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/OVMF_VARS.fd -net none
