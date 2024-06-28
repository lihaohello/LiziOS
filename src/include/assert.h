#ifndef __ASSERT_H
#define __ASSERT_H

void panic_spin(char*, int, const char*, const char*);

#define PANIC(...) panic_spin(__FILE__, __LINE__, __func__, __VA_ARGS__)

#ifdef NDEBUG
#define ASSERT(CONDITION) ((void)0)
#else
#define ASSERT(CONDITION) \
    if (CONDITION) {      \
    } else                \
        PANIC(#CONDITION);
#endif

#endif