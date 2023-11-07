/**
 * Pat Leahy pat@patleahy.com - CSEP 544 SP21 - Mini-project - Lemur
 *
 * The types used for keys and values in the LSM tree.
 * Functions for converting between types.
 */

#include "types.hpp"
#include <assert.h>

namespace kv
{

/**
 * Convert a string to a value type.
 */
void string2value(std::string str, kv_val_t &val)
{
    assert(str.length() < VAL_LEN);
    str.copy(val.data(), VAL_LEN-1, 0);
    val[str.length()] = '\0';
}

/**
 * Convert a value type to a string.
 */
void value2string(kv_val_t val, std::string &str)
{
    str = std::string(val.data());
}

}