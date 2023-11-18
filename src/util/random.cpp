// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#include "random.hpp"

std::random_device Randomizer::ranDev;
std::default_random_engine Randomizer::engine(Randomizer::ranDev());

int Randomizer::getRandomBetween(int min, int max)
{
	if (min == max)
		return min;

	std::uniform_int_distribution<int> dist(min, max);
	return dist(Randomizer::engine);
}
