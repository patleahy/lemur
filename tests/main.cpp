/**
 * Pat Leahy pat@patleahy.com - CSEP 544 SP21 - Mini-project - Lemur
 *
 * Simple tests of the LSM tree.
 *
 * The makefile compiles this program to bin/lemur-test. To run the tests use
 * the command:
 *
 *     bin/lemur-test n
 *
 * where n is an integer.
 *
 * n items will be put in the LSM tree and then n/100 items will be searched
 * for. Half the searche will be for existing keys and half will be for
 * non-existing key. This test will be run twice, once using leveling and once
 * using tiering.
 *
 * To display the contents of the LSM tree add 'debug' to the command, e.g.:
 *
 *  bin/lemur-test 100 debug
 *
 * A second test is run to verify that if the same key is added multiple times
 * the most recent value is reterived.
 */

#include "../src/store.hpp"
#include "../src/test.hpp"
#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>

using namespace kv;

void test(std::vector<int>& tests, bool leveling, bool debug);
void testUpdates(bool leveling);


int main(int argc, char** argv)
{
    int n = atoi(argv[1]);
    bool debug = (argc > 2) && (strcmp(argv[2], "debug") == 0);

    std::vector<int> tests = makeRandomKeys(n);

    test(tests, false, debug);
    test(tests, true, debug);
    testUpdates(false);
    testUpdates(true);
}


void test(std::vector<int>& tests, bool leveling, bool debug)
{
    Store kv(leveling, 1024, 3, 1024*1024, 4);
    std::string str = "This is text";

    for (int i = 0; i < tests.size(); i++)
    {
        int t = tests[i];
        std::stringstream val;
        val << "Val " << std::setw(8) << t;
        kv.Put(t, val.str());
    }

    std::cout << std::endl << kv.Dump(debug) << std::endl;

    int max = tests.size()*2;
    int searches = tests.size()/100;

    for (int i = 0; i < searches; i++)
    {
        int r = rand() % max;
        bool c = kv.Contains(r);

        if (((r % 2) == 0) != c)
        {
            std::cout
                << "Failure"
                << " " << (leveling ? "leveling" : "tiering")
                << " " << r
                << " " << (c ? "exists" : "not exists ")
                << std::endl;
            return;
        }
    }
    std::cout
        << "Success"
        << " " << (leveling ? "leveling" : "tiering")
        << " size " << tests.size()
        << " searches " << searches
        << std::endl;
}

void testUpdates(bool leveling)
{
   std::cout << std::endl << "TestUpdates";

    Store kv(leveling, 5, 2, 1024*1024, 4);
    kv.Put(1, "1");
    kv.Put(1, "2");
    kv.Put(1, "3");
    kv.Put(1, "4");
    kv.Put(1, "5");
    kv.Put(1, "6");
    kv.Put(1, "7");

    std::cout << std::endl << kv.Dump(true) << std::endl;

    std::string val;
    kv.Get(1, val);

    if (val.compare("7") == 0)
    {
        std::cout << " Success";
    }
    else
    {
        std::cout << " Failure";
    }

    std::cout
        << " " << (leveling ? "leveling" : "tiering")
        << " " << val
        << std::endl;
}