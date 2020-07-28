#!/bin/bash

g++ -o UTest -Wall -std=c++17 expressive_test.cpp
./UTest > expressive_test.output
rm UTest
