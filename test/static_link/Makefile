OBJS:=main.o external.o

%o:%.c
	gcc -c -o $@ $< -fno-PIC -g

main:$(OBJS)
	gcc -o $@ $^ -no-pie

.PHONY:doc
doc: main $(OBJS)
	objdump -d -j.text main > main.text.txt
	objdump -d -j.text main.o > main.o.text.txt
	objdump -d -j.text external.o > external.o.text.txt

.PHONY:clean
clean:
	rm -f *.o *.out main *.txt
