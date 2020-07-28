

echo -e "UTest_genetics\n|"


echo -e "|--\t DiploidIndividual_test............................"
g++ -o DiploidIndividual_test -Wall -std=c++14 DiploidIndividual_test.cpp
./DiploidIndividual_test
rm DiploidIndividual_test
echo -e "|--\t DONE \n|"


echo -e "|--\t SpatialGeneticSample_test.........................."
g++ -o SpatialGeneticSample_test -Wall -std=c++14 SpatialGeneticSample_test.cpp
./SpatialGeneticSample_test
rm SpatialGeneticSample_test
echo -e "|--\t DONE \n|"

echo -e "|--\t Loader_test.........................."
g++ -o SpatialGeneticSampleLoader_test -Wall -std=c++14 SpatialGeneticSampleLoader_test.cpp
./SpatialGeneticSampleLoader_test
rm SpatialGeneticSampleLoader_test
echo -e "|--\t DONE \n|"

echo -e "DONE\n"
