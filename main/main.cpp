/**
 * Pat Leahy patleahy@uw.edu - CSEP 544 SP21 - Mini-project - Lemur
 *
 * A program to create an key/value store, insert values into it and then
 * search for some of the values. The time take to do the inserts and searches
 * will be reported.
 *
 * The makefile will compile this program to bin/lemur. To run this file enter
 * this command:
 *
 *      bin/lemur leveling|tiering size ratio bits hashes inserts
 *
 * where
 *      leveling|tiering    Which kind of LSM tree to use.
 *      size                The number of items to store in the top level.
 *      ratio               Size ratio between adjacent levels.
 *      bits                The number of bits int the bloom filter.
 *      hashes              The number of hashes in the bloom filter.
 *      inserts             The number of items to insert.
 *      searches            The number of items to search for.
 *
 * E.g.:
 *      bin/lemur leveling 1024 4 131072 4 100000 1000
 */

#include "../src/store.hpp"
#include "../src/test.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <random>
#include <cstring>

using namespace kv;

int main(int argc, char** argv)
{
    // Get the command line arguments.
    bool leveling = strcmp(argv[1], "leveling") == 0;
    int size = atoi(argv[2]);
    int ratio = atoi(argv[3]);
    int bits = atoi(argv[4]);
    int hashes = atoi(argv[5]);
    int inserts = atoi(argv[6]);
    int searches = atoi(argv[7]);

    // Setup the store.
    Store kv(leveling, size, ratio, bits, hashes);

    // Get a random list of keys.
    // This only contains even keys.
    srand(clock());
    std::vector<int> tests = makeRandomKeys(inserts);

    // Use this value.
    std::string str = "This is text";

    // Do the inserts.
    clock_t time = std::clock();
    for (int i = 0; i < inserts; i++)
    {
        int t = tests[i];
        kv.Put(t, str);
    }
    double insertTime = (std::clock() - time) / (double)CLOCKS_PER_SEC;

    // Search for values. Since only even keys are in the store
    // half the searches will be for a value that is not in the store.
    time = std::clock();
    for (int i = 0; i < searches; i++)
    {
        int r = rand() % inserts;
        kv.Contains(r);
    }
    double searchTime = (std::clock() - time) / (double)CLOCKS_PER_SEC;

    // Output the results.
    std::cout
        << (leveling ? "leveling" : "tiering")
        << "," << size
        << "," << ratio
        << "," << bits
        << "," << hashes
        << "," << inserts
        << "," << searches
        << "," << insertTime
        << "," << searchTime
        << std::endl;
}
