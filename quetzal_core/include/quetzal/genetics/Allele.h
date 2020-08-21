// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr> Florence Jornod <florence@jornod.com>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef __ALLELE_H_INCLUDED__
#define __ALLELE_H_INCLUDED__


namespace quetzal{
namespace genetics{

template<typename Value>
class Allele
{
public:
	using value_type = Value;

	Allele(value_type allelic_state) : m_allelic_state(allelic_state)
	{}

	Allele() = default;

	value_type get_allelic_state() const {return m_allelic_state;}

private:
	value_type m_allelic_state;
};

}
}

#endif
