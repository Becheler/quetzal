// Copyright 2018 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/
#define BOOST_TEST_DYN_LINK

#define BOOST_TEST_MODULE geography_test
#include <boost/test/unit_test.hpp>

#include <quetzal/geography.h>
#include <iostream>

//#include <filesystem>
#include <boost/filesystem.hpp>

namespace utf = boost::unit_test;

inline std::string resolvePath(const std::string &relPath)
{
    //namespace fs = std::tr2::sys;
    //namespace fs = std::filesystem;
    namespace fs = boost::filesystem;
    auto baseDir = fs::current_path();
    while (baseDir.has_parent_path())
    {
        auto combinePath = baseDir / relPath;
        if (fs::exists(combinePath))
        {
            return combinePath.string();
        }
        baseDir = baseDir.parent_path();
    }
    throw std::runtime_error("File not found!");
}

BOOST_AUTO_TEST_SUITE( geography )

BOOST_AUTO_TEST_CASE( DiscreteLandscape )
  {
    using time_type = unsigned int;
    using landscape_type = quetzal::geography::DiscreteLandscape<std::string, time_type>;
    try {
    auto path1 = resolvePath("test/data/bio1.tif");
    auto path2 = resolvePath("test/data/bio12.tif");
    landscape_type env( {{"bio1",path1},{"bio12",path2}},
                        {2001,2002,2003,2004,2005,2006,2007,2008,2009,2010} );
    BOOST_CHECK_EQUAL(env.quantities_nbr(), 2);
    landscape_type::coord_type Bordeaux(44.5, 0.34);
    BOOST_TEST(env.is_in_spatial_extent(Bordeaux));
    BOOST_TEST(env.is_in_spatial_extent(env.reproject_to_centroid(Bordeaux)));
    auto f = env["bio1"];
    auto g = env["bio12"];
    auto times = env.temporal_definition_space();
    BOOST_CHECK_EQUAL(times.size(), 10);
    auto space = env.geographic_definition_space();
    BOOST_TEST(space.size() < 100);
    for(auto const& t : times){
      for(auto const& x : space){
        if( f(x,t) <= 200. && g(x,t) <= 600.){
          std::cout << f(x,t) << std::endl;
          std::cout << g(x,t) << std::endl;
        }
      }
    }
  } catch ( const std::exception & ex )
  {
    BOOST_ERROR(ex.what());
  }
}

