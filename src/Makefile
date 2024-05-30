CFLAGS:= -m32 \
		 -O0 \
		 -static \
		 -fno-asynchronous-unwind-tables \
		 -fno-pic \
		 -fno-pie \
		 -mpreferred-stack-boundary=2 \
		 -fno-stack-protector \
		 -fno-builtin \
		 -fomit-frame-pointer \
		 -nostdlib \
		 -nodefaultlibs \
		 -nostdinc \
		 -nostartfiles \

%.bin: %.asm
	nasm -f bin $< -o $@

%.o: %.asm
	nasm -f elf32 $< -o $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

ENTRYPOINT:= 0x00011000
kernel.bin: start.o main.o
	ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRYPOINT)

master.img: boot.bin loader.bin kernel.bin 
	yes | bximage -q -hd -mode=flat -size=16 $@
	dd if=boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=loader.bin of=$@ bs=512 count=4 seek=1 conv=notrunc
	dd if=kernel.bin of=$@ bs=512 count=100 seek=10 conv=notrunc	

.PHONY: bochs
bochs: bochsrc master.img
	bochs -f $<

.PHONY: clean
clean:
	rm -f *.bin *.img *.o main

%.s: %.c 
	gcc $(CFLAGS) -S -fverbose-asm $< -o $@