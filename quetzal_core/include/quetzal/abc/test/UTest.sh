#!/bin/bash

g++ -o UTest -Wall -std=c++14 test.cpp
./UTest > test.output
rm UTest

g++ -o UTest -Wall -std=c++14 rubin_test.cpp
./UTest > rubin_test.output
rm UTest

g++ -o UTest -Wall -std=c++14 pritchard_test.cpp
./UTest > pritchard_test.output
rm UTest

g++ -o UTest -Wall -std=c++14 pritchard_identity_test.cpp
./UTest > pritchard_identity_test.output
rm UTest
