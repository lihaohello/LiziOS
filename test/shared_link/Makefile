%.bin:%.S
	nasm -fbin -o $@ $^

%.o: %.c
	gcc -m32 -c -o $@ $^

math.so: add.c subtract.c
	gcc -shared -fpic -o $@ $^

main: main.c math.so
	gcc -o $@ main.c ./math.so

.PHONY: clean
clean:
	rm -f *.bin *.o main *.out *.so

