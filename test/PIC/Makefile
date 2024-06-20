libfoo.so: foo.c
	gcc $< -fPIC -shared -fno-plt -o $@

.PHONY:doc
doc: libfoo.so
	objdump -S $< > libfoo.so.txt

.PHONY:clean
clean:
	rm -f *.o *.out *.so *.txt
