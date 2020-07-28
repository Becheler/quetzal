// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr> Florence Jornod <florence@jornod.com>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef __GENET_H_INCLUDED__
#define __GENET_H_INCLUDED__

/**************************************************************************
	Descriptor for marker types used in genetic sample data :
	 - In microsatellite data, allelic states are unsigned integers
	 - In Single Nuclear Polymorphism data, allelic states are A, T, G, C
***************************************************************************/

namespace quetzal{
namespace genetics{

	template<typename T>
	struct Marker_Descriptor
	{
		typedef T value_type;
	};

	Marker_Descriptor<unsigned int> microsat_instance;
	Marker_Descriptor<std::string> SNP_instance;

	using undefined = int;
	using microsatellite = Marker_Descriptor<unsigned int>;
	using SNP = Marker_Descriptor<std::string>;

}
}

#endif
