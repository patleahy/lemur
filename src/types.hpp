/**
 * Pat Leahy pat@patleahy.com - CSEP 544 SP21 - Mini-project - Lemur
 *
 * The types used for keys and values in the LSM tree.
 * Also contains some functions for converting between types.
 */

#ifndef KVTYPES_H
#define KVTYPES_H

#include <array>

namespace kv
{

const int VAL_LEN = 20;     // The lenght of a value.

using kv_key_t = int;       // The key type.
using kv_val_t = std::array<char, VAL_LEN>; // The value type.

/**
 * Convert a string to a value type.
 */
void string2value(std::string str, kv_val_t &val);

/**
 * Convert a value type to a string.
 */
void value2string(kv_val_t val, std::string &str);

}

#endif