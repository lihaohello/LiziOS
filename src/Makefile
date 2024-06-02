%.bin: %.S
	nasm -f bin $< -o $@

master.img: mbr.bin loader.bin
	yes | bximage -q -hd -mode=flat -size=16 $@
	dd if=mbr.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=loader.bin of=$@ bs=512 count=1 seek=2 conv=notrunc

.PHONY: bochs
bochs: bochsrc master.img
	bochs -f $<

.PHONY: clean
clean:
	rm -f *.bin *.img