OBJS:=main.o subtract.o add.o

%o:%.c
	gcc -c -o $@ $<

main:$(OBJS)
	gcc -o $@ $^

.PHONY:doc
doc: main main.o add.o
	objdump -d -j.text main > main.text.txt
	objdump -d -j.text main.o > main.o.text.txt
	objdump -d -j.text add.o > add.o.text.txt

.PHONY:clean
clean:
	rm -f *.o *.out main *.txt
