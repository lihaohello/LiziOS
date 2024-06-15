myopen.so: myopen.c
	gcc -shared -fpic -o $@ $<

main: main.c
	gcc -o $@ $<

run: main myopen.so
	LD_PRELOAD=./myopen.so ./main

.PHONY: clean
clean:
	rm -f *.bin *.o main *.out *.so

