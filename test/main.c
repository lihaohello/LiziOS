int global_init_var = 84;
int globale_uninit_var;

void func1(int i) {
}

int main(void) {
    static int static_var = 85;
    static int static_var2;
    int a = 1;
    int b;
    func1(static_var2 + static_var + 1 + b);
    return 0;
}