static int static_var;
int global_var;
extern int extern_var;
extern int extern_func();

static int static_func()
{
    return 10;
}

int global_func()
{
    return 20;
}

int demo()
{
    static_var = 1;
    global_var = 2;
    extern_var = 3;
    int ret_var = static_var + global_var + extern_var;
    ret_var += static_func();
    ret_var += global_func();
    ret_var += extern_func();
    return ret_var;
}
