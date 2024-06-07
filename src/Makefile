BUILD_DIR:=../build

$(BUILD_DIR)/%.bin: %.S
	nasm -f bin $< -o $@

$(BUILD_DIR)/%.o: %.S
	nasm -felf -o $@ $<

$(BUILD_DIR)/%.o: %.c
	gcc -m32 -c -o $@ $<

$(BUILD_DIR)/%.S: %.c
	gcc -m32 -S -o $@ $<
	
$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/main.o $(BUILD_DIR)/print.o
	ld -melf_i386 -e main -Ttext 0xc0001500 -o $@ $(BUILD_DIR)/main.o $(BUILD_DIR)/print.o

$(BUILD_DIR)/master.img: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/loader.bin $(BUILD_DIR)/kernel.bin
	yes | bximage -q -hd -mode=flat -size=16 $@
	dd if=$(BUILD_DIR)/boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD_DIR)/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD_DIR)/kernel.bin of=$@ bs=512 count=200 seek=9 conv=notrunc

.PHONY: bochs
bochs: bochsrc $(BUILD_DIR)/master.img
	bochs -f $<

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*.*
