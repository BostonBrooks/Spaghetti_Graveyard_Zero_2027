#ifndef BB_RAND_H
#define BB_RAND_H
#include "bbIntTypes.h"

static U64 bbRand_next(U64* state) {
    *state +=0x9e3779b97f4a7c15ULL;
    U64 z = *state;

    // 2. Apply a 64-bit avalanche hash mixer (bit-shifts and prime multipliers)

    z  = (z  ^ (z  >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z  = (z  ^ (z  >> 27)) * 0x94d049bb133111ebULL;
    return z  ^ (z  >> 31);
}

static U64 bbRand_hash(U64 old_hash) {
    U64 new_hash = old_hash + 0x9e3779b97f4a7c15ULL;

    // 2. Apply a 64-bit avalanche hash mixer (bit-shifts and prime multipliers)

    new_hash  = (new_hash  ^ (new_hash  >> 30)) * 0xbf58476d1ce4e5b9ULL;
    new_hash  = (new_hash  ^ (new_hash  >> 27)) * 0x94d049bb133111ebULL;
    return new_hash  ^ (new_hash  >> 31);
}

static U64 bbRand_hashIndex(U64 old_hash, I64 index) {
    U64 new_hash = index * 1610612741ULL + old_hash + 0x9e3779b97f4a7c15ULL;

    // 2. Apply a 64-bit avalanche hash mixer (bit-shifts and prime multipliers)

    new_hash  = (new_hash  ^ (new_hash  >> 30)) * 0xbf58476d1ce4e5b9ULL;
    new_hash  = (new_hash  ^ (new_hash  >> 27)) * 0x94d049bb133111ebULL;
    return new_hash  ^ (new_hash  >> 31);
}

#endif //BB_RAND_H