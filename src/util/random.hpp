// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <random>

/**
 * "Just calculate the across the globe accrued man hours of everyone wasting
 * time on understanding that header and its contents to see how bad it is."
 * ~ BitTickler on SO
 */
class Randomizer
{
	private:
		static std::random_device ranDev; // rendez-vous...
		static std::default_random_engine engine;

	public:
		static int getRandomBetween(int min, int max);
};
