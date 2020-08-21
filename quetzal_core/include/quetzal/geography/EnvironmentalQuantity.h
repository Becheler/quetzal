// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __ENVIRONMENTAL_QUANTITY_H_INCLUDED
#define __ENVIRONMENTAL_QUANTITY_H_INCLUDED

#include "gdalcpp.h"
#include "GeographicCoordinates.h"
#include "Resolution.h"
#include "Extent.h"

#include <string>
#include <algorithm>
#include <numeric>      // std::iota
#include <stdexcept>      // std::invalid_argument
#include <cmath>

namespace quetzal {
namespace geography {

/*!
 * \brief Discretized representation of an ecological quantity that varies in space and time
 *
 * The values of an ecological quantity are represented in a geographic explicit context constructed from raster files and gives strong
 * guarantees on data consistency.
 *
 * \tparam Time     Time type
 * \ingroup geography
 * \section Example
 * \snippet geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.cpp Example
 * \section Output
 * \include geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.output
 */
template<typename Time>
class EnvironmentalQuantity : public gdalcpp::Dataset {

  struct XY;

public:

  //! \typedef coordinate type
  using coord_type = GeographicCoordinates;

  //! \typedef time type
  using time_type = Time;

  //! \typedef decimal degree type
  using decimal_degree = typename GeographicCoordinates::decimal_degree;

	//! \typedef value type
	using value_type = double;

	/**
		* \brief Constructor
		*
		* \param dataset_name the raster dataset file to read.
		* \param times a sorted vector giving the times that represents each band of the dataset
		* \remark the size of times argument should be equal to the depth of the given dataset
		* \remark times vector should be sorted
		* \remark values should be defined for same coordinates at all times"
		* \section Example
	  * \snippet geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.cpp Example
	  * \section Output
	  * \include geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.output
		*/
  EnvironmentalQuantity(const std::string& dataset_name, std::vector<time_type> const& times):
  gdalcpp::Dataset(dataset_name),
  m_times(times),
  m_gT(affine_transformation_coefficients()),
  m_origin(compute_origin(m_gT)),
  m_resolution(compute_resolution(m_gT)),
  m_extent(compute_extent(m_origin, m_resolution)),
  m_valid_XY(get_domain(0)),
  m_valid_LonLat(to_lat_lon(m_valid_XY))
  {

    if(times.size() != depth()){
      std::string message("the size of times argument should be equal to the depth of the given dataset: size is ");
      message += std::to_string(times.size());
      message += ", should be ";
      message += std::to_string(depth());
      message += ".";
      throw std::runtime_error(message.c_str());
    }

		if( ! std::is_sorted(m_times.cbegin(), m_times.cend())){
     throw std::runtime_error("times vector should be sorted");
    }

    std::sort(m_valid_LonLat.begin(), m_valid_LonLat.end());

    if( !has_same_domain_at_all_times()){
      throw std::runtime_error("Values should be defined for same coordinates at all times");
    }
  }

	/**
		* \brief Gets the origin of the spatial grid
		*
		* \section Example
	  * \snippet geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.cpp Example
	  * \section Output
	  * \include geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.output
		*/
  const GeographicCoordinates & origin() const {
    return m_origin;
  }

	/**
		* \brief Gets the resolution of the spatial grid
		*
		* \section Example
	  * \snippet geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.cpp Example
	  * \section Output
	  * \include geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.output
		*/
  const Resolution<decimal_degree> & resolution() const {
    return m_resolution;
  }

	/**
		* \brief Gets the extent of the spatial grid
		*
		* \section Example
	  * \snippet geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.cpp Example
	  * \section Output
	  * \include geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.output
		*/
  const Extent<decimal_degree>& extent() const {
    return m_extent;
  }

	/**
		* \brief read the value at position x and time t
		*
		* \remark possibly slow
		* \remark checks if the given time is in the temporal extent
		* \section Example
	  * \snippet geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.cpp Example
	  * \section Output
	  * \include geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.output
		*/
  value_type at(GeographicCoordinates const& c, Time const& t) const {
    auto xy = to_xy(c);
    unsigned int pos = distance(m_times.begin(), find(m_times.begin(), m_times.end(), t));
    assert(pos < m_times.size());
    return read(xy.x, xy.y, pos);
  }

	/**
		* \brief The geographic coordinates of the centroids of the demes for which values are defined at all times.
		*
		* Returns the geographic coordinates of the centroids of the demes for which values are defined at all times.
		* Values can be read from other coordinates, but this function provides a natural
		* way to construct the geographic support for a demic structure (for example for demographic simulations).
		*
		* \section Example
	  * \snippet geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.cpp Example
	  * \section Output
	  * \include geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.output
		*/
  const std::vector<GeographicCoordinates>& geographic_definition_space() const {
    return m_valid_LonLat;
  }

