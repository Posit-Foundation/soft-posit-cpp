// tests/common_tests/random_generators.h
#ifndef SOFTPOSIT_RANDOM_GENERATORS_H
#define SOFTPOSIT_RANDOM_GENERATORS_H

#include <random>
#include <functional>
#include "softposit_cpp.h"

namespace softposit_test
{

    class RandomGenerators
    {
    private:
        static std::random_device rd;
        static std::mt19937 gen;

    public:
        // Integer distributions for different posit sizes
        static std::uniform_int_distribution<int8_t> int8_dist;
        static std::uniform_int_distribution<int16_t> int16_dist;
        static std::uniform_int_distribution<int32_t> int32_dist;

        // Real distributions
        static std::uniform_real_distribution<double> real_dist_small;  // -100 to 100
        static std::uniform_real_distribution<double> real_dist_medium; // -10000 to 10000
        static std::uniform_real_distribution<double> real_dist_large;  // -1e6 to 1e6

        // Initialize the generators
        static void initialize(unsigned int seed = 0)
        {
            if (seed != 0)
            {
                gen.seed(seed);
            }
        }

        // Generator functions for different posit types
        static posit8 randomPosit8()
        {
            posit8 p;
            p.value = int8_dist(gen);
            return p;
        }

        static posit16 randomPosit16()
        {
            posit16 p;
            p.value = int16_dist(gen);
            return p;
        }

        static posit32 randomPosit32()
        {
            posit32 p;
            p.value = int32_dist(gen);
            return p;
        }

        // Generator for positive posits only
        static posit8 randomPositivePosit8()
        {
            posit8 p;
            do
            {
                p.value = int8_dist(gen);
            } while (p.toDouble() <= 0);
            return p;
        }

        static posit16 randomPositivePosit16()
        {
            posit16 p;
            do
            {
                p.value = int16_dist(gen);
            } while (p.toDouble() <= 0);
            return p;
        }

        static posit32 randomPositivePosit32()
        {
            posit32 p;
            do
            {
                p.value = int32_dist(gen);
            } while (p.toDouble() <= 0);
            return p;
        }
    };

    // Initialize static members
    std::random_device RandomGenerators::rd;
    std::mt19937 RandomGenerators::gen(RandomGenerators::rd());
    std::uniform_int_distribution<int8_t> RandomGenerators::int8_dist(INT8_MIN, INT8_MAX);
    std::uniform_int_distribution<int16_t> RandomGenerators::int16_dist(INT16_MIN, INT16_MAX);
    std::uniform_int_distribution<int32_t> RandomGenerators::int32_dist(INT32_MIN, INT32_MAX);
    std::uniform_real_distribution<double> RandomGenerators::real_dist_small(-100.0, 100.0);
    std::uniform_real_distribution<double> RandomGenerators::real_dist_medium(-10000.0, 10000.0);
    std::uniform_real_distribution<double> RandomGenerators::real_dist_large(-1e6, 1e6);

} // namespace softposit_test

#endif // SOFTPOSIT_RANDOM_GENERATORS_H