BOOST_AUTO_TEST_CASE( EnvironmentalQuantity )
{
  using time_type = unsigned int;
	using quantity_type = quetzal::geography::EnvironmentalQuantity<time_type>;
	using coord_type = typename quantity_type::coord_type;
  auto path1 = resolvePath("test/data/bio1.tif");
	quantity_type bio1( path1, {2001,2002,2003,2004,2005,2006,2007,2008,2009,2010} );
	auto times = bio1.temporal_definition_space();
	auto space = bio1.geographic_definition_space();
	BOOST_CHECK_EQUAL( space.size() , 9 );
	BOOST_CHECK_EQUAL( times.size() , 10);
	BOOST_CHECK_EQUAL( bio1.origin() , coord_type(52. , -5.));
	std::cout << "Possible geographic support (demic structure) for demographic simulation" << std::endl;
	for(auto const& it : space)
  {
		std::cout << it << std::endl;
	}
	coord_type c6 = space.at(0);
	coord_type c8 = space.at(2);
	coord_type c3 = space.at(3);
	coord_type c4 = space.at(4);
	coord_type c0 = space.at(6);
	coord_type c1 = space.at(7);
	double some_value = bio1.at(space.front(), times.front());
	BOOST_TEST( some_value != 0 );
  /*
  //  Expected dataSet structure
  //
  //  * origin at Lon -5, Lat 52.
  //  * 9 cells
  //  * 10 layers
  //  * pixel size (-5, 5)
  //  * East and South limits ARE NOT in spatial extent
  //
  //
	// origin	    -5        0       5      10
	// 			 \   /         /  	 /		/
	// 			\ / 		/		/	   /
	// 		52	 * ------ * ----- * ---- *
	// 			 |    .   |   .   |   .
	// 			 |   c0   |  c1   |  c2
	// 		47	 * ------ * ----- * ---- *
	// 			 |    .   |   .   |   .
	// 			 |   c3   |  c4   |  c5
	// 		42	 * ------ * ----- * ---- *
	// 			 |   .    |   .   |   .
	// 			 |   c6   |  c7   |  c8
	// 		37	 *        *       *      *
  //
	// 		0	-  -----> + 180    Xsize positive in decimal degre (east direction positive)
  //
	// 		90
	// 		+
	// 			 |
	// 			 |
	// 		-	 \/ Y size negative in decimal degree (south direction negative)
	// 		0
  */
	// Corners
	coord_type expected_top_left(52., -5.);
	coord_type expected_bottom_right(37., 10.);
	BOOST_TEST(bio1.is_in_spatial_extent(expected_top_left));
	BOOST_TEST( ! bio1.is_in_spatial_extent(expected_bottom_right));
	// In boxes
	coord_type in_box_0(51., -4.5);
	coord_type in_box_1(48.32, 1.5);
	coord_type in_box_6(37.21, -3.2);
	coord_type in_box_8(38.21, 7.125);
	// Out of all boxes
	coord_type out_north(55. , 0.);
	// At boxes limits
	coord_type W_c0_limit(48. , -5.);
	coord_type N_c0_limit(52. , -2.);
	coord_type c0_c3_limit(47. , -2.);
	coord_type W_c0_c3_limit(47. , -5);
	coord_type c0_c1_c3_c4_limit(47. , 0.);
	coord_type E_c2_c5_limit(47. , 10.);
	// In boxes
	BOOST_TEST(bio1.reproject_to_centroid(in_box_0) == c0);
	BOOST_TEST(bio1.reproject_to_centroid(in_box_1) == c1);
	BOOST_TEST(bio1.reproject_to_centroid(in_box_6) == c6);
	BOOST_TEST(bio1.reproject_to_centroid(in_box_8) == c8);
	// Out of all boxes
	BOOST_CHECK_THROW(bio1.reproject_to_centroid(out_north), std::runtime_error);
	// At boxes limits
	BOOST_TEST(bio1.reproject_to_centroid(W_c0_limit) == c0);
	BOOST_TEST(bio1.reproject_to_centroid(N_c0_limit) == c0);
	BOOST_TEST(bio1.reproject_to_centroid(c0_c3_limit) == c3);
	BOOST_TEST(bio1.reproject_to_centroid(W_c0_c3_limit) == c3);
	BOOST_TEST(bio1.reproject_to_centroid(c0_c1_c3_c4_limit) == c4);
	BOOST_CHECK_THROW(bio1.reproject_to_centroid(E_c2_c5_limit), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( Extent )
{
  	using decimal_degree = double;
    using extent_type = quetzal::geography::Extent<decimal_degree>;
  	decimal_degree paris_lat = 48.8566;
  	decimal_degree paris_lon = 2.3522;
  	decimal_degree moscow_lat = 55.7522;
  	decimal_degree moscow_lon = 37.6155;
  	extent_type extent(paris_lat, moscow_lat, paris_lon, moscow_lon);
  	BOOST_CHECK_EQUAL(extent.lat_min() , paris_lat);
    BOOST_CHECK_EQUAL(extent.lat_max() , moscow_lat);
    BOOST_CHECK_EQUAL(extent.lon_min() , paris_lon);
    BOOST_CHECK_EQUAL(extent.lon_max() , moscow_lon );
  	auto copy = extent;
  	copy.lat_min(0.0);
    assert(copy != extent);
  }

BOOST_AUTO_TEST_CASE( GeographicCoordinate )
{
  using coord_type = quetzal::geography::GeographicCoordinates;
  coord_type paris(48.856614, 2.3522219000000177);
  coord_type moscow(55.7522200, 37.6155600 );
  BOOST_TEST(paris < moscow);
  coord_type::km computed = paris.great_circle_distance_to(moscow);
  coord_type::km expected = 2486.22;
  coord_type::km EPSILON = 1.; // a 1-km error is acceptable.
  coord_type::km diff = expected - computed;
  BOOST_TEST(diff < EPSILON);
  BOOST_TEST(-diff < EPSILON);
}

BOOST_AUTO_TEST_CASE( Resolution )
{
  using decimal_degree = double;
	decimal_degree lat_resolution = 2.0;
	decimal_degree lon_resolution = 1.0;
	quetzal::geography::Resolution<decimal_degree> res(lat_resolution, lon_resolution);
	BOOST_TEST(res.lat() == lat_resolution);
  BOOST_TEST(res.lon() == lon_resolution);
	auto copy = res;
	copy.lon(2.0);
	assert(copy != res);
}

BOOST_AUTO_TEST_CASE( gdalcppTest )
{
  using namespace gdalcpp;
  auto path = resolvePath("test/data/bio1.tif");
	gdalcpp::Dataset data(path);
	std::cout << "Name:\t" << data.dataset_name() << std::endl;
	std::cout << "Driver:\t " << data.driver_name() << std::endl;
	std::cout << "Width:\t" << data.width() << std::endl;
	std::cout << "Height:\t" << data.height() << std::endl;
	std::cout << "Depth:\t" << data.depth() << std::endl;
	std::cout << "Coefs:\t";
	for(auto const& it : data.affine_transformation_coefficients()){
			std::cout << it << " ";
	}
	std::cout << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