	/**
		* \brief read the times at which quantity is defined.
		*
		* \section Example
	  * \snippet geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.cpp Example
	  * \section Output
	  * \include geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.output
		*/
  const std::vector<time_type>& temporal_definition_space() const {
    return m_times;
  }


	/**
		* \brief Checks if a given geographic coordinate is in the landscape spatial extent
		*
		* \section Example
		* \snippet geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.cpp Example
		* \section Output
		* \include geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.output
		*/
  bool is_in_spatial_extent(GeographicCoordinates const& c) const {
    bool is_in_spatial_extent = false;
    if( c.lon() >= m_extent.lon_min() && c.lon() < m_extent.lon_max() &&
        c.lat() <= m_extent.lat_min() && c.lat() > m_extent.lat_max()     )
    {
      is_in_spatial_extent = true;
    }
    return(is_in_spatial_extent);
  }

	/**
		* \brief Coordinate of the centroid of the deme to which the given coordinate belongs
		*
		* \section Example
		* \snippet geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.cpp Example
		* \section Output
		* \include geography/test/EnvironmentalQuantity/EnvironmentalQuantity_test.output
		*/
  GeographicCoordinates reproject_to_centroid(GeographicCoordinates const& c) const {
    if( ! is_in_spatial_extent(c)){
      throw std::runtime_error("Geographic coordinate should belong to spatial extent to be reprojected");
    }
    auto xy = to_xy(c);
    return to_lat_lon(xy.x, xy.y);
  }


  template<typename F, typename G>
  void export_to_geotiff(F f, G g, time_type const& t1, time_type const& t2, std::string const& filename) const {

    int nBands = std::abs(t2 - t1);
    assert(nBands >= 1);

    // Create a model dataset band
    GDALDataset* p_sink = get().GetDriver()->Create(filename.c_str(), width(), height(), nBands, GDT_Float32, NULL);
    std::vector<float> buffer(width()*height());

    GDALRasterBand* p_source_band = get().GetRasterBand(1);
    double na = p_source_band->GetNoDataValue();

    // Modify data
    for(int i = 1; i <= nBands; ++i ){


      auto err = p_source_band->RasterIO( GF_Read, 0, 0, width(), height(), buffer.data(), width(), height(), GDT_Float32, 0, 0 );
      (void)err;

      //replace all continental cells value by 0
      auto continental_cells = get_domain(0);
      for(auto const& it : continental_cells){
        buffer.at(it.x + it.y*width()) = 0.0;
      }

      // Replace all cells where F is defined by F
      auto t_curr = t1 + i-1;
      auto f_defined_cells = g(t_curr);

      for(coord_type const& it: f_defined_cells){
        auto xy = to_xy(it);
        buffer.at(xy.x  + (xy.y*width()) ) = static_cast<float>(f(it, t_curr));
      }

      //write the data
      GDALRasterBand* p_sink_band = p_sink->GetRasterBand(i);
      p_sink_band->SetNoDataValue(na);
      auto err2 = p_sink_band->RasterIO( GF_Write, 0, 0, width(), height(), buffer.data(), width(), height(), GDT_Float32, 0, 0 );
      (void)err2;
      p_sink->FlushCache();

    }

    // write metadata
    std::vector<double> geo_transform(6);
    get().GetGeoTransform( geo_transform.data() );
    p_sink->SetGeoTransform( geo_transform.data() );
    p_sink->SetProjection( get().GetProjectionRef() );

    GDALClose( (GDALDatasetH) p_sink );
  }

  void export_to_shapefile(std::map<coord_type, unsigned int> counts, std::string const& filename) const {
    std::string driver_name = "ESRI Shapefile";
    GDALDriver *poDriver;
    GDALAllRegister();
    poDriver = GetGDALDriverManager()->GetDriverByName(driver_name.c_str() );
    assert( poDriver != NULL);

    GDALDataset *poDS;
    poDS = poDriver->Create( filename.c_str(), 0, 0, 0, GDT_Unknown, NULL );
    assert( poDS != NULL );

    OGRLayer *poLayer;
    poLayer = poDS->CreateLayer( "sample", NULL, wkbPoint, NULL );
    assert( poLayer != NULL );

    OGRFieldDefn oField( "count", OFTInteger );
    if( poLayer->CreateField( &oField ) != OGRERR_NONE ){
      throw(std::string("Creating sample size field failed."));
    }

    for(auto const& it: counts){
      double x = it.first.lon();
      double y = it.first.lat();
      int sample_size = it.second;

      OGRFeature *poFeature;
      poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );

      poFeature->SetField( "count", sample_size );

      OGRPoint pt;
      pt.setX( x );
      pt.setY( y );

      poFeature->SetGeometry( &pt );

      if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE ){
        throw(std::string("Failed to create feature in shapefile."));
      }
      OGRFeature::DestroyFeature( poFeature );
    }
    GDALClose( poDS );
  }

