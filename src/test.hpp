/**
 * Pat Leahy pat@patleahy.com - CSEP 544 SP21 - Mini-project - Lemur
 *
 * Methods to help testing the LSM tree.
 */

#ifndef KVTEST_H
#define KVTEST_H

#include "types.hpp"
#include <vector>

namespace kv
{

/**
 * Make a list of all the even numbers between 0 and n-1. The list will is
 * shuffled.
 */
std::vector<int> makeRandomKeys(int n);

}
#endif