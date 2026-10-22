/* SPDX-License-Identifier: GPL-2.0-or-later */
/* SH-4 ROTL/ROTR must copy the outgoing bit to T, for both initial T values. */

#include <stdint.h>

static int check_rotl(uint32_t input, unsigned initial_t)
{
    uint32_t value = input;
    unsigned t, clear_t = !initial_t;

    asm volatile("tst %2,%2\n\t"
                 "rotl %0\n\t"
                 "movt %1"
                 : "+r"(value), "=r"(t) : "r"(clear_t) : "t");
    return value != ((input << 1) | (input >> 31)) || t != (input >> 31);
}

static int check_rotr(uint32_t input, unsigned initial_t)
{
    uint32_t value = input;
    unsigned t, clear_t = !initial_t;

    asm volatile("tst %2,%2\n\t"
                 "rotr %0\n\t"
                 "movt %1"
                 : "+r"(value), "=r"(t) : "r"(clear_t) : "t");
    return value != ((input >> 1) | (input << 31)) || t != (input & 1);
}

static int check(uint32_t input)
{
    return check_rotl(input, 0) || check_rotl(input, 1) ||
           check_rotr(input, 0) || check_rotr(input, 1);
}

int main(void)
{
    uint32_t value = 0x13451234u;

    if (check(0) || check(UINT32_MAX)) {
        return 1;
    }
    for (unsigned bit = 0; bit < 32; bit++) {
        if (check(1u << bit) || check(~(1u << bit))) {
            return 1;
        }
    }
    for (unsigned i = 0; i < 64; i++) {
        value ^= value << 13;
        value ^= value >> 17;
        value ^= value << 5;
        if (check(value)) {
            return 1;
        }
    }
    return 0;
}
