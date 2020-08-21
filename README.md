![quetzal logo](documentation/logo.png)


[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/Becheler/quetzal/graphs/commit-activity) [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0) [![Tests](https://img.shields.io/badge/tests-passed-green.svg)](https://en.wikipedia.org/wiki/Unit_testing) [![Doc](https://img.shields.io/badge/doc-latest-blue.svg)](https://becheler.github.io/quetzalAPI/html/index.html)
[![Website becheler.github.io](https://img.shields.io/website-up-down-green-red/https/becheler.github.io.svg)](https://becheler.github.io/pages/quetzal.html)

# Overview

Quetzal-CoalTL is a C++ template scientific library for simulating gene genealogies in explicit landscapes.

Infering populations ecological features (such as migration or growth patterns) from genetic datasets can be a complex task. Is some settings it is actually a mathematically intractable problem, so simulations methods are needed ! Approximate Bayesian Computation (ABC) can be used to update your knowledge about the processes shaping your genetic dataset by simulating the underlying gene genealogies (coalescents) in an explicit geographic space.

Existing softwares like [SPLATCHE](http://splatche.com/), [simcoal2](http://cmpg.unibe.ch/software/simcoal2/), [egglib](http://mycor.nancy.inra.fr/egglib/index.html), or [msprime](http://msprime.readthedocs.io/en/stable/index.html) are very useful and user-friendly resources that should be used whenever possible to achieve this task.

However if you are working on developing some border case simulation model, or if you are not comfortable with hypothesis of the existing software solutions, you will surely consider to build your own program.

Quetzal can help you doing so by offering atomic components (structures, algorithms, concepts) that can be easily reused to build a new program. The template mechanism allow to adapt them efficiently to each particular situation you may encounter.

# Website

Please visit [Quetzal website](https://becheler.github.io/pages/quetzal.html) for more details and:
 - [step-by-step installation instructions](https://becheler.github.io/pages/getting_started.html)
 - [user tutorials](https://becheler.github.io/pages/tutorials.html)
 - [developer documentation](https://becheler.github.io/quetzalAPI/html/index.html)

# Getting Started

## Installing

Quetzal is a header-only library. Just clone the project and include the files you need.

## Prerequisites

### g++
gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.2)

### GDAL

The Geospatial Data Abstraction Library (GDAL) is essential to represent a spatially explicit landscape!
To install GDAL please visit: http://www.gdal.org/

### OS

The present project was tested with the following OS:

- Distributor ID: Ubuntu
- Description: Ubuntu 16.04.1 LTS
- Release: 16.04
- Codename: xenial

## CMake build
In quetzal project root:
```
mkdir build
cd build
cmake .. -DCMAKE_DISABLE_FIND_PACKAGE_Boost=ON -DCMAKE_DISABLE_FIND_PACKAGE_GDAL=ON
make --build .
make test
```

## Acknowledgements

# Authors

This github repository is basically the implementation of the probabilistic model of biological invasion developed during my PhD thesis under the direction of Stephane Dupas (laboratoire d'Evolution, Génomes, Comportements et Ecologie de Gif-sur-Yvette) and Camille Coron (Laboratoire de Mathématiques d'Orsay).

- AUTHOR : Arnaud Becheler
- DATE   : 2016
- LICENCE : This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.    
- CONTACT : Arnaud.Becheler@egce.cnrs-gif.fr
- WEBSITE : https://becheler.github.io/
- DOCUMENTATION : https://becheler.github.io/pages/quetzal.html

## How to cite:

 Becheler, A, Coron, C, Dupas, S. The Quetzal Coalescence template library: A C++ programmers resource for integrating distributional, demographic and coalescent models. Mol Ecol Resour. 2019; 19: 788– 793. https://doi.org/10.1111/1755-0998.12992

# Acknowledgements

- Florence Jornod who was a tremendous intern and add much to the project.
- Ambre Marques for his help, tips and the implementation of the expressive library.
- Arnaud Le Rouzic for help and tips.
- The developpez.com community, for his large help. This project would have failed without the constant support of Bousk, ternel, Koala, bacelar, dalfab, Medinoc, jo_link_noir and many others...
