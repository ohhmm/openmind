#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned char (*Increment)(uint64_t *, size_t);

__attribute__((noinline)) static unsigned char inc_loopz(uint64_t *word, size_t count)
{
    if (!count)
        return 1;

    unsigned char overflow;
    __asm__ volatile (
        "1: incq (%[word])\n\t"
        "leaq 8(%[word]), %[word]\n\t"
        "loopz 1b\n\t"
        "setz %[overflow]"
        : [word] "+r"(word), "+c"(count), [overflow] "=qm"(overflow)
        :
        : "cc", "memory");
    return overflow;
}

__attribute__((noinline)) static unsigned char adc_carry(uint64_t *word, size_t count)
{
    unsigned char overflow;
    __asm__ volatile (
        "stc\n\t"
        "jrcxz 2f\n\t"
        "1: adcq $0, (%[word])\n\t"
        "jnc 2f\n\t"
        "leaq 8(%[word]), %[word]\n\t"
        "decq %%rcx\n\t"
        "jnz 1b\n\t"
        "2: setc %[overflow]"
        : [word] "+r"(word), "+c"(count), [overflow] "=qm"(overflow)
        :
        : "cc", "memory");
    return overflow;
}

static uint64_t ns(void)
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t))
        abort();
    return (uint64_t)t.tv_sec * 1000000000 + t.tv_nsec;
}

__attribute__((noinline)) static double run(Increment inc, uint64_t *values, size_t width,
                                            size_t count, size_t wrapped)
{
    for (size_t row = 0; row < count; ++row)
        for (size_t i = 0; i < width; ++i)
            values[row * width + i] = i < wrapped ? UINT64_MAX : 7;

    size_t overflows = 0;
    const uint64_t start = ns();
    for (size_t row = 0; row < count; ++row)
        overflows += inc(values + row * width, width);
    const uint64_t elapsed = ns() - start;

    if (overflows != (wrapped == width ? count : 0))
        abort();
    for (size_t row = 0; row < count; ++row)
        for (size_t i = 0; i < width; ++i)
            if (values[row * width + i] != (i < wrapped ? 0U : i == wrapped ? 8U : 7U))
                abort();

    return (double)elapsed / (double)count;
}

static int cmp(const void *a, const void *b)
{
    const double left = *(const double *)a, right = *(const double *)b;
    return (left > right) - (left < right);
}

int main(void)
{
    const size_t widths[] = {1, 8, 64, 1024};
    const Increment loops[] = {inc_loopz, adc_carry};
    for (size_t i = 0; i < 2; ++i)
        if (loops[i](NULL, 0) != 1)
            abort();

    puts("words,wrapped_words,inc_loopz_ns,adc_cf_ns");
    for (size_t i = 0; i < sizeof widths / sizeof widths[0]; ++i) {
        const size_t width = widths[i], count = 2097152 / width;
        const size_t prefixes[] = {0, width / 2, width - 1, width};
        uint64_t *values = malloc(count * width * sizeof *values);
        if (!values)
            return 1;

        for (size_t p = 0; p < sizeof prefixes / sizeof prefixes[0]; ++p) {
            const size_t wrapped = prefixes[p];
            if (p && wrapped == prefixes[p - 1])
                continue;

            double samples[2][9];
            for (size_t trial = 0; trial < 9; ++trial)
                for (size_t j = 0; j < 2; ++j) {
                    const size_t which = (trial + j) % 2;
                    samples[which][trial] = run(loops[which], values, width, count, wrapped);
                }
            for (size_t j = 0; j < 2; ++j)
                qsort(samples[j], 9, sizeof(double), cmp);
            printf("%zu,%zu,%.2f,%.2f\n", width, wrapped, samples[0][4], samples[1][4]);
        }
        free(values);
    }
}
