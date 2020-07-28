#!/bin/bash

echo -e "UTest_fuzzy_transfer_distance\n|"


echo -e "|--\t Partitioner_test..................................."
g++ -o Partitioner_test -Wall -std=c++14 Partitioner_test.cpp
./Partitioner_test
rm Partitioner_test
echo -e "|--\t DONE \n|"


echo -e "|--\t FuzzyPartition_test ..............................."
g++ -o FuzzyPartition_test FuzzyPartition_test.cpp -std=c++14 -Wall
./FuzzyPartition_test
rm FuzzyPartition_test
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
