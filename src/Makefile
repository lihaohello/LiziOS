BUILD_DIR:=../build

$(BUILD_DIR)/%.bin: %.S
	nasm -f bin $< -o $@

$(BUILD_DIR)/%.o: %.c
	gcc -m32 -c -o $@ $<
	
$(BUILD_DIR)/kernel.bin: main.c print.S
	nasm -felf -o $(BUILD_DIR)/print.o print.S
	gcc -m32 -c -o $(BUILD_DIR)/main.o main.c
	ld -melf_i386 -e main -Ttext 0xc0001500 -o $@ $(BUILD_DIR)/main.o $(BUILD_DIR)/print.o

$(BUILD_DIR)/master.img: $(BUILD_DIR)/mbr.bin $(BUILD_DIR)/loader.bin $(BUILD_DIR)/kernel.bin
	yes | bximage -q -hd -mode=flat -size=16 $@
	dd if=$(BUILD_DIR)/mbr.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD_DIR)/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD_DIR)/kernel.bin of=$@ bs=512 count=200 seek=9 conv=notrunc

.PHONY: bochs
bochs: bochsrc $(BUILD_DIR)/master.img
	bochs -f $<

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*.bin $(BUILD_DIR)/*.img $(BUILD_DIR)/*.o
