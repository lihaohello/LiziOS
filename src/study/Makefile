CFLAGS:= -m32 \
		-fno-asynchronous-unwind-tables \
		-static \
		-fno-pic \
		-mpreferred-stack-boundary=2 \
		-fno-stack-protector \
		-fno-builtin \
		-O0 \
		# -fverbose-asm

DEBUG:=-g

CC:gcc

%.s: %.c
	rm -f $@
	gcc $(CFLAGS) -S $< -o $@

%.o: %.asm
	rm -f $@
	nasm -f elf32 $(DEBUG) $< -o $@

%.o: %.c 
	rm -f $@
	gcc $(CFLAGS) $(DEBUG) -c $< -o $@

test: test.o asm.o 
	rm -f $@
	gcc $(CFLAGS) $(DEBUG) $^ -o $@
	./$@

.PHONY: clean
clean:
	rm -f *.o *.s test