#!/bin/bash

tests/test_bits.sh 2> tests/bits.err 1> tests/bits.csv
tests/test_size.sh 2> tests/size.err 1> tests/size.csv

echo !!! Tests Done !!!