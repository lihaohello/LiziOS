%.bin:%.S
	nasm -fbin -o $@ $^

%.o: %.c
	gcc -m32 -c -o $@ $^

main.out: main.o
	ld -melf_i386 -e main -o $@ $^

.PHONY: clean
clean:
	rm -f *.bin *.o main *.out

