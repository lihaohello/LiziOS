char* str = "Hello World!\n";

void print() {
    asm("movl $13,%edx; \
        movl str,%ecx; \
        movl $0,%ebx; \
        movl $4,%eax; \
        int $0x80;");
}

void exit() {
    asm("movl $42,%ebx; \
        movl $1,%eax; \
        int $0x80;");
}

void nomain() {
    print();
    exit();
}