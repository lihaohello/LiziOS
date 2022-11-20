%.bin: %.asm
	nasm -f bin $< -o $@

master.img: boot.bin loader.bin
	yes | bximage -q -hd -mode=flat -size=16 $@
	dd if=boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=loader.bin of=$@ bs=512 count=1 seek=1 conv=notrunc
	dd if=loader.bin of=$@ bs=512 count=1 seek=2 conv=notrunc
	dd if=loader.bin of=$@ bs=512 count=1 seek=3 conv=notrunc
	dd if=loader.bin of=$@ bs=512 count=1 seek=4 conv=notrunc

.PHONY: clean
clean:
	rm -rf *.bin *.img

.PHONY: bochs
bochs: bashrc master.img
	bochs -f $<
