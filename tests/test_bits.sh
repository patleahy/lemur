#!/bin/bash
#
# Create the LSM tree with different Bloom filter paramters.

for i in {1..20}
{
    for b in 1024 8192 65536 524288 4194304 33554432
    do
        for h in 1 2 3 4 5 6 7 8
        do
            rm data/lsm.*
            bin/lemur leveling 8192 4 $b $h 1000000 1000

            rm data/lsm.*
            bin/lemur leveling 8192 4 $b $h 1000000 100
        done
    done
}