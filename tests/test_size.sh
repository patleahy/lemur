#!/bin/bash
#
# Create the LSM tree with different level sizes and ratios.

for i in {1..20}
{
    for s in 1024 8192 32768 65536
    do
        for r in 2 4 8
        do
            rm data/lsm.*
            bin/lemur leveling $s $r 1048576 4 1000000 1000

            rm data/lsm.*
            bin/lemur tiering $s $r 1048576 4 100000 100
        done
    done
}