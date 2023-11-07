/**
 * Pat Leahy pat@patleahy.com - CSEP 544 SP21 - Mini-project - Lemur
 *
 * Methods to help testing the LSM tree.
 */

#include "test.hpp"
#include <algorithm>

namespace kv
{

/**
 * Make a list of all the even numbers between 0 and n-1. The list will is
 * shuffled.
 */
std::vector<int> makeRandomKeys(int n)
{
    std::vector<int> tests(n);

    for (int i = 0; i < n; i++)
    {
        tests[i] = i*2;
    }
    std::random_shuffle(tests.begin(), tests.end());

    return tests;
}

}