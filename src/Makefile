boot.bin: boot.asm
	nasm -f bin $< -o $@

master.img: boot.bin
	bximage -q -hd -mode=flat -size=16 $@
	dd if=$< of=$@ bs=512 count=1 conv=notrunc

.PHONY: clean
clean:
	rm -rf *.bin *.img

.PHONY: bochs
bochs: bashrc master.img
	bochs -f $<
