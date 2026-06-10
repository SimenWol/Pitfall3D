// random number generator - Marsaglia's xor32 // from Jacco's template
// This is a high-quality RNG that uses a single 32-bit seed. More info:
// https://www.researchgate.net/publication/5142825_Xorshift_RNGs
#include "random.h"
#include <iostream>
#include <string>
#include <vector>

// RNG seed. NOTE: in a multithreaded application, don't use a single seed!
unsigned int seed = 0x12345678;

// WangHash: calculates a high-quality seed based on an arbitrary non-zero
// integer. Use this to create your own seed based on e.g. thread index.
unsigned int WangHash(unsigned int s)
{
	s = (s ^ 61) ^ (s >> 16);
	s *= 9, s = s ^ (s >> 4);
	s *= 0x27d4eb2d;
	s = s ^ (s >> 15);
#ifdef _DEBUG
	std::cout << "Initialised random number seed with: " << s << std::endl;
#endif // _DEBUG
	return s;
}

//////////////////////////////////////////////////////////////////////////////////
// Random Name Generator based off: https://www.youtube.com/watch?v=epzhyABuNb8 //
std::string GetRandomName(unsigned int length)
{
	const std::string vowels = "aeiou";
	const std::string consonants = "bcdfghjklmnpqrstvwxyz";

	std::string name;

	int maxVowels = 2, vowelReset = 2;
	int maxConsonants = 2, consonantReset = 2;
	int vowelChance = RandomUInt() % 100;

	for (unsigned int l = 0; l < length; l++)
	{
		if (vowelChance < 50)
		{
			name += vowels[RandomUInt() % vowels.size()];
			maxVowels -= 1;
			maxConsonants = consonantReset;
			if (maxVowels == 0) { vowelChance = 100; }
			else { vowelChance = RandomUInt() % 100; }
		}
		else
		{
			name += consonants[RandomUInt() % consonants.size()];
			maxConsonants -= 1;
			maxVowels = vowelReset;
			if (maxConsonants == 0) { vowelChance = 0; }
			else { vowelChance = RandomUInt() % 100; }
		}
	}

	return name;
}

unsigned int InitSeed(unsigned int seedBase)
{
	seed = WangHash((seedBase + 1) * 17);
	return seed;
}

// RandomUInt()
// Update the seed and return it as a random 32-bit unsigned int.
unsigned int RandomUInt()
{
	seed ^= seed << 13;
	seed ^= seed >> 17;
	seed ^= seed << 5;
	return seed;
}

// RandomFloat()
// Calculate a random unsigned int and cast it to a float in the range
// [0..1)
float RandomFloat() { return (float)RandomUInt() * 2.3283064365387e-10f; }
float Random(float range) { return RandomFloat() * range; }

// Calculate a random number based on a specific seed
unsigned int RandomUInt(unsigned int& customSeed)
{
	customSeed ^= customSeed << 13;
	customSeed ^= customSeed >> 17;
	customSeed ^= customSeed << 5;
	return customSeed;
}

float RandomFloat(unsigned int& customSeed) { return (float)RandomUInt(customSeed) * 2.3283064365387e-10f; }