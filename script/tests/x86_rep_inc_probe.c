#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MEMORY_CASE(name, operand)                                          \
    case name:                                                             \
        __asm__ volatile ("cld\n\tbt %[input_cf], 0\n\t"                    \
                          "rep inc " operand "\n\tsetc %[output_cf]"       \
                          : "+D"(di), "+S"(si), "+d"(dx), "+c"(count),       \
                            [output_cf] "=qm"(cf)                          \
                          : [input_cf] "r"(initial_cf)                     \
                          : "cc", "memory");                              \
        break

#define REGISTER_CASE(name, operand)                                        \
    case name:                                                              \
        __asm__ volatile ("bt %[input_cf], 0\n\t"                            \
                          "rep inc " operand "\n\tsetc %[output_cf]"        \
                          : "+a"(value), "+c"(count), [output_cf] "=qm"(cf) \
                          : [input_cf] "r"(initial_cf)                      \
                          : "cc");                                         \
        break

enum Variant {
    DI8, DI16, DI32, DI64, SI8, SI16, SI32, SI64, DX8, DX16, DX32, DX64,
    AL8, AH8, AX16, AX32, AX64,
};

struct Case {
    enum Variant variant;
    const char *operand;
    unsigned width;
};

static const struct Case cases[] = {
    {DI8, "byte [rdi]", 1}, {DI16, "word [rdi]", 2},
    {DI32, "dword [rdi]", 4}, {DI64, "qword [rdi]", 8},
    {SI8, "byte [rsi]", 1}, {SI16, "word [rsi]", 2},
    {SI32, "dword [rsi]", 4}, {SI64, "qword [rsi]", 8},
    {DX8, "byte [rdx]", 1}, {DX16, "word [rdx]", 2},
    {DX32, "dword [rdx]", 4}, {DX64, "qword [rdx]", 8},
    {AL8, "al", 1}, {AH8, "ah", 1}, {AX16, "ax", 2},
    {AX32, "eax", 4}, {AX64, "rax", 8},
};

static void probe(struct Case test, uint64_t initial_count, uint64_t initial_cf, int wrap)
{
    unsigned char regions[3][5 * sizeof(uint64_t)];
    memset(regions, 0x3a, sizeof regions);
    uint64_t initial = wrap ? UINT64_MAX : UINT64_C(0x0a0a0a0a0a0a0a0a);
    for (unsigned i = 0; i < 3; ++i) {
        memcpy(regions[i], &initial, test.width);
        if (wrap)
            memcpy(regions[i] + test.width, &initial, test.width);
    }

    unsigned char *di = regions[0], *si = regions[1], *dx = regions[2];
    uint64_t count = initial_count, value = initial;
    unsigned char cf;

    switch (test.variant) {
        MEMORY_CASE(DI8,  "byte ptr [rdi]");
        MEMORY_CASE(DI16, "word ptr [rdi]");
        MEMORY_CASE(DI32, "dword ptr [rdi]");
        MEMORY_CASE(DI64, "qword ptr [rdi]");
        MEMORY_CASE(SI8,  "byte ptr [rsi]");
        MEMORY_CASE(SI16, "word ptr [rsi]");
        MEMORY_CASE(SI32, "dword ptr [rsi]");
        MEMORY_CASE(SI64, "qword ptr [rsi]");
        MEMORY_CASE(DX8,  "byte ptr [rdx]");
        MEMORY_CASE(DX16, "word ptr [rdx]");
        MEMORY_CASE(DX32, "dword ptr [rdx]");
        MEMORY_CASE(DX64, "qword ptr [rdx]");
        REGISTER_CASE(AL8,  "al");
        REGISTER_CASE(AH8,  "ah");
        REGISTER_CASE(AX16, "ax");
        REGISTER_CASE(AX32, "eax");
        REGISTER_CASE(AX64, "rax");
    }

    printf("rep inc %-12s wrap=%d rcx=%llu->%llu cf=%llu->%u"
           " ptr_same=%d%d%d rax=%016llx mem=",
           test.operand, wrap, (unsigned long long)initial_count, (unsigned long long)count,
           (unsigned long long)initial_cf, (unsigned)cf,
           di == regions[0], si == regions[1], dx == regions[2], (unsigned long long)value);
    for (unsigned i = 0; i < 3; ++i) {
        for (unsigned j = 0; j < 3 * test.width; ++j)
            printf("%02x", (unsigned)regions[i][j]);
        putchar(i == 2 ? '\n' : '/');
    }
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    for (unsigned i = 0; i < sizeof cases / sizeof cases[0]; ++i)
        for (uint64_t count = 0; count <= 5; count += 5)
            for (uint64_t cf = 0; cf < 2; ++cf)
                for (int wrap = 0; wrap < 2; ++wrap)
                    probe(cases[i], count, cf, wrap);

    unsigned char dest[5] = {0};
    unsigned char *di = dest;
    uint64_t count = sizeof dest;
    __asm__ volatile ("cld\n\trep stosb"
                      : "+D"(di), "+c"(count) : "a"(0xff) : "memory");
    printf("rep stosb control rcx=%llu advanced=%d bytes=", (unsigned long long)count,
           di == dest + sizeof dest);
    for (unsigned i = 0; i < sizeof dest; ++i)
        printf("%02x", (unsigned)dest[i]);
    putchar('\n');
}
