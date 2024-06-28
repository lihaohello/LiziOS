BUILD_DIR:=../build/
BOOT_DIR:=boot/
KERNEL_DIR:=kernel/
KERNEL_OBJS:=$(BUILD_DIR)main.o $(BUILD_DIR)interrupt.o $(BUILD_DIR)print.o \
$(BUILD_DIR)handler.o $(BUILD_DIR)timer.o $(BUILD_DIR)assert.o

$(BUILD_DIR)%.bin: $(BOOT_DIR)%.asm
	nasm -f bin $< -o $@

$(BUILD_DIR)%.o: $(KERNEL_DIR)%.asm
	nasm -felf -o $@ $<

$(BUILD_DIR)%.o: $(KERNEL_DIR)%.c
	gcc -m32 -c -o $@ $< -fno-builtin -fno-stack-protector

$(BUILD_DIR)kernel.bin: $(KERNEL_OBJS)
	ld -melf_i386 -e main -Ttext 0xc0001500 -o $@ $^

$(BUILD_DIR)master.img: $(BUILD_DIR)boot.bin $(BUILD_DIR)loader.bin $(BUILD_DIR)kernel.bin
	yes | bximage -func=create -hd=16 -imgmode=flat -q $@
	dd if=$(BUILD_DIR)boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD_DIR)loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD_DIR)kernel.bin of=$@ bs=512 count=200 seek=9 conv=notrunc

.PHONY: check
check:
	mkdir -p $(BUILD_DIR)

.PHONY: bochs
bochs: bochsrc $(BUILD_DIR)master.img
	bochs -f $< -q

.PHONY: qemu
qemu: $(BUILD_DIR)master.img
	qemu-system-i386 \
	-m 32M \
	-boot c \
	-hda $< \
	-S -s

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)*.*
