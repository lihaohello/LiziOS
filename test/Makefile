%.bin:%.S
	nasm -fbin -o $@ $^

%.o: %.c
	gcc -m32 -c -o $@ $^

main.out: main.o
	ld -melf_i386 -e main -o $@ $^

C_with_S.out: C_with_S_c.c C_with_S_S.S
	gcc -m32 -c -o C_with_S_c.o C_with_S_c.c
	nasm -felf -o C_with_S_S.o C_with_S_S.S
	ld -melf_i386 -o $@ C_with_S_c.o C_with_S_S.o
	./$@

.PHONY: clean
clean:
	rm -f *.bin *.o main *.out

