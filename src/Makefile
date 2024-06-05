BUILD_DIR:=../build

$(BUILD_DIR)/%.bin: %.S
	nasm -f bin $< -o $@

$(BUILD_DIR)/master.img: $(BUILD_DIR)/mbr.bin $(BUILD_DIR)/loader.bin
	yes | bximage -q -hd -mode=flat -size=16 $@
	dd if=$(BUILD_DIR)/mbr.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD_DIR)/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc

.PHONY: bochs
bochs: bochsrc $(BUILD_DIR)/master.img
	bochs -f $<

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*.bin $(BUILD_DIR)/*.img $(BUILD_DIR)/*.o