  void export_to_shapefile(std::vector<coord_type> coords, std::string const& filename) const {
    std::string driver_name = "ESRI Shapefile";
    GDALDriver *poDriver;
    GDALAllRegister();
    poDriver = GetGDALDriverManager()->GetDriverByName(driver_name.c_str() );
    assert( poDriver != NULL);

    GDALDataset *poDS;
    poDS = poDriver->Create( filename.c_str(), 0, 0, 0, GDT_Unknown, NULL );
    assert( poDS != NULL );

    OGRLayer *poLayer;
    poLayer = poDS->CreateLayer( "sample", NULL, wkbPoint, NULL );
    assert( poLayer != NULL );

    for(auto const& it: coords){
      double x = it.lon();
      double y = it.lat();

      OGRFeature *poFeature;
      poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );

      OGRPoint pt;
      pt.setX( x );
      pt.setY( y );

      poFeature->SetGeometry( &pt );

      if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE ){
        throw(std::string("Failed to create feature in shapefile."));
      }
      OGRFeature::DestroyFeature( poFeature );
    }
    GDALClose( poDS );
  }


private:

  std::vector<time_type> m_times;
  std::vector<double> m_gT;
  GeographicCoordinates m_origin;
  Resolution<decimal_degree> m_resolution;
  Extent<decimal_degree> m_extent;
  std::vector<XY> m_valid_XY;
  std::vector<GeographicCoordinates> m_valid_LonLat;

  struct XY{

    XY(unsigned int col, unsigned int row) : x(col), y(row){}

    unsigned int x;
    unsigned int y;

    bool operator==(const XY& other) const {
        if(x == other.x && y == other.y) return true;
        return false;
      }
  };

  // Convert latitude/longitude to col/row indices
  XY to_xy(GeographicCoordinates const& c) const {
    int col = (c.lon() - m_gT[0]) / m_gT[1];
    int row = (c.lat() - m_gT[3]) / m_gT[5];
    return XY(col, row);
  }

  std::vector<XY> get_domain(unsigned int bandID) const {
    assert( bandID < depth());
    std::vector<XY> v;
    for(unsigned int x = 0; x < width(); ++x){
      for( unsigned int y = 0; y < height(); ++y){
        if( read(x, y, bandID) > std::numeric_limits<value_type>::min() ){
          v.emplace_back(x,y);
        }
      }
    }
    return v;
  }

  // Read 1 value using GDAL API. To change if access is too slow
  double read(unsigned int x, unsigned int y, unsigned int bandID) const {
    int nXSize = 1;
    auto line = (float *) CPLMalloc(sizeof(float)*nXSize);
    auto err = band(bandID).RasterIO( GF_Read, x, y, 1, 1, line, nXSize, 1, GDT_Float32, 0, 0 );
    if(err != CE_None){
      throw std::invalid_argument( "received negative x y or band ID" );
    };
    double val = double(*line);
    CPLFree(line);
    return(val);
  }

  // Convert row x and col y to geographic coordinate
  GeographicCoordinates to_lat_lon(unsigned int x, unsigned int y) const {
    auto lon = m_gT[1] * x + m_gT[2] * y + m_gT[1] * 0.5 + m_gT[2] * 0.5 + m_gT[0];
    auto lat = m_gT[4] * x + m_gT[5] * y + m_gT[4] * 0.5 + m_gT[5] * 0.5 + m_gT[3];
    return GeographicCoordinates(lat, lon);
  }

  // Geographic coordinates of the top left corner.
  GeographicCoordinates compute_origin(std::vector<double> const& gT) const {
    return GeographicCoordinates(gT[3], gT[0]) ;
  }

  // Pixel size in x and y
  Resolution<decimal_degree> compute_resolution(std::vector<double> const& gT) const {
      return Resolution<decimal_degree>(gT[5], gT[1]);
  }

  // Top left corner and bottom right corner geographic coordinates
  Extent<decimal_degree> compute_extent(GeographicCoordinates const& o, Resolution<decimal_degree> const& r) const {
    auto lon_max = o.lon() + width() * r.lon();
    auto lat_max = o.lat() + height() * r.lat();
    return Extent<decimal_degree>(o.lat(), lat_max, o.lon(), lon_max);
  }

  std::vector<GeographicCoordinates> to_lat_lon(std::vector<XY> const& xy) const {
    std::vector<GeographicCoordinates> out;
    for(auto const& it : xy){
      out.push_back(to_lat_lon(it.x, it.y));
    }
    return out;
}

  bool has_same_domain_at_all_times(){
    bool answer = true;
    if(depth()>1){
      std::vector<unsigned int> bands(depth()-1);
      std::iota(bands.begin(), bands.end(), 1);
      auto s_0 = get_domain(0);
      auto have_same_domain = [this, &s_0](unsigned int i){
        auto s_i = this->get_domain(i);
        return std::equal(s_0.begin(), s_0.end(), s_i.begin());
      };
      answer = std::all_of(bands.begin(), bands.end(), have_same_domain);
    }
    return answer;
  }

};

} // namespace quetzal
} // namespace geography

#endif
