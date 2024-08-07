BUILD_DIR:=../build/
BOOT_DIR:=boot/
KERNEL_DIR:=kernel/
LIB_DIR:=lib/
INCLUDES = -I ./include

KERNEL_OBJS:=$(BUILD_DIR)main.o $(BUILD_DIR)interrupt.o $(BUILD_DIR)prints.o \
$(BUILD_DIR)interrupt_handler.o $(BUILD_DIR)timer.o $(BUILD_DIR)assert.o \
$(BUILD_DIR)thread.o $(BUILD_DIR)printf.o $(BUILD_DIR)string.o $(BUILD_DIR)vsprintf.o \
$(BUILD_DIR)memory.o $(BUILD_DIR)bitmap.o $(BUILD_DIR)list.o $(BUILD_DIR)switch.o \
$(BUILD_DIR)sync.o $(BUILD_DIR)console.o $(BUILD_DIR)keyboard.o $(BUILD_DIR)ioqueue.o \
$(BUILD_DIR)tss.o $(BUILD_DIR)process.o


$(BUILD_DIR)%.bin: $(BOOT_DIR)%.asm
	nasm -f bin $< -o $@

$(BUILD_DIR)%.o: $(KERNEL_DIR)%.asm
	nasm -felf -o $@ $<

$(BUILD_DIR)%.o: $(LIB_DIR)%.asm
	nasm -felf -o $@ $<


$(BUILD_DIR)%.o: $(KERNEL_DIR)%.c
	gcc -m32 -c -o $@ $< -fno-builtin -fno-stack-protector $(INCLUDES)

$(BUILD_DIR)%.o: $(LIB_DIR)%.c
	gcc -m32 -c -o $@ $< -fno-builtin -fno-stack-protector $(INCLUDES)


$(BUILD_DIR)kernel: $(KERNEL_OBJS)
	ld -melf_i386 -e main -Ttext 0xc0001500 -o $@ $^

$(BUILD_DIR)master.img: $(BUILD_DIR)boot.bin $(BUILD_DIR)loader.bin $(BUILD_DIR)kernel
	yes | bximage -func=create -hd=16 -imgmode=flat -q $@
	dd if=$(BUILD_DIR)boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD_DIR)loader.bin of=$@ bs=512 count=2 seek=2 conv=notrunc
	dd if=$(BUILD_DIR)kernel of=$@ bs=512 count=200 seek=9 conv=notrunc

.PHONY: check
check:
	mkdir -p $(BUILD_DIR)

.PHONY: bochs
bochs: bochsrc $(BUILD_DIR)master.img
	bochs -f $< -q

.PHONY: qemu
qemu: $(BUILD_DIR)master.img
	qemu-system-i386 -s -S \
	-m 32M \
	-boot c \
	-hda $<

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)*.*
