#pragma once
#include <string>

// random number generator - Marsaglia's xor32 // from Jacco's template
// This is a high-quality RNG that uses a single 32-bit seed. More info:
// https://www.researchgate.net/publication/5142825_Xorshift_RNGs

extern unsigned int seed;

unsigned int WangHash(unsigned int s);
unsigned int InitSeed(unsigned int seedBase);
unsigned int RandomUInt();
float RandomFloat();
float Random(float range);
unsigned int RandomUInt(unsigned int& customSeed);
float RandomFloat(unsigned int& customSeed);
std::string GetRandomName(unsigned int length);