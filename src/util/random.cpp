#include "random.hpp"

std::random_device Randomizer::ranDev;
std::default_random_engine Randomizer::engine(Randomizer::ranDev());

int Randomizer::getRandomBetween(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(Randomizer::engine);
}
