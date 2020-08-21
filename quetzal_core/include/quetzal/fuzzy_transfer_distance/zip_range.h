// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/ 

#ifndef ZIP_RANGE_H
#define ZIP_RANGE_H

#include <boost/range/iterator_range.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/tuple/tuple.hpp>

template<class... Conts>
auto zip_range(Conts&... conts)
  -> decltype(boost::make_iterator_range(
  boost::make_zip_iterator(boost::make_tuple(conts.begin()...)),
  boost::make_zip_iterator(boost::make_tuple(conts.end()...))))
{
  return {boost::make_zip_iterator(boost::make_tuple(conts.begin()...)),
          boost::make_zip_iterator(boost::make_tuple(conts.end()...))};
}

#endif