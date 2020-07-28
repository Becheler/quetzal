// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

//	g++ -o GeneticsLoader_test -Wall -std=c++14 GeneticsLoader_test.cpp -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include <assert.h>
#include <iostream>

#include "SpatialGeneticSampleLoader.h"
#include "marker_traits.h"

struct LatLon{
	LatLon(double lat, double lon): m_lat(lat), m_lon(lon) {}
	double m_lat;
	double m_lon;
	bool operator<(const LatLon& other) const {
			if(m_lat < other.m_lat) return true;
			if(m_lat > other.m_lat) return false;
			if(m_lon < other.m_lon) return true;
			if(m_lon > other.m_lon) return false;
			return false;
	}
};

int main(){
	std::string file = "microsat_test.csv";
	quetzal::genetics::Loader<LatLon, quetzal::genetics::microsatellite> reader;
	auto dataset = reader.read(file);

	std::set<std::string> expected({"List2003", "VMA8", "VMA6", "LIST2018B", "LIST2004B", "LIST2015", "LIST2020B", "R426", "D2185", "R4100",
									"R4114", "D315", "R136", "R175", "R177", "R180", "R433", "R1137", "R1169", "D2142", "R3115", "R1158"});

	std::set<std::string> obtained = dataset.loci();
	assert( ! obtained.empty());
	//std::cout << obtained << std::endl;
	assert(dataset.loci() == expected);

	return 0;
}
