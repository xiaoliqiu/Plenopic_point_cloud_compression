/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2017, ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "PCCCommon.h"

#include "PCCPointSet.h"
#include "PCCMath.h"
#include "KDTreeVectorOfVectorsAdaptor.h"
#include "PCCKdTree.h"
#include <numeric>

using namespace pcc;

void PCCPointSet3::removeDuplicate() {
  PCCPointSet3 newPointcloud;
  if ( withColors_ ) { newPointcloud.hasColors(); }
  if ( withReflectances_ ) { newPointcloud.addReflectances(); }
  std::map<float, std::map<float, std::map<float, size_t>>> eMapA, eMapB;
  // size_t duplicate = 0;
  if ( withColors_ ) {
    for ( size_t i = 0; i < positions_.size(); ++i ) {
      float x = positions_[i][0], y = positions_[i][1], z = positions_[i][2];
      if ( eMapA.find( x ) != eMapA.end() && eMapA[x].find( y ) != eMapA[x].end() &&
           eMapA[x][y].find( z ) != eMapA[x][y].end() ) {
        // duplicate++;
      } else {
        eMapA[x][y][z] = i;
#if MULTIPLE_ATTRIBUTES
        PCCColor3B pointCand[NUMBER_VIEWS];
        for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
          pointCand[viewIndex] = colors_[viewIndex][i];
        }
        newPointcloud.addPoint( positions_[i], pointCand );
#else
        newPointcloud.addPoint( positions_[i], colors_[i] );
#endif
      }
    }
  } else {
    for ( size_t i = 0; i < positions_.size(); ++i ) {
      float x = positions_[i][0], y = positions_[i][1], z = positions_[i][2];
      if ( eMapA.find( x ) != eMapA.end() && eMapA[x].find( y ) != eMapA[x].end() &&
           eMapA[x][y].find( z ) != eMapA[x][y].end() ) {
        // duplicate++;
      } else {
        eMapA[x][y][z] = i;
        newPointcloud.addPoint( positions_[i] );
      }
    }
  }
  positions_.swap( newPointcloud.positions_ );
#if MULTIPLE_ATTRIBUTES
  for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
    colors_[viewIndex].swap( newPointcloud.colors_[viewIndex] );
  }
#else
  colors_.swap( newPointcloud.colors_ );
#endif
  reflectances_.swap( newPointcloud.reflectances_ );
  types_.swap( newPointcloud.types_ );
}

void PCCPointSet3::distanceGeo( const PCCPointSet3& pointcloud, float& distPAB, float& distPBA ) const {
  this->distance( pointcloud, distPAB );
  pointcloud.distance( *this, distPBA );
}

void PCCPointSet3::distanceGeoColor( const PCCPointSet3& pointcloud,
                                     float&              distPAB,
                                     float&              distPBA,
                                     float&              distYAB,
                                     float&              distYBA,
                                     float&              distUAB,
                                     float&              distUBA,
                                     float&              distVAB,
                                     float&              distVBA ) const {
  this->distance( pointcloud, distPAB, distYAB, distUAB, distVAB );
  pointcloud.distance( *this, distPBA, distYBA, distUBA, distVBA );
}
void convertRGBtoYUV_BT709( const PCCColor3B& in_rgb, float* out_yuv ) {
  // color space conversion to YUV
  out_yuv[0] = ( 0.2126f * in_rgb[0] + 0.7152f * in_rgb[1] + 0.0722f * in_rgb[2] ) / 255.0f;
  out_yuv[1] = ( -0.1146f * in_rgb[0] - 0.3854f * in_rgb[1] + 0.5000f * in_rgb[2] ) / 255.0f + 0.5000f;
  out_yuv[2] = ( 0.5000f * in_rgb[0] - 0.4542f * in_rgb[1] - 0.0458f * in_rgb[2] ) / 255.0f + 0.5000f;
}
void PCCPointSet3::distance( const PCCPointSet3& pointcloud,
                             float&              distP,
                             float&              distY,
                             float&              distU,
                             float&              distV ) const {
  distP = 0.f;
  distY = 0.f;
  distU = 0.f;
  distV = 0.f;
  PCCKdTree   kdtree( pointcloud );
  PCCNNResult result;

  for ( size_t i = 0; i < positions_.size(); ++i ) {
    kdtree.search( positions_[i], 1, result );
    distP += result.dist( 0 );
    float yuvA[3], yuvB[3];
#if MULTIPLE_ATTRIBUTES
    // view 0 is used as the representative
    convertRGBtoYUV_BT709( colors_[0][i], yuvA );
    convertRGBtoYUV_BT709( pointcloud.colors_[0][result.indices( 0 )], yuvB );
    distY += pow( yuvA[0] - yuvB[0], 2.f );
    distU += pow( yuvA[1] - yuvB[1], 2.f );
    distV += pow( yuvA[2] - yuvB[2], 2.f );
#else
    convertRGBtoYUV_BT709( colors_[i], yuvA );
    convertRGBtoYUV_BT709( pointcloud.colors_[result.indices( 0 )], yuvB );
    distY += pow( yuvA[0] - yuvB[0], 2.f );
    distU += pow( yuvA[1] - yuvB[1], 2.f );
    distV += pow( yuvA[2] - yuvB[2], 2.f );
#endif
  }
  distP /= (float)( positions_.size() );

  distY /= (float)( positions_.size() );
  distU /= (float)( positions_.size() );
  distV /= (float)( positions_.size() );
}

void PCCPointSet3::distance( const PCCPointSet3& pointcloud, float& distP ) const {
  distP = 0.f;
  PCCKdTree   kdtree( pointcloud );
  PCCNNResult result;
  for ( size_t i = 0; i < positions_.size(); ++i ) {
    kdtree.search( positions_[i], 1, result );
    distP += result.dist( 0 );
  }
  distP /= (float)( positions_.size() );
}

PCCPoint3D PCCPointSet3::computeCentroid() const {
  PCCPoint3D   bary( 0.0 );
  const size_t pointCount = getPointCount();
  if ( pointCount ) {
    for ( size_t i = 0; i < pointCount; ++i ) {
      const PCCPoint3D& pt = ( *this )[i];
      bary += pt;
    }
    bary /= double( pointCount );
  }
  return bary;
}

PCCBox3D PCCPointSet3::computeBoundingBox() const {
  PCCBox3D     bbox       = {( std::numeric_limits<double>::max )(), ( std::numeric_limits<double>::lowest )()};
  const size_t pointCount = getPointCount();
  for ( size_t i = 0; i < pointCount; ++i ) {
    const PCCPoint3D& pt = ( *this )[i];
    for ( int k = 0; k < 3; ++k ) {
      if ( pt[k] > bbox.max_[k] ) { bbox.max_[k] = pt[k]; }
      if ( pt[k] < bbox.min_[k] ) { bbox.min_[k] = pt[k]; }
    }
  }
  return bbox;
}

void PCCPointSet3::removeDuplicate( PCCPointSet3& newPointcloud, size_t dropDuplicates ) const {
  if ( withColors_ ) { newPointcloud.hasColors(); }
  if ( withReflectances_ ) { newPointcloud.addReflectances(); }
  if ( withNormals_ ) {
    std::cerr << "Normaled objects can't be modified or reordered \n" << std::endl;
    exit( -1 );
  }
  std::map<float, std::map<float, std::map<float, std::vector<size_t>>>> map;
  for ( size_t i = 0; i < positions_.size(); ++i ) {
    float x = positions_[i][0], y = positions_[i][1], z = positions_[i][2];
    map[x][y][z].push_back( i );
  }
  if ( withColors_ ) {
    for ( auto itX = map.begin(); itX != map.end(); itX++ ) {
      for ( auto itY = itX->second.begin(); itY != itX->second.end(); itY++ ) {
        for ( auto itZ = itY->second.begin(); itZ != itY->second.end(); itZ++ ) {
          auto& listIndex = itZ->second;
          if ( listIndex.size() == 1 || dropDuplicates == 1 ) {
#if MULTIPLE_ATTRIBUTES
            PCCColor3B pointCand[NUMBER_VIEWS];
            for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
              pointCand[viewIndex] = colors_[viewIndex][listIndex[0]];
            }
            newPointcloud.addPoint( positions_[listIndex[0]], pointCand );
#else
            newPointcloud.addPoint( positions_[listIndex[0]], colors_[listIndex[0]] );
#endif
          } else {
#if MULTIPLE_ATTRIBUTES
            PCCColor3B average[NUMBER_VIEWS];
            for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
              size_t r = 0, g = 0, b = 0;
              for ( auto& index : listIndex ) {
                r += colors_[viewIndex][index][0];
                g += colors_[viewIndex][index][1];
                b += colors_[viewIndex][index][2];
              }
              average[viewIndex][0] = r / listIndex.size();
              average[viewIndex][1] = g / listIndex.size();
              average[viewIndex][2] = b / listIndex.size();
            }
            newPointcloud.addPoint( positions_[listIndex[0]], average );
#else
            PCCColor3B average;
            size_t     r = 0, g = 0, b = 0;
            for ( auto& index : listIndex ) {
              r += colors_[index][0];
              g += colors_[index][1];
              b += colors_[index][2];
            }
            average[0] = r / listIndex.size();
            average[1] = g / listIndex.size();
            average[2] = b / listIndex.size();
            newPointcloud.addPoint( positions_[listIndex[0]], average );
#endif
          }
        }
      }
    }
  } else {
    for ( auto itX = map.begin(); itX != map.end(); itX++ ) {
      for ( auto itY = itX->second.begin(); itY != itX->second.end(); itY++ ) {
        for ( auto itZ = itY->second.begin(); itZ != itY->second.end(); itZ++ ) {
          auto& listIndex = itZ->second;
          newPointcloud.addPoint( positions_[listIndex[0]] );
        }
      }
    }
  }
}

typedef unsigned int UInt;
#include "MD5.h"
std::vector<uint8_t> PCCPointSet3::computeChecksum( bool reorderPoints ) {
  if ( reorderPoints ) {
    PCCPointSet3 reorderPointCloud;
    if ( withColors_ ) { reorderPointCloud.hasColors(); }
    if ( withReflectances_ ) { reorderPointCloud.addReflectances(); }
    reorder( reorderPointCloud, true );
    return reorderPointCloud.computeMd5();
  } else {
    return computeMd5();
  }
}
std::vector<uint8_t> PCCPointSet3::computeMd5() {
  std::vector<uint8_t> digest;
  MD5                  md5;
  md5.update( (uint8_t*)positions_.data(), positions_.size() * sizeof( PCCPoint3D ) );
#if MULTIPLE_ATTRIBUTES
  if ( withColors_ ) {
    for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
      md5.update( (uint8_t*)colors_[viewIndex].data(), colors_[viewIndex].size() * sizeof( PCCColor3B ) );
    }
  }
#else
  if ( withColors_ ) { md5.update( (uint8_t*)colors_.data(), colors_.size() * sizeof( PCCColor3B ) ); }
#endif
  if ( withReflectances_ ) { md5.update( (uint8_t*)reflectances_.data(), reflectances_.size() * sizeof( uint16_t ) ); }
  digest.resize( MD5_DIGEST_STRING_LENGTH );
  md5.finalize( digest.data() );
  return digest;
}

void PCCPointSet3::sortColor( std::vector<size_t>& list ) {
  for ( size_t i = 0; i < list.size(); i++ ) {
    size_t indexMin = i;
    for ( size_t j = i + 1; j < list.size(); j++ ) {
#if MULTIPLE_ATTRIBUTES
      // sort based on the view 0
      if ( colors_[0][list[j]] < colors_[0][list[indexMin]] ) { indexMin = j; }
#else
      if ( colors_[list[j]] < colors_[list[indexMin]] ) { indexMin = j; }
#endif
    }
    if ( i != indexMin ) {
      size_t tmp     = list[indexMin];
      list[indexMin] = list[i];
      list[i]        = tmp;
    }
  }
}

void PCCPointSet3::reorder( PCCPointSet3& newPointcloud, bool dropDuplicates ) {
  std::map<float, std::map<float, std::map<float, std::vector<size_t>>>> map;
  size_t                                                                 duplicate = 0;
  for ( size_t i = 0; i < positions_.size(); ++i ) {
    float x = positions_[i][0], y = positions_[i][1], z = positions_[i][2];
    if ( map.find( x ) != map.end() && map[x].find( y ) != map[x].end() && map[x][y].find( z ) != map[x][y].end() ) {
      duplicate++;
    }
    map[x][y][z].push_back( i );
  }
  if ( withColors_ ) {
    for ( auto itX = map.begin(); itX != map.end(); itX++ ) {
      for ( auto itY = itX->second.begin(); itY != itX->second.end(); itY++ ) {
        for ( auto itZ = itY->second.begin(); itZ != itY->second.end(); itZ++ ) {
          auto& listIndex = itZ->second;
          if ( listIndex.size() > 1 ) { sortColor( listIndex ); }
          if ( dropDuplicates ) {
#if MULTIPLE_ATTRIBUTES
            PCCColor3B average[NUMBER_VIEWS];
            for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
              size_t r = 0, g = 0, b = 0;
              for ( auto& index : listIndex ) {
                r += colors_[viewIndex][index][0];
                g += colors_[viewIndex][index][1];
                b += colors_[viewIndex][index][2];
              }
              average[viewIndex][0] = r / listIndex.size();
              average[viewIndex][1] = g / listIndex.size();
              average[viewIndex][2] = b / listIndex.size();
            }
            newPointcloud.addPoint( positions_[listIndex[0]], average );
#else
            PCCColor3B average;
            size_t     r = 0, g = 0, b = 0;
            for ( auto& index : listIndex ) {
              r += colors_[index][0];
              g += colors_[index][1];
              b += colors_[index][2];
            }
            average[0] = r / listIndex.size();
            average[1] = g / listIndex.size();
            average[2] = b / listIndex.size();
            newPointcloud.addPoint( positions_[listIndex[0]], average );
#endif
          } else {
#if MULTIPLE_ATTRIBUTES
            for ( auto& index : listIndex ) {
              PCCColor3B pointCand[NUMBER_VIEWS];
              for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
                pointCand[viewIndex] = colors_[viewIndex][index];
              }
              newPointcloud.addPoint( positions_[index], pointCand );
            }
#else
            for ( auto& index : listIndex ) { newPointcloud.addPoint( positions_[index], colors_[index] ); }
#endif
          }
        }
      }
    }
  } else {
    for ( auto itX = map.begin(); itX != map.end(); itX++ ) {
      for ( auto itY = itX->second.begin(); itY != itX->second.end(); itY++ ) {
        for ( auto itZ = itY->second.begin(); itZ != itY->second.end(); itZ++ ) {
          auto& listIndex = itZ->second;
          for ( auto& index : listIndex ) { newPointcloud.addPoint( positions_[index] ); }
        }
      }
    }
  }
}

void PCCPointSet3::reorder() {
  PCCPointSet3 newPointcloud;
  if ( withColors_ ) { newPointcloud.hasColors(); }
  if ( withReflectances_ ) { newPointcloud.addReflectances(); }
  reorder( newPointcloud, false );
  swap( newPointcloud );
}
void PCCPointSet3::swap( PCCPointSet3& newPointcloud ) {
  positions_.swap( newPointcloud.positions_ );
#if MULTIPLE_ATTRIBUTES
  for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
    colors_[viewIndex].swap( newPointcloud.colors_[viewIndex] );
  }
#else
  colors_.swap( newPointcloud.colors_ );
#endif
  reflectances_.swap( newPointcloud.reflectances_ );
  types_.swap( newPointcloud.types_ );
  normals_.swap( newPointcloud.normals_ );
}
bool PCCPointSet3::isBboxEmpty( PCCBox3D bbox ) const {
  const size_t pointCount = getPointCount();
  for ( size_t i = 0; i < pointCount; ++i ) {
    const PCCPoint3D& pt = ( *this )[i];
    if ( ( pt[0] >= bbox.min_[0] && pt[0] <= bbox.max_[0] ) && ( pt[1] >= bbox.min_[1] && pt[1] <= bbox.max_[1] ) &&
         ( pt[2] >= bbox.min_[2] && pt[2] <= bbox.max_[2] ) ) {
      return false;
    }
  }
  return true;
}

int PCCPointSet3::fillMissedPointsBbox( std::vector<size_t>  missedPoints,
                                        PCCBox3D             bbox,
                                        std::vector<size_t>& bboxMissedPoints ) const {
  const size_t MissedPointCount = missedPoints.size();

  for ( size_t i = 0; i < MissedPointCount; ++i ) {
    const PCCPoint3D& pt = ( *this )[missedPoints[i]];
    if ( ( pt[0] >= bbox.min_[0] && pt[0] <= bbox.max_[0] ) && ( pt[1] >= bbox.min_[1] && pt[1] <= bbox.max_[1] ) &&
         ( pt[2] >= bbox.min_[2] && pt[2] <= bbox.max_[2] ) ) {
      bboxMissedPoints.push_back( missedPoints[i] );
    }
  }
  return bboxMissedPoints.size();
}

bool PCCPointSet3::isMissedBboxEmpty( std::vector<size_t> missedPoints, PCCBox3D bbox ) const {
  const size_t MissedPointCount = missedPoints.size();

  for ( size_t i = 0; i < MissedPointCount; ++i ) {
    const PCCPoint3D& pt = ( *this )[missedPoints[i]];
    if ( ( pt[0] >= bbox.min_[0] && pt[0] <= bbox.max_[0] ) && ( pt[1] >= bbox.min_[1] && pt[1] <= bbox.max_[1] ) &&
         ( pt[2] >= bbox.min_[2] && pt[2] <= bbox.max_[2] ) ) {
      return false;
    }
  }
  return true;
}

bool PCCPointSet3::write( const std::string& fileName, const bool asAscii ) {
  std::ofstream fout( fileName, std::ofstream::out );
  if ( !fout.is_open() ) { return false; }
  const size_t pointCount = getPointCount();
  fout << "ply" << std::endl;

  if ( asAscii ) {
    fout << "format ascii 1.0" << std::endl;
  } else {
    PCCEndianness endianess = PCCSystemEndianness();
    if ( endianess == PCC_BIG_ENDIAN ) {
      fout << "format binary_big_endian 1.0" << std::endl;
    } else {
      fout << "format binary_little_endian 1.0" << std::endl;
    }
  }
  fout << "element vertex " << pointCount << std::endl;
  if ( asAscii ) {
    fout << "property float x" << std::endl;
    fout << "property float y" << std::endl;
    fout << "property float z" << std::endl;
  } else {
    fout << "property int16 x" << std::endl;
    fout << "property int16 y" << std::endl;
    fout << "property int16 z" << std::endl;
  }
  if ( hasColors() ) {
    fout << "property uchar red" << std::endl;
    fout << "property uchar green" << std::endl;
    fout << "property uchar blue" << std::endl;
  }
  if ( hasReflectances() ) { fout << "property uint16 refc" << std::endl; }
  if ( PCC_SAVE_POINT_TYPE ) {
    fout << "property uchar type" << std::endl;
    switch ( PCC_SAVE_POINT_TYPE ) {
      case 1: fout << "comment POINT_TYPE: Unset D0 D1 Filling Smooth InBetween" << std::endl; break;
      case 2: fout << "comment POINT_TYPE: type0 type1 type2  " << std::endl; break;
      default: break;
    }
  }
  fout << "element face 0" << std::endl;
  fout << "property list uint8 int32 vertex_index" << std::endl;
  fout << "end_header" << std::endl;
  if ( asAscii ) {
    fout << std::setprecision( std::numeric_limits<double>::max_digits10 );
    for ( size_t i = 0; i < pointCount; ++i ) {
      const PCCPoint3D& position = ( *this )[i];
      fout << position.x() << " " << position.y() << " " << position.z();
      if ( hasColors() ) {
#if MULTIPLE_ATTRIBUTES
        for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
          const PCCColor3B& color = getColor( i, viewIndex );
          fout << " " << static_cast<int>( color[0] ) << " " << static_cast<int>( color[1] ) << " "
               << static_cast<int>( color[2] );
        }
#else
        const PCCColor3B& color = getColor( i );
        fout << " " << static_cast<int>( color[0] ) << " " << static_cast<int>( color[1] ) << " "
             << static_cast<int>( color[2] );
#endif
      }
      if ( hasReflectances() ) { fout << " " << static_cast<int>( getReflectance( i ) ); }
      if ( PCC_SAVE_POINT_TYPE ) { fout << " " << static_cast<int>( types_[i] ); }
      fout << std::endl;
    }
  } else {
    fout.clear();
    fout.close();
    fout.open( fileName, std::ofstream::binary | std::ofstream::out );
    for ( size_t i = 0; i < pointCount; ++i ) {
      const PCCPoint3D& position = ( *this )[i];
      fout.write( reinterpret_cast<const char* const>( &position ), sizeof( PCCType ) * 3 );
      if ( hasColors() ) {
#if MULTIPLE_ATTRIBUTES
        for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
          const PCCColor3B& color = getColor( i, viewIndex );
          fout.write( reinterpret_cast<const char*>( &color ), sizeof( uint8_t ) * 3 );
        }
#else
        const PCCColor3B& color = getColor( i );
        fout.write( reinterpret_cast<const char*>( &color ), sizeof( uint8_t ) * 3 );
#endif
      }
      if ( hasReflectances() ) {
        const uint16_t& reflectance = getReflectance( i );
        fout.write( reinterpret_cast<const char*>( &reflectance ), sizeof( uint16_t ) );
      }
      if ( PCC_SAVE_POINT_TYPE ) { fout.write( reinterpret_cast<const char* const>( &types_[i] ), sizeof( uint8_t ) ); }
    }
  }
  fout.close();
  return true;
}

#if ELEVEN_BITS
bool comparator( const std::pair<size_t, size_t>& l, const std::pair<size_t, size_t>& r ) { return l.first < r.first; }
#endif

bool PCCPointSet3::read( const std::string& fileName, const bool readNormals ) {
  std::ifstream ifs( fileName, std::ifstream::in );
  if ( !ifs.is_open() ) { return false; }
  enum AttributeType {
    ATTRIBUTE_TYPE_FLOAT64 = 0,
    ATTRIBUTE_TYPE_FLOAT32 = 1,
    ATTRIBUTE_TYPE_UINT64  = 2,
    ATTRIBUTE_TYPE_UINT32  = 3,
    ATTRIBUTE_TYPE_UINT16  = 4,
    ATTRIBUTE_TYPE_UINT8   = 5,
    ATTRIBUTE_TYPE_INT64   = 6,
    ATTRIBUTE_TYPE_INT32   = 7,
    ATTRIBUTE_TYPE_INT16   = 8,
    ATTRIBUTE_TYPE_INT8    = 9,
  };
  struct AttributeInfo {
    std::string   name;
    AttributeType type;
    size_t        byteCount;
  };

  std::vector<AttributeInfo> attributesInfo;
  attributesInfo.reserve( 16 );
  const size_t             MAX_BUFFER_SIZE = 4096;
  char                     tmp[MAX_BUFFER_SIZE];
  const char*              sep = " \t\r";
  std::vector<std::string> tokens;

  ifs.getline( tmp, MAX_BUFFER_SIZE );
  getTokens( tmp, sep, tokens );
  if ( tokens.empty() || tokens[0] != "ply" ) {
    std::cout << "Error: corrupted file!" << std::endl;
    return false;
  }
  bool   isAscii          = false;
  double version          = 1.0;
  size_t pointCount       = 0;
  bool   isVertexProperty = true;

  while ( 1 ) {
    if ( ifs.eof() ) {
      std::cout << "Error: corrupted header!" << std::endl;
      return false;
    }
    ifs.getline( tmp, MAX_BUFFER_SIZE );
    getTokens( tmp, sep, tokens );
    if ( tokens.empty() || tokens[0] == "comment" ) { continue; }
    if ( tokens[0] == "format" ) {
      if ( tokens.size() != 3 ) {
        std::cout << "Error: corrupted format info!" << std::endl;
        return false;
      }
      isAscii = tokens[1] == "ascii";
      version = atof( tokens[2].c_str() );
    } else if ( tokens[0] == "element" ) {
      if ( tokens.size() != 3 ) {
        std::cout << "Error: corrupted element info!" << std::endl;
        return false;
      }
      if ( tokens[1] == "vertex" ) {
        pointCount = atoi( tokens[2].c_str() );
      } else {
        isVertexProperty = false;
      }
    } else if ( tokens[0] == "property" && isVertexProperty ) {
      if ( tokens.size() != 3 ) {
        std::cout << "Error: corrupted property info!" << std::endl;
        return false;
      }
      const std::string& propertyType   = tokens[1];
      const std::string& propertyName   = tokens[2];
      const size_t       attributeIndex = attributesInfo.size();
      attributesInfo.resize( attributeIndex + 1 );
      AttributeInfo& attributeInfo = attributesInfo[attributeIndex];
      attributeInfo.name           = propertyName;
      if ( propertyType == "float64" ) {
        attributeInfo.type      = ATTRIBUTE_TYPE_FLOAT64;
        attributeInfo.byteCount = 8;
      } else if ( propertyType == "float" || propertyType == "float32" ) {
        attributeInfo.type      = ATTRIBUTE_TYPE_FLOAT32;
        attributeInfo.byteCount = 4;
      } else if ( propertyType == "uint64" ) {
        attributeInfo.type      = ATTRIBUTE_TYPE_UINT64;
        attributeInfo.byteCount = 8;
      } else if ( propertyType == "uint32" ) {
        attributeInfo.type      = ATTRIBUTE_TYPE_UINT32;
        attributeInfo.byteCount = 4;
      } else if ( propertyType == "uint16" ) {
        attributeInfo.type      = ATTRIBUTE_TYPE_UINT16;
        attributeInfo.byteCount = 2;
      } else if ( propertyType == "uchar" || propertyType == "uint8" ) {
        attributeInfo.type      = ATTRIBUTE_TYPE_UINT8;
        attributeInfo.byteCount = 1;
      } else if ( propertyType == "int64" ) {
        attributeInfo.type      = ATTRIBUTE_TYPE_INT64;
        attributeInfo.byteCount = 8;
      } else if ( propertyType == "int32" ) {
        attributeInfo.type      = ATTRIBUTE_TYPE_INT32;
        attributeInfo.byteCount = 4;
      } else if ( propertyType == "int16" ) {
        attributeInfo.type      = ATTRIBUTE_TYPE_INT16;
        attributeInfo.byteCount = 2;
      } else if ( propertyType == "char" || propertyType == "int8" ) {
        attributeInfo.type      = ATTRIBUTE_TYPE_INT8;
        attributeInfo.byteCount = 1;
      }
    } else if ( tokens[0] == "end_header" ) {
      break;
    }
  }
  if ( version != 1.0 ) {
    std::cout << "Error: non-supported version!" << std::endl;
    return false;
  }

  size_t indexX           = PCC_UNDEFINED_INDEX;
  size_t indexY           = PCC_UNDEFINED_INDEX;
  size_t indexZ           = PCC_UNDEFINED_INDEX;
  size_t indexR           = PCC_UNDEFINED_INDEX;
  size_t indexG           = PCC_UNDEFINED_INDEX;
  size_t indexB           = PCC_UNDEFINED_INDEX;
  size_t indexReflectance = PCC_UNDEFINED_INDEX;
  size_t indexNX          = PCC_UNDEFINED_INDEX;
  size_t indexNY          = PCC_UNDEFINED_INDEX;
  size_t indexNZ          = PCC_UNDEFINED_INDEX;
#if MULTIPLE_ATTRIBUTES
  size_t indexR0  = PCC_UNDEFINED_INDEX;
  size_t indexG0  = PCC_UNDEFINED_INDEX;
  size_t indexB0  = PCC_UNDEFINED_INDEX;
  size_t indexR1  = PCC_UNDEFINED_INDEX;
  size_t indexG1  = PCC_UNDEFINED_INDEX;
  size_t indexB1  = PCC_UNDEFINED_INDEX;
  size_t indexR2  = PCC_UNDEFINED_INDEX;
  size_t indexG2  = PCC_UNDEFINED_INDEX;
  size_t indexB2  = PCC_UNDEFINED_INDEX;
  size_t indexR3  = PCC_UNDEFINED_INDEX;
  size_t indexG3  = PCC_UNDEFINED_INDEX;
  size_t indexB3  = PCC_UNDEFINED_INDEX;
  size_t indexR4  = PCC_UNDEFINED_INDEX;
  size_t indexG4  = PCC_UNDEFINED_INDEX;
  size_t indexB4  = PCC_UNDEFINED_INDEX;
  size_t indexR5  = PCC_UNDEFINED_INDEX;
  size_t indexG5  = PCC_UNDEFINED_INDEX;
  size_t indexB5  = PCC_UNDEFINED_INDEX;
  size_t indexR6  = PCC_UNDEFINED_INDEX;
  size_t indexG6  = PCC_UNDEFINED_INDEX;
  size_t indexB6  = PCC_UNDEFINED_INDEX;
  size_t indexR7  = PCC_UNDEFINED_INDEX;
  size_t indexG7  = PCC_UNDEFINED_INDEX;
  size_t indexB7  = PCC_UNDEFINED_INDEX;
  size_t indexR8  = PCC_UNDEFINED_INDEX;
  size_t indexG8  = PCC_UNDEFINED_INDEX;
  size_t indexB8  = PCC_UNDEFINED_INDEX;
  size_t indexR9  = PCC_UNDEFINED_INDEX;
  size_t indexG9  = PCC_UNDEFINED_INDEX;
  size_t indexB9  = PCC_UNDEFINED_INDEX;
  size_t indexR10 = PCC_UNDEFINED_INDEX;
  size_t indexG10 = PCC_UNDEFINED_INDEX;
  size_t indexB10 = PCC_UNDEFINED_INDEX;
  size_t indexR11 = PCC_UNDEFINED_INDEX;
  size_t indexG11 = PCC_UNDEFINED_INDEX;
  size_t indexB11 = PCC_UNDEFINED_INDEX;
#if NUMBER_VIEWS == 13
  size_t indexR12 = PCC_UNDEFINED_INDEX;
  size_t indexG12 = PCC_UNDEFINED_INDEX;
  size_t indexB12 = PCC_UNDEFINED_INDEX;
#endif
#endif
  const size_t attributeCount = attributesInfo.size();
  for ( size_t a = 0; a < attributeCount; ++a ) {
    const auto& attributeInfo = attributesInfo[a];
    if ( attributeInfo.name == "x" && ( attributeInfo.byteCount == 8 || attributeInfo.byteCount == 4 ) ) {
      indexX = a;
    } else if ( attributeInfo.name == "y" && ( attributeInfo.byteCount == 8 || attributeInfo.byteCount == 4 ) ) {
      indexY = a;
    } else if ( attributeInfo.name == "z" && ( attributeInfo.byteCount == 8 || attributeInfo.byteCount == 4 ) ) {
      indexZ = a;
    } else if ( attributeInfo.name == "red" && attributeInfo.byteCount == 1 ) {
      indexR = a;
    } else if ( attributeInfo.name == "green" && attributeInfo.byteCount == 1 ) {
      indexG = a;
    } else if ( attributeInfo.name == "blue" && attributeInfo.byteCount == 1 ) {
      indexB = a;
    } else if ( attributeInfo.name == "nx" && attributeInfo.byteCount == 4 && readNormals ) {
      indexNX = a;
    } else if ( attributeInfo.name == "ny" && attributeInfo.byteCount == 4 && readNormals ) {
      indexNY = a;
    } else if ( attributeInfo.name == "nz" && attributeInfo.byteCount == 4 && readNormals ) {
      indexNZ = a;
    } else if ( ( attributeInfo.name == "reflectance" || attributeInfo.name == "refc" ) &&
                attributeInfo.byteCount <= 2 ) {
      indexReflectance = a;
#if MULTIPLE_ATTRIBUTES
    } else if ( attributeInfo.name == "red0" && attributeInfo.byteCount == 1 ) {
      indexR0 = a;
    } else if ( attributeInfo.name == "green0" && attributeInfo.byteCount == 1 ) {
      indexG0 = a;
    } else if ( attributeInfo.name == "blue0" && attributeInfo.byteCount == 1 ) {
      indexB0 = a;
    } else if ( attributeInfo.name == "red1" && attributeInfo.byteCount == 1 ) {
      indexR1 = a;
    } else if ( attributeInfo.name == "green1" && attributeInfo.byteCount == 1 ) {
      indexG1 = a;
    } else if ( attributeInfo.name == "blue1" && attributeInfo.byteCount == 1 ) {
      indexB1 = a;
    } else if ( attributeInfo.name == "red2" && attributeInfo.byteCount == 1 ) {
      indexR2 = a;
    } else if ( attributeInfo.name == "green2" && attributeInfo.byteCount == 1 ) {
      indexG2 = a;
    } else if ( attributeInfo.name == "blue2" && attributeInfo.byteCount == 1 ) {
      indexB2 = a;
    } else if ( attributeInfo.name == "red3" && attributeInfo.byteCount == 1 ) {
      indexR3 = a;
    } else if ( attributeInfo.name == "green3" && attributeInfo.byteCount == 1 ) {
      indexG3 = a;
    } else if ( attributeInfo.name == "blue3" && attributeInfo.byteCount == 1 ) {
      indexB3 = a;
    } else if ( attributeInfo.name == "red4" && attributeInfo.byteCount == 1 ) {
      indexR4 = a;
    } else if ( attributeInfo.name == "green4" && attributeInfo.byteCount == 1 ) {
      indexG4 = a;
    } else if ( attributeInfo.name == "blue4" && attributeInfo.byteCount == 1 ) {
      indexB4 = a;
    } else if ( attributeInfo.name == "red5" && attributeInfo.byteCount == 1 ) {
      indexR5 = a;
    } else if ( attributeInfo.name == "green5" && attributeInfo.byteCount == 1 ) {
      indexG5 = a;
    } else if ( attributeInfo.name == "blue5" && attributeInfo.byteCount == 1 ) {
      indexB5 = a;
    } else if ( attributeInfo.name == "red6" && attributeInfo.byteCount == 1 ) {
      indexR6 = a;
    } else if ( attributeInfo.name == "green6" && attributeInfo.byteCount == 1 ) {
      indexG6 = a;
    } else if ( attributeInfo.name == "blue6" && attributeInfo.byteCount == 1 ) {
      indexB6 = a;
    } else if ( attributeInfo.name == "red7" && attributeInfo.byteCount == 1 ) {
      indexR7 = a;
    } else if ( attributeInfo.name == "green7" && attributeInfo.byteCount == 1 ) {
      indexG7 = a;
    } else if ( attributeInfo.name == "blue7" && attributeInfo.byteCount == 1 ) {
      indexB7 = a;
    } else if ( attributeInfo.name == "red8" && attributeInfo.byteCount == 1 ) {
      indexR8 = a;
    } else if ( attributeInfo.name == "green8" && attributeInfo.byteCount == 1 ) {
      indexG8 = a;
    } else if ( attributeInfo.name == "blue8" && attributeInfo.byteCount == 1 ) {
      indexB8 = a;
    } else if ( attributeInfo.name == "red9" && attributeInfo.byteCount == 1 ) {
      indexR9 = a;
    } else if ( attributeInfo.name == "green9" && attributeInfo.byteCount == 1 ) {
      indexG9 = a;
    } else if ( attributeInfo.name == "blue9" && attributeInfo.byteCount == 1 ) {
      indexB9 = a;
    } else if ( attributeInfo.name == "red10" && attributeInfo.byteCount == 1 ) {
      indexR10 = a;
    } else if ( attributeInfo.name == "green10" && attributeInfo.byteCount == 1 ) {
      indexG10 = a;
    } else if ( attributeInfo.name == "blue10" && attributeInfo.byteCount == 1 ) {
      indexB10 = a;
    } else if ( attributeInfo.name == "red11" && attributeInfo.byteCount == 1 ) {
      indexR11 = a;
    } else if ( attributeInfo.name == "green11" && attributeInfo.byteCount == 1 ) {
      indexG11 = a;
    } else if ( attributeInfo.name == "blue11" && attributeInfo.byteCount == 1 ) {
      indexB11 = a;
#if NUMBER_VIEWS == 13
    } else if ( attributeInfo.name == "red12" && attributeInfo.byteCount == 1 ) {
      indexR12 = a;
    } else if ( attributeInfo.name == "green12" && attributeInfo.byteCount == 1 ) {
      indexG12 = a;
    } else if ( attributeInfo.name == "blue12" && attributeInfo.byteCount == 1 ) {
      indexB12 = a;
#endif
    }
#endif
  }
  if ( indexX == PCC_UNDEFINED_INDEX || indexY == PCC_UNDEFINED_INDEX || indexZ == PCC_UNDEFINED_INDEX ) {
    std::cout << "Error: missing coordinates!" << std::endl;
    return false;
  }
  withColors_       = indexR != PCC_UNDEFINED_INDEX && indexG != PCC_UNDEFINED_INDEX && indexB != PCC_UNDEFINED_INDEX;
  withReflectances_ = indexReflectance != PCC_UNDEFINED_INDEX;
  withNormals_ = indexNX != PCC_UNDEFINED_INDEX && indexNY != PCC_UNDEFINED_INDEX && indexNZ != PCC_UNDEFINED_INDEX;
  resize( pointCount );
  if ( isAscii ) {
    size_t pointCounter = 0;
    while ( !ifs.eof() && pointCounter < pointCount ) {
      ifs.getline( tmp, MAX_BUFFER_SIZE );
      getTokens( tmp, sep, tokens );
      if ( tokens.empty() ) { continue; }
      if ( tokens.size() < attributeCount ) { return false; }
      auto& position = positions_[pointCounter];
      position[0]    = atof( tokens[indexX].c_str() );
      position[1]    = atof( tokens[indexY].c_str() );
      position[2]    = atof( tokens[indexZ].c_str() );
      if ( hasColors() ) {
#if MULTIPLE_ATTRIBUTES
        auto& color0  = colors_[0][pointCounter];
        color0[0]     = atoi( tokens[indexR0].c_str() );
        color0[1]     = atoi( tokens[indexG0].c_str() );
        color0[2]     = atoi( tokens[indexB0].c_str() );
        auto& color1  = colors_[1][pointCounter];
        color1[0]     = atoi( tokens[indexR1].c_str() );
        color1[1]     = atoi( tokens[indexG1].c_str() );
        color1[2]     = atoi( tokens[indexB1].c_str() );
        auto& color2  = colors_[2][pointCounter];
        color2[0]     = atoi( tokens[indexR2].c_str() );
        color2[1]     = atoi( tokens[indexG2].c_str() );
        color2[2]     = atoi( tokens[indexB2].c_str() );
        auto& color3  = colors_[3][pointCounter];
        color3[0]     = atoi( tokens[indexR3].c_str() );
        color3[1]     = atoi( tokens[indexG3].c_str() );
        color3[2]     = atoi( tokens[indexB3].c_str() );
        auto& color4  = colors_[4][pointCounter];
        color4[0]     = atoi( tokens[indexR4].c_str() );
        color4[1]     = atoi( tokens[indexG4].c_str() );
        color4[2]     = atoi( tokens[indexB4].c_str() );
        auto& color5  = colors_[5][pointCounter];
        color5[0]     = atoi( tokens[indexR5].c_str() );
        color5[1]     = atoi( tokens[indexG5].c_str() );
        color5[2]     = atoi( tokens[indexB5].c_str() );
        auto& color6  = colors_[6][pointCounter];
        color6[0]     = atoi( tokens[indexR6].c_str() );
        color6[1]     = atoi( tokens[indexG6].c_str() );
        color6[2]     = atoi( tokens[indexB6].c_str() );
        auto& color7  = colors_[7][pointCounter];
        color7[0]     = atoi( tokens[indexR7].c_str() );
        color7[1]     = atoi( tokens[indexG7].c_str() );
        color7[2]     = atoi( tokens[indexB7].c_str() );
        auto& color8  = colors_[8][pointCounter];
        color8[0]     = atoi( tokens[indexR8].c_str() );
        color8[1]     = atoi( tokens[indexG8].c_str() );
        color8[2]     = atoi( tokens[indexB8].c_str() );
        auto& color9  = colors_[9][pointCounter];
        color9[0]     = atoi( tokens[indexR9].c_str() );
        color9[1]     = atoi( tokens[indexG9].c_str() );
        color9[2]     = atoi( tokens[indexB9].c_str() );
        auto& color10 = colors_[10][pointCounter];
        color10[0]    = atoi( tokens[indexR10].c_str() );
        color10[1]    = atoi( tokens[indexG10].c_str() );
        color10[2]    = atoi( tokens[indexB10].c_str() );
        auto& color11 = colors_[11][pointCounter];
        color11[0]    = atoi( tokens[indexR11].c_str() );
        color11[1]    = atoi( tokens[indexG11].c_str() );
        color11[2]    = atoi( tokens[indexB11].c_str() );
#if NUMBER_VIEWS == 13
        auto& color12 = colors_[12][pointCounter];
        color12[0]    = atoi( tokens[indexR12].c_str() );
        color12[1]    = atoi( tokens[indexG12].c_str() );
        color12[2]    = atoi( tokens[indexB12].c_str() );
#endif
#else
          auto& color = colors_[pointCounter];
          color[0]    = atoi( tokens[indexR].c_str() );
          color[1]    = atoi( tokens[indexG].c_str() );
          color[2]    = atoi( tokens[indexB].c_str() );
#endif
      }
      if ( hasReflectances() ) { reflectances_[pointCounter] = uint16_t( atoi( tokens[indexReflectance].c_str() ) ); }
      ++pointCounter;
#if MULTIPLE_ATTRIBUTES
      if ( hasNormals() ) {
        auto& normal = normals_[pointCounter];
        normal[0]    = atof( tokens[indexNX].c_str() );
        normal[1]    = atof( tokens[indexNY].c_str() );
        normal[2]    = atof( tokens[indexNZ].c_str() );
      }
#endif
    }

#if ELEVEN_BITS
    // average
    double meanPosition[3] = { 0.0, 0.0, 0.0 };
    for ( size_t pointCounter = 0; pointCounter < pointCount; pointCounter++ ) {
      for (size_t i = 0; i < 3; i++) {
        meanPosition[i] += positions_[pointCounter][i];
      }
    }
    for (size_t i = 0; i < 3; i++) {
      meanPosition[i] /= pointCount;
    }

    // difference
    double** diffPosition;
    diffPosition = new double*[pointCount];
    for (size_t pointCounter = 0; pointCounter < pointCount; pointCounter++) {
      diffPosition[pointCounter] = new double[3];
    }

    size_t** diffIntPosition;
    diffIntPosition = new size_t*[pointCount];
    for ( size_t pointCounter = 0; pointCounter < pointCount; pointCounter++ ) {
      diffIntPosition[pointCounter] = new size_t[3];
    }

    for (size_t pointCounter = 0; pointCounter < pointCount; pointCounter++) {
      for (size_t i = 0; i < 3; i++) {
        diffPosition[pointCounter][i] = positions_[pointCounter][i] - meanPosition[i];
      }
    }

    // absolute max
    double maxAbsPosition = 0.0;
    for (size_t pointCounter = 0; pointCounter < pointCount; pointCounter++) {
      for (size_t i = 0; i < 3; i++) {
        if ( fabs(diffPosition[pointCounter][i]) > maxAbsPosition ) {
          maxAbsPosition = fabs(diffPosition[pointCounter][i]);
        }
      }
    }

    maxAbsPosition /= 0.8;

    for (size_t pointCounter = 0; pointCounter < pointCount; pointCounter++) {
      for (size_t i = 0; i < 3; i++) {
        diffIntPosition[pointCounter][i] = size_t((diffPosition[pointCounter][i] + maxAbsPosition) / maxAbsPosition * pow(2.0, 10) + 0.5);    // 11 bits
      }
    }

    // v2val
    std::vector<std::pair<size_t, size_t>> positionValuePairInitial;
    for (size_t pointCounter = 0; pointCounter < pointCount; pointCounter++) {
      size_t vx = diffIntPosition[pointCounter][0];
      size_t vy = diffIntPosition[pointCounter][1];
      size_t vz = diffIntPosition[pointCounter][2];

      size_t value  = ( ( 0x000001 & vx ) ) + ( ( 0x000001 & vy ) << 1 ) + ( ( 0x000001 & vz ) << 2 ) +
               ( ( 0x000002 & vx ) << 2 ) + ( ( 0x000002 & vy ) << 3 ) + ( ( 0x000002 & vz ) << 4 ) +
               ( ( 0x000004 & vx ) << 4 ) + ( ( 0x000004 & vy ) << 5 ) + ( ( 0x000004 & vz ) << 6 ) +
               ( ( 0x000008 & vx ) << 6 ) + ( ( 0x000008 & vy ) << 7 ) + ( ( 0x000008 & vz ) << 8 ) +
               ( ( 0x000010 & vx ) << 8 ) + ( ( 0x000010 & vy ) << 9 ) + ( ( 0x000010 & vz ) << 10 ) +
               ( ( 0x000020 & vx ) << 10 ) + ( ( 0x000020 & vy ) << 11 ) + ( ( 0x000020 & vz ) << 12 ) +
               ( ( 0x000040 & vx ) << 12 ) + ( ( 0x000040 & vy ) << 13 ) + ( ( 0x000040 & vz ) << 14 ) +
               ( ( 0x000080 & vx ) << 14 ) + ( ( 0x000080 & vy ) << 15 ) + ( ( 0x000080 & vz ) << 16 ) +
               ( ( 0x000100 & vx ) << 16 ) + ( ( 0x000100 & vy ) << 17 ) + ( ( 0x000100 & vz ) << 18 ) +
               ( ( 0x000200 & vx ) << 18 ) + ( ( 0x000200 & vy ) << 19 ) + ( ( 0x000200 & vz ) << 20 ) +
               ( ( 0x000400 & vx ) << 20 ) + ( ( 0x000400 & vy ) << 21 ) + ( ( 0x000400 & vz ) << 22 ) +
               ( ( 0x000800 & vx ) << 22 ) + ( ( 0x000800 & vy ) << 23 ) + ( ( 0x000800 & vz ) << 24 ) +
               ( ( 0x001000 & vx ) << 24 ) + ( ( 0x001000 & vy ) << 25 ) + ( ( 0x001000 & vz ) << 26 ) +
               ( ( 0x002000 & vx ) << 26 ) + ( ( 0x002000 & vy ) << 27 ) + ( ( 0x002000 & vz ) << 28 ) +
               ( ( 0x004000 & vx ) << 28 ) + ( ( 0x004000 & vy ) << 29 ) + ( ( 0x004000 & vz ) << 30 ) +
               ( ( 0x008000 & vx ) << 30 ) + ( ( 0x008000 & vy ) << 31 ) + ( ( 0x008000 & vz ) << 32 ) +
               ( ( 0x010000 & vx ) << 32 ) + ( ( 0x010000 & vy ) << 33 ) + ( ( 0x010000 & vz ) << 34 ) +
               ( ( 0x020000 & vx ) << 34 ) + ( ( 0x020000 & vy ) << 35 ) + ( ( 0x020000 & vz ) << 36 ) +
               ( ( 0x040000 & vx ) << 36 ) + ( ( 0x040000 & vy ) << 37 ) + ( ( 0x040000 & vz ) << 38 ) +
               ( ( 0x080000 & vx ) << 38 ) + ( ( 0x080000 & vy ) << 39 ) + ( ( 0x080000 & vz ) << 40 ) +
               ( ( 0x100000 & vx ) << 40 ) + ( ( 0x100000 & vy ) << 41 ) + ( ( 0x100000 & vz ) << 42 );

      std::pair<size_t, size_t> onePair(value, pointCounter);
      positionValuePairInitial.push_back(onePair);
    }

    std::sort(positionValuePairInitial.begin(), positionValuePairInitial.end(), comparator);

    std::vector<std::pair<size_t, size_t>> positionValuePair;
    positionValuePair.push_back(*positionValuePairInitial.begin());
    for (std::vector<std::pair<size_t, size_t>>::iterator iter = positionValuePairInitial.begin() + 1; iter < positionValuePairInitial.end(); iter++ )
    {
      if ( iter->first != (iter - 1)->first )
      {
        positionValuePair.push_back(*iter);
      }
    }

    size_t newPointCount = positionValuePair.size();

    // position
    positions_.resize(newPointCount);
    for ( size_t pointCounter = 0; pointCounter < newPointCount; pointCounter++ ) {
      for ( size_t i = 0; i < 3; i++ ) {
        positions_[pointCounter][i] = diffIntPosition[positionValuePair[pointCounter].second][i];
      }
    }

    // normals
    if ( hasNormals() ) {
      std::vector<PCCNormal3D> newNormals_;
      for ( size_t pointCounter = 0; pointCounter < newPointCount; pointCounter++ ) {
        newNormals_.push_back( normals_[positionValuePair[pointCounter].second]);
      }
      normals_.resize( newPointCount );
      for ( size_t pointCounter = 0; pointCounter < newPointCount; pointCounter++ ) {
        normals_[pointCounter] = newNormals_[pointCounter];
      }
    }

    // colors
    std::vector<PCCColor3B> newColors_[NUMBER_VIEWS];
    for ( size_t viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
      for (size_t pointCounter = 0; pointCounter < newPointCount; pointCounter++) {
        newColors_[viewIndex].push_back(colors_[viewIndex][positionValuePair[pointCounter].second]);
      }
    }
    for ( size_t viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
      colors_[viewIndex].resize( newPointCount );
      for (size_t pointCounter = 0; pointCounter < newPointCount; pointCounter++) {
        colors_[viewIndex][pointCounter] = newColors_[viewIndex][pointCounter];
      }
    }

    for ( size_t pointCounter = 0; pointCounter < pointCount; pointCounter++ ) { delete[] diffPosition[pointCounter]; }
    delete[] diffPosition;

    for ( size_t pointCounter = 0; pointCounter < pointCount; pointCounter++ ) { delete[] diffIntPosition[pointCounter]; }
    delete[] diffIntPosition;
#endif

  } else {
    ifs.close();
    ifs.open( fileName, std::ifstream::binary | std::ifstream::in );
    ifs.read( tmp, MAX_BUFFER_SIZE );
    char* str       = strstr( tmp, "end_header" );
    str             = strstr( str, "\n" );
    int headerCount = str - tmp + 1;
#if 0
    ifs.seekg(headerCount, std::ios::beg);  // JR: NOK on windows
#else
      ifs.close();
      ifs.open( fileName, std::ifstream::binary | std::ifstream::in );
      ifs.read( tmp, headerCount );
#endif
    for ( size_t pointCounter = 0; pointCounter < pointCount && !ifs.eof(); ++pointCounter ) {
      auto& position = positions_[pointCounter];
      for ( size_t a = 0; a < attributeCount && !ifs.eof(); ++a ) {
        const auto& attributeInfo = attributesInfo[a];
        if ( a == indexX ) {
          if ( attributeInfo.byteCount == 4 ) {
            float x;
            ifs.read( reinterpret_cast<char*>( &x ), sizeof( float ) );
            position[0] = x;
          } else {
            double x;
            ifs.read( reinterpret_cast<char*>( &x ), sizeof( double ) );
            position[0] = x;
          }
        } else if ( a == indexY ) {
          if ( attributeInfo.byteCount == 4 ) {
            float y;
            ifs.read( reinterpret_cast<char*>( &y ), sizeof( float ) );
            position[1] = y;
          } else {
            double y;
            ifs.read( reinterpret_cast<char*>( &y ), sizeof( double ) );
            position[1] = y;
          }
        } else if ( a == indexZ ) {
          if ( attributeInfo.byteCount == 4 ) {
            float z;
            ifs.read( reinterpret_cast<char*>( &z ), sizeof( float ) );
            position[2] = z;
          } else {
            double z;
            ifs.read( reinterpret_cast<char*>( &z ), sizeof( double ) );
            position[2] = z;
          }
        } else if ( a == indexR && attributeInfo.byteCount == 1 ) {
          auto& color = colors_[pointCounter];
          ifs.read( reinterpret_cast<char*>( &color[0] ), sizeof( uint8_t ) );
        } else if ( a == indexG && attributeInfo.byteCount == 1 ) {
          auto& color = colors_[pointCounter];
          ifs.read( reinterpret_cast<char*>( &color[1] ), sizeof( uint8_t ) );
        } else if ( a == indexB && attributeInfo.byteCount == 1 ) {
          auto& color = colors_[pointCounter];
          ifs.read( reinterpret_cast<char*>( &color[2] ), sizeof( uint8_t ) );
        } else if ( a == indexNX ) {
          if ( attributeInfo.byteCount == 4 ) {
            float nx;
            ifs.read( reinterpret_cast<char*>( &nx ), sizeof( float ) );
            normals_[pointCounter][0] = nx;
          } else {
            double nx;
            ifs.read( reinterpret_cast<char*>( &nx ), sizeof( double ) );
            normals_[pointCounter][0] = nx;
          }
        } else if ( a == indexNY ) {
          if ( attributeInfo.byteCount == 4 ) {
            float ny;
            ifs.read( reinterpret_cast<char*>( &ny ), sizeof( float ) );
            normals_[pointCounter][1] = ny;
          } else {
            double ny;
            ifs.read( reinterpret_cast<char*>( &ny ), sizeof( double ) );
            normals_[pointCounter][1] = ny;
          }
        } else if ( a == indexNZ ) {
          if ( attributeInfo.byteCount == 4 ) {
            float nz;
            ifs.read( reinterpret_cast<char*>( &nz ), sizeof( float ) );
            normals_[pointCounter][2] = nz;
          } else {
            double nz;
            ifs.read( reinterpret_cast<char*>( &nz ), sizeof( double ) );
            normals_[pointCounter][2] = nz;
          }
        } else if ( a == indexReflectance && attributeInfo.byteCount <= 2 ) {
          if ( indexReflectance == 1 ) {
            uint8_t reflectance;
            ifs.read( reinterpret_cast<char*>( &reflectance ), sizeof( uint8_t ) );
            reflectances_[pointCounter] = reflectance;
          } else {
            auto& reflectance = reflectances_[pointCounter];
            ifs.read( reinterpret_cast<char*>( reflectance ), sizeof( uint16_t ) );
          }
        } else {
          char buffer[128];
          ifs.read( buffer, attributeInfo.byteCount );
        }
      }
    }
  }
  return true;
}

void PCCPointSet3::convertRGBToYUV() {  // BT709
#if MULTIPLE_ATTRIBUTES
  for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
    for ( auto& color : colors_[viewIndex] ) {
      const uint8_t r = color[0];
      const uint8_t g = color[1];
      const uint8_t b = color[2];
      const double  y = std::round( 0.212600 * r + 0.715200 * g + 0.072200 * b );
      const double  u = std::round( -0.114572 * r - 0.385428 * g + 0.500000 * b + 128.0 );
      const double  v = std::round( 0.500000 * r - 0.454153 * g - 0.045847 * b + 128.0 );
      assert( y >= 0.0 && y <= 255.0 && u >= 0.0 && u <= 255.0 && v >= 0.0 && v <= 255.0 );
      color[0] = static_cast<uint8_t>( y );
      color[1] = static_cast<uint8_t>( u );
      color[2] = static_cast<uint8_t>( v );
    }
  }
#else
    for ( auto& color : colors_ ) {
      const uint8_t r = color[0];
      const uint8_t g = color[1];
      const uint8_t b = color[2];
      const double  y = std::round( 0.212600 * r + 0.715200 * g + 0.072200 * b );
      const double  u = std::round( -0.114572 * r - 0.385428 * g + 0.500000 * b + 128.0 );
      const double  v = std::round( 0.500000 * r - 0.454153 * g - 0.045847 * b + 128.0 );
      assert( y >= 0.0 && y <= 255.0 && u >= 0.0 && u <= 255.0 && v >= 0.0 && v <= 255.0 );
      color[0] = static_cast<uint8_t>( y );
      color[1] = static_cast<uint8_t>( u );
      color[2] = static_cast<uint8_t>( v );
    }
#endif
}

void PCCPointSet3::convertRGBToYUVClosedLoop() {  // BT709
#if MULTIPLE_ATTRIBUTES
  for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
    for ( auto& color : colors_[viewIndex] ) {
      const uint8_t r = color[0];
      const uint8_t g = color[1];
      const uint8_t b = color[2];
      const double  y = std::round( 0.212600 * r + 0.715200 * g + 0.072200 * b );
      const double  u = std::round( ( b - y ) / 1.8556 + 128.0 );
      const double  v = std::round( ( r - y ) / 1.5748 + 128.0 );
      assert( y >= 0.0 && y <= 255.0 && u >= 0.0 && u <= 255.0 && v >= 0.0 && v <= 255.0 );
      color[0] = static_cast<uint8_t>( y );
      color[1] = static_cast<uint8_t>( u );
      color[2] = static_cast<uint8_t>( v );
    }
  }
#else
    for ( auto& color : colors_ ) {
      const uint8_t r = color[0];
      const uint8_t g = color[1];
      const uint8_t b = color[2];
      const double  y = std::round( 0.212600 * r + 0.715200 * g + 0.072200 * b );
      const double  u = std::round( ( b - y ) / 1.8556 + 128.0 );
      const double  v = std::round( ( r - y ) / 1.5748 + 128.0 );
      assert( y >= 0.0 && y <= 255.0 && u >= 0.0 && u <= 255.0 && v >= 0.0 && v <= 255.0 );
      color[0] = static_cast<uint8_t>( y );
      color[1] = static_cast<uint8_t>( u );
      color[2] = static_cast<uint8_t>( v );
    }
#endif
}
void PCCPointSet3::convertYUVToRGB() {  // BT709
#if MULTIPLE_ATTRIBUTES
  for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
    for ( auto& color : colors_[viewIndex] ) {
      const double y1 = color[0];
      const double u1 = color[1] - 128.0;
      const double v1 = color[2] - 128.0;
      const double r  = PCCClip( round( y1 /*- 0.00000 * u1*/ + 1.57480 * v1 ), 0.0, 255.0 );
      const double g  = PCCClip( round( y1 - 0.18733 * u1 - 0.46813 * v1 ), 0.0, 255.0 );
      const double b  = PCCClip( round( y1 + 1.85563 * u1 /*+ 0.00000 * v1*/ ), 0.0, 255.0 );
      color[0]        = static_cast<uint8_t>( r );
      color[1]        = static_cast<uint8_t>( g );
      color[2]        = static_cast<uint8_t>( b );
    }
  }
#else
    for ( auto& color : colors_ ) {
      const double y1 = color[0];
      const double u1 = color[1] - 128.0;
      const double v1 = color[2] - 128.0;
      const double r  = PCCClip( round( y1 /*- 0.00000 * u1*/ + 1.57480 * v1 ), 0.0, 255.0 );
      const double g  = PCCClip( round( y1 - 0.18733 * u1 - 0.46813 * v1 ), 0.0, 255.0 );
      const double b  = PCCClip( round( y1 + 1.85563 * u1 /*+ 0.00000 * v1*/ ), 0.0, 255.0 );
      color[0]        = static_cast<uint8_t>( r );
      color[1]        = static_cast<uint8_t>( g );
      color[2]        = static_cast<uint8_t>( b );
    }
#endif
}

bool PCCPointSet3::transfertColors( PCCPointSet3& target,
                                    const int32_t searchRange,
                                    const bool    losslessTexture,
                                    const int     numNeighborsColorTransferFwd,
                                    const int     numNeighborsColorTransferBwd,
                                    const bool    useDistWeightedAverageFwd,
                                    const bool    useDistWeightedAverageBwd,
                                    const bool    skipAvgIfIdenticalSourcePointPresentFwd,
                                    const bool    skipAvgIfIdenticalSourcePointPresentBwd,
                                    const double  distOffsetFwd,
                                    const double  distOffsetBwd,
                                    double        maxGeometryDist2Fwd,
                                    double        maxGeometryDist2Bwd,
                                    double        maxColorDist2Fwd,
                                    double        maxColorDist2Bwd ) const {
  const auto&  source           = *this;
  const size_t pointCountSource = source.getPointCount();
  const size_t pointCountTarget = target.getPointCount();
  if ( !pointCountSource || !pointCountTarget || !source.hasColors() ) { return false; }
  PCCKdTree kdtreeTarget( target ), kdtreeSource( source );
  target.addColors();
#if MULTIPLE_ATTRIBUTES
  std::vector<PCCColor3B> refinedColors1[NUMBER_VIEWS];
  for ( size_t i = 0; i < NUMBER_VIEWS; i++ ) { refinedColors1[i].resize( pointCountTarget ); }
#else
    std::vector<PCCColor3B> refinedColors1;
    refinedColors1.resize( pointCountTarget );
#endif
  maxGeometryDist2Fwd = ( maxGeometryDist2Fwd < 512 ) ? maxGeometryDist2Fwd : std::numeric_limits<double>::max();
  maxGeometryDist2Bwd = ( maxGeometryDist2Bwd < 512 ) ? maxGeometryDist2Bwd : std::numeric_limits<double>::max();
  maxColorDist2Fwd    = ( maxColorDist2Fwd < 512 ) ? maxColorDist2Fwd : std::numeric_limits<double>::max();
  maxColorDist2Bwd    = ( maxColorDist2Bwd < 512 ) ? maxColorDist2Bwd : std::numeric_limits<double>::max();

  // ==========================================================================================
  //                                     Forward direction
  // ==========================================================================================
  // for each target point indexed by index, derive the refined color as refinedColors1[index]
  PCCNNResult result;
  for ( size_t index = 0; index < pointCountTarget; ++index ) {
    kdtreeSource.search( target[index], numNeighborsColorTransferFwd, result );
    // keep the points that satisfy geometry dist threshold
    while ( 1 ) {
      if ( result.count() == 1 ) { break; }
      if ( result.dist( int( result.count() ) - 1 ) <= maxGeometryDist2Fwd ) {
        break;
      } else {
        result.pop_dist();
        result.pop_indices();
        result.dec_count();
      }
    }
    bool isDone = false;
    if ( skipAvgIfIdenticalSourcePointPresentFwd ) {
      if ( result.dist( 0 ) < 0.0001 ) {
#if MULTIPLE_ATTRIBUTES
        for ( size_t viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
          refinedColors1[viewIndex][index] = source.getColor( result.indices( 0 ), viewIndex );
        }
#else
          refinedColors1[index] = source.getColor( result.indices( 0 ) );
#endif
        isDone = true;
      }
    }
    if ( !isDone ) {
      int nNN = (int)result.count();
      while ( nNN > 0 && !isDone ) {
        if ( nNN == 1 ) {
#if MULTIPLE_ATTRIBUTES
          for ( size_t viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
            refinedColors1[viewIndex][index] = source.getColor( result.indices( 0 ), viewIndex );
          }
#else
            refinedColors1[index] = source.getColor( result.indices( 0 ) );
#endif
          isDone = true;
        }
        if ( !isDone ) {
#if MULTIPLE_ATTRIBUTES
          std::vector<PCCVector3D> colors[NUMBER_VIEWS];
          for ( int i = 0; i < NUMBER_VIEWS; i++ ) {
            colors[i].resize( 0 );
            colors[i].resize( nNN );
          }
          for ( int i = 0; i < nNN; ++i ) {
            for ( int k = 0; k < 3; ++k ) {
              for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
                colors[viewIndex][i][k] = double( source.getColor( result.indices( i ), viewIndex )[k] );
              }
            }
#else
            std::vector<PCCVector3D> colors;
            colors.resize( 0 );
            colors.resize( nNN );
            for ( int i = 0; i < nNN; ++i ) {
              for ( int k = 0; k < 3; ++k ) { colors[i][k] = double( source.getColor( result.indices( i ) )[k] ); }
#endif
          }
          double maxColorDist2 = std::numeric_limits<double>::min();
          for ( int i = 0; i < nNN; ++i ) {
            for ( int j = 0; j < nNN; ++j ) {
#if MULTIPLE_ATTRIBUTES
              for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
                const double dist2 = ( colors[viewIndex][i] - colors[viewIndex][j] ).getNorm2();
                if ( dist2 > maxColorDist2 ) { maxColorDist2 = dist2; }
              }
#else
                const double dist2 = ( colors[i] - colors[j] ).getNorm2();
                if ( dist2 > maxColorDist2 ) { maxColorDist2 = dist2; }
#endif
            }
          }
          if ( maxColorDist2 <= maxColorDist2Fwd ) {
#if MULTIPLE_ATTRIBUTES
            for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
              PCCVector3D refinedColor( 0.0 );
              if ( useDistWeightedAverageFwd ) {
                double sumWeights{0.0};
                for ( int i = 0; i < nNN; ++i ) {
                  const double weight = 1 / ( result.dist( i ) + distOffsetFwd );
                  for ( int k = 0; k < 3; ++k ) {
                    refinedColor[k] += source.getColor( result.indices( i ), viewIndex )[k] * weight;
                  }
                  sumWeights += weight;
                }
                refinedColor /= sumWeights;
              } else {
                for ( int i = 0; i < nNN; ++i ) {
                  for ( int k = 0; k < 3; ++k ) {
                    refinedColor[k] += source.getColor( result.indices( i ), viewIndex )[k];
                  }
                }
                refinedColor /= nNN;
              }
              for ( int k = 0; k < 3; ++k ) {
                refinedColors1[viewIndex][index][k] = uint8_t( PCCClip( round( refinedColor[k] ), 0.0, 255.0 ) );
              }
            }
#else
              PCCVector3D refinedColor( 0.0 );
              if ( useDistWeightedAverageFwd ) {
                double sumWeights{0.0};
                for ( int i = 0; i < nNN; ++i ) {
                  const double weight = 1 / ( result.dist( i ) + distOffsetFwd );
                  for ( int k = 0; k < 3; ++k ) {
                    refinedColor[k] += source.getColor( result.indices( i ) )[k] * weight;
                  }
                  sumWeights += weight;
                }
                refinedColor /= sumWeights;
              } else {
                for ( int i = 0; i < nNN; ++i ) {
                  for ( int k = 0; k < 3; ++k ) { refinedColor[k] += source.getColor( result.indices( i ) )[k]; }
                }
                refinedColor /= nNN;
              }
              for ( int k = 0; k < 3; ++k ) {
                refinedColors1[index][k] = uint8_t( PCCClip( round( refinedColor[k] ), 0.0, 255.0 ) );
              }
#endif
            isDone = true;
          } else {
            --nNN;
          }
        }
      }
    }
  }
  // ==========================================================================================
  //                                  Backward direction
  // ==========================================================================================
  // for each target point, derive a vector of source candidate points as colorsDists2.
  // colorsDists2 is iteratively refined (by removing the farthest points) until the
  // std of remaining colors in it is smaller than a threshold.
  struct DistColor {
    double     dist;
    PCCColor3B color;
  };

#if MULTIPLE_ATTRIBUTES
  std::vector<std::vector<DistColor>> refinedColorsDists2[NUMBER_VIEWS];
  for ( size_t viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
    refinedColorsDists2[viewIndex].resize( pointCountTarget );
    // populate refinedColorsDists2
    for ( size_t index = 0; index < pointCountSource; ++index ) {
      const PCCColor3B color = source.getColor( index, viewIndex );
      kdtreeTarget.search( source[index], numNeighborsColorTransferBwd, result );
      // keep the points that satisfy geometry dist threshold
      for ( int i = 0; i < result.count(); ++i ) {
        if ( result.dist( i ) <= maxGeometryDist2Bwd ) {
          refinedColorsDists2[viewIndex][result.indices( i )].push_back( DistColor{result.dist( i ), color} );
        }
      }
    }
  }
#else
    std::vector<std::vector<DistColor>> refinedColorsDists2;
    refinedColorsDists2.resize( pointCountTarget );
    // populate refinedColorsDists2
    for ( size_t index = 0; index < pointCountSource; ++index ) {
      const PCCColor3B color = source.getColor( index );
      kdtreeTarget.search( source[index], numNeighborsColorTransferBwd, result );
      // keep the points that satisfy geometry dist threshold
      for ( int i = 0; i < result.count(); ++i ) {
        if ( result.dist( i ) <= maxGeometryDist2Bwd ) {
          refinedColorsDists2[result.indices( i )].push_back( DistColor{result.dist( i ), color} );
        }
      }
    }
#endif
    // sort refinedColorsDists2 according to distance
#if MULTIPLE_ATTRIBUTES
  for ( size_t viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
    for ( size_t index = 0; index < pointCountTarget; ++index ) {
      std::sort( refinedColorsDists2[viewIndex][index].begin(), refinedColorsDists2[viewIndex][index].end(),
                 []( DistColor& dc1, DistColor& dc2 ) { return dc1.dist < dc2.dist; } );
    }
  }
#else
    for ( size_t index = 0; index < pointCountTarget; ++index ) {
      std::sort( refinedColorsDists2[index].begin(), refinedColorsDists2[index].end(),
                 []( DistColor& dc1, DistColor& dc2 ) { return dc1.dist < dc2.dist; } );
    }
#endif
    // compute centroid2
#if MULTIPLE_ATTRIBUTES
  for ( size_t viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
    for ( size_t index = 0; index < pointCountTarget; ++index ) {
      const PCCColor3B color1 = refinedColors1[viewIndex][index];  // refined color derived in forward direction
      auto&            colorsDists2 =
          refinedColorsDists2[viewIndex][index];  // set of candidate points derived in backward direction
      if ( colorsDists2.empty() || losslessTexture ) {
        target.setColor( index, viewIndex, color1 );
      } else {
        bool              isDone = false;
        const PCCVector3D centroid1( color1[0], color1[1], color1[2] );
        PCCVector3D       centroid2( 0.0 );
        if ( skipAvgIfIdenticalSourcePointPresentBwd ) {
          if ( colorsDists2[0].dist < 0.0001 ) {
            auto temp = colorsDists2[0];
            colorsDists2.clear();
            colorsDists2.push_back( temp );
            for ( int k = 0; k < 3; ++k ) { centroid2[k] = colorsDists2[0].color[k]; }
            isDone = true;
          }
        }
        if ( !isDone ) {
          int nNN = (int)colorsDists2.size();
          while ( nNN > 0 && !isDone ) {
            nNN = (int)colorsDists2.size();
            if ( nNN == 1 ) {
              auto temp = colorsDists2[0];
              colorsDists2.clear();
              colorsDists2.push_back( temp );
              for ( int k = 0; k < 3; ++k ) { centroid2[k] = colorsDists2[0].color[k]; }
              isDone = true;
            }
            if ( !isDone ) {
              std::vector<PCCVector3D> colors;
              colors.resize( 0 );
              colors.resize( nNN );
              for ( int i = 0; i < nNN; ++i ) {
                for ( int k = 0; k < 3; ++k ) { colors[i][k] = double( colorsDists2[i].color[k] ); }
              }
              double maxColorDist2 = std::numeric_limits<double>::min();
              for ( int i = 0; i < nNN; ++i ) {
                for ( int j = 0; j < nNN; ++j ) {
                  const double dist2 = ( colors[i] - colors[j] ).getNorm2();
                  if ( dist2 > maxColorDist2 ) { maxColorDist2 = dist2; }
                }
              }
              if ( maxColorDist2 <= maxColorDist2Bwd ) {
                for ( size_t k = 0; k < 3; ++k ) { centroid2[k] = 0; }
                if ( useDistWeightedAverageBwd ) {
                  double sumWeights{0.0};
                  for ( int i = 0; i < colorsDists2.size(); ++i ) {
                    const double weight = 1 / ( sqrt( colorsDists2[i].dist ) + distOffsetBwd );
                    for ( size_t k = 0; k < 3; ++k ) { centroid2[k] += ( colorsDists2[i].color[k] * weight ); }
                    sumWeights += weight;
                  }
                  centroid2 /= sumWeights;
                } else {
                  for ( auto& coldist : colorsDists2 ) {
                    for ( int k = 0; k < 3; ++k ) { centroid2[k] += coldist.color[k]; }
                  }
                  centroid2 /= colorsDists2.size();
                }
                isDone = true;
              } else {
                colorsDists2.pop_back();
              }
            }
          }
        }
        double H  = double( colorsDists2.size() );
        double D2 = 0.0;
        for ( const auto color2dist : colorsDists2 ) {
          auto color2 = color2dist.color;
          for ( size_t k = 0; k < 3; ++k ) {
            const double d2 = centroid2[k] - color2[k];
            D2 += d2 * d2;
          }
        }
        const double r      = double( pointCountTarget ) / double( pointCountSource );
        const double delta2 = ( centroid2 - centroid1 ).getNorm2();
        const double eps    = 0.000001;

        const bool fixWeight = 1;           // m42538
        if ( fixWeight || delta2 > eps ) {  // centroid2 != centroid1
          double w = 0.0;

          if ( !fixWeight ) {
            const double alpha = D2 / delta2;
            const double a     = H * r - 1.0;
            const double c     = alpha * r - 1.0;
            if ( fabs( a ) < eps ) {
              w = -0.5 * c;
            } else {
              const double delta = 1.0 - a * c;
              if ( delta >= 0.0 ) { w = ( -1.0 + sqrt( delta ) ) / a; }
            }
          }
          const double oneMinusW = 1.0 - w;
          PCCVector3D  color0;
          for ( size_t k = 0; k < 3; ++k ) {
            color0[k] = PCCClip( round( w * centroid1[k] + oneMinusW * centroid2[k] ), 0.0, 255.0 );
          }
          const double rSource  = 1.0 / double( pointCountSource );
          const double rTarget  = 1.0 / double( pointCountTarget );
          const double maxValue = std::numeric_limits<uint8_t>::max();
          double       minError = std::numeric_limits<double>::max();
          PCCVector3D  bestColor( color0 );
          PCCVector3D  color;
          for ( int32_t s1 = -searchRange; s1 <= searchRange; ++s1 ) {
            color[0] = PCCClip( color0[0] + s1, 0.0, maxValue );
            for ( int32_t s2 = -searchRange; s2 <= searchRange; ++s2 ) {
              color[1] = PCCClip( color0[1] + s2, 0.0, maxValue );
              for ( int32_t s3 = -searchRange; s3 <= searchRange; ++s3 ) {
                color[2] = PCCClip( color0[2] + s3, 0.0, maxValue );

                double e1 = 0.0;
                for ( size_t k = 0; k < 3; ++k ) {
                  const double d = color[k] - color1[k];
                  e1 += d * d;
                }
                e1 *= rTarget;

                double e2 = 0.0;
                for ( const auto color2dist : colorsDists2 ) {
                  auto color2 = color2dist.color;
                  for ( size_t k = 0; k < 3; ++k ) {
                    const double d = color[k] - color2[k];
                    e2 += d * d;
                  }
                }
                e2 *= rSource;

                const double error = std::max( e1, e2 );
                if ( error < minError ) {
                  minError  = error;
                  bestColor = color;
                }
              }
            }
          }
          target.setColor( index, viewIndex,
                           PCCColor3B( uint8_t( bestColor[0] ), uint8_t( bestColor[1] ), uint8_t( bestColor[2] ) ) );
        } else {  // centroid2 == centroid1
          target.setColor( index, viewIndex, color1 );
        }
      }
    }
  }
#else
    for ( size_t index = 0; index < pointCountTarget; ++index ) {
      const PCCColor3B color1 = refinedColors1[index];       // refined color derived in forward direction
      auto& colorsDists2      = refinedColorsDists2[index];  // set of candidate points derived in backward direction
      if ( colorsDists2.empty() || losslessTexture ) {
        target.setColor( index, color1 );
      } else {
        bool              isDone = false;
        const PCCVector3D centroid1( color1[0], color1[1], color1[2] );
        PCCVector3D       centroid2( 0.0 );
        if ( skipAvgIfIdenticalSourcePointPresentBwd ) {
          if ( colorsDists2[0].dist < 0.0001 ) {
            auto temp = colorsDists2[0];
            colorsDists2.clear();
            colorsDists2.push_back( temp );
            for ( int k = 0; k < 3; ++k ) { centroid2[k] = colorsDists2[0].color[k]; }
            isDone = true;
          }
        }
        if ( !isDone ) {
          int nNN = (int)colorsDists2.size();
          while ( nNN > 0 && !isDone ) {
            nNN = (int)colorsDists2.size();
            if ( nNN == 1 ) {
              auto temp = colorsDists2[0];
              colorsDists2.clear();
              colorsDists2.push_back( temp );
              for ( int k = 0; k < 3; ++k ) { centroid2[k] = colorsDists2[0].color[k]; }
              isDone = true;
            }
            if ( !isDone ) {
              std::vector<PCCVector3D> colors;
              colors.resize( 0 );
              colors.resize( nNN );
              for ( int i = 0; i < nNN; ++i ) {
                for ( int k = 0; k < 3; ++k ) { colors[i][k] = double( colorsDists2[i].color[k] ); }
              }
              double maxColorDist2 = std::numeric_limits<double>::min();
              for ( int i = 0; i < nNN; ++i ) {
                for ( int j = 0; j < nNN; ++j ) {
                  const double dist2 = ( colors[i] - colors[j] ).getNorm2();
                  if ( dist2 > maxColorDist2 ) { maxColorDist2 = dist2; }
                }
              }
              if ( maxColorDist2 <= maxColorDist2Bwd ) {
                for ( size_t k = 0; k < 3; ++k ) { centroid2[k] = 0; }
                if ( useDistWeightedAverageBwd ) {
                  double sumWeights{0.0};
                  for ( int i = 0; i < colorsDists2.size(); ++i ) {
                    const double weight = 1 / ( sqrt( colorsDists2[i].dist ) + distOffsetBwd );
                    for ( size_t k = 0; k < 3; ++k ) { centroid2[k] += ( colorsDists2[i].color[k] * weight ); }
                    sumWeights += weight;
                  }
                  centroid2 /= sumWeights;
                } else {
                  for ( auto& coldist : colorsDists2 ) {
                    for ( int k = 0; k < 3; ++k ) { centroid2[k] += coldist.color[k]; }
                  }
                  centroid2 /= colorsDists2.size();
                }
                isDone = true;
              } else {
                colorsDists2.pop_back();
              }
            }
          }
        }
        double H  = double( colorsDists2.size() );
        double D2 = 0.0;
        for ( const auto color2dist : colorsDists2 ) {
          auto color2 = color2dist.color;
          for ( size_t k = 0; k < 3; ++k ) {
            const double d2 = centroid2[k] - color2[k];
            D2 += d2 * d2;
          }
        }
        const double r      = double( pointCountTarget ) / double( pointCountSource );
        const double delta2 = ( centroid2 - centroid1 ).getNorm2();
        const double eps    = 0.000001;

        const bool fixWeight = 1;           // m42538
        if ( fixWeight || delta2 > eps ) {  // centroid2 != centroid1
          double w = 0.0;

          if ( !fixWeight ) {
            const double alpha = D2 / delta2;
            const double a     = H * r - 1.0;
            const double c     = alpha * r - 1.0;
            if ( fabs( a ) < eps ) {
              w = -0.5 * c;
            } else {
              const double delta = 1.0 - a * c;
              if ( delta >= 0.0 ) { w = ( -1.0 + sqrt( delta ) ) / a; }
            }
          }
          const double oneMinusW = 1.0 - w;
          PCCVector3D  color0;
          for ( size_t k = 0; k < 3; ++k ) {
            color0[k] = PCCClip( round( w * centroid1[k] + oneMinusW * centroid2[k] ), 0.0, 255.0 );
          }
          const double rSource  = 1.0 / double( pointCountSource );
          const double rTarget  = 1.0 / double( pointCountTarget );
          const double maxValue = std::numeric_limits<uint8_t>::max();
          double       minError = std::numeric_limits<double>::max();
          PCCVector3D  bestColor( color0 );
          PCCVector3D  color;
          for ( int32_t s1 = -searchRange; s1 <= searchRange; ++s1 ) {
            color[0] = PCCClip( color0[0] + s1, 0.0, maxValue );
            for ( int32_t s2 = -searchRange; s2 <= searchRange; ++s2 ) {
              color[1] = PCCClip( color0[1] + s2, 0.0, maxValue );
              for ( int32_t s3 = -searchRange; s3 <= searchRange; ++s3 ) {
                color[2] = PCCClip( color0[2] + s3, 0.0, maxValue );

                double e1 = 0.0;
                for ( size_t k = 0; k < 3; ++k ) {
                  const double d = color[k] - color1[k];
                  e1 += d * d;
                }
                e1 *= rTarget;

                double e2 = 0.0;
                for ( const auto color2dist : colorsDists2 ) {
                  auto color2 = color2dist.color;
                  for ( size_t k = 0; k < 3; ++k ) {
                    const double d = color[k] - color2[k];
                    e2 += d * d;
                  }
                }
                e2 *= rSource;

                const double error = std::max( e1, e2 );
                if ( error < minError ) {
                  minError  = error;
                  bestColor = color;
                }
              }
            }
          }
          target.setColor( index,
                           PCCColor3B( uint8_t( bestColor[0] ), uint8_t( bestColor[1] ), uint8_t( bestColor[2] ) ) );
        } else {  // centroid2 == centroid1
          target.setColor( index, color1 );
        }
      }
    }
#endif
  return true;
}

bool PCCPointSet3::transfertColorSimple( PCCPointSet3& target, const double bestColorSearchStep ) {
  const auto&  source           = *this;
  const size_t pointCountSource = source.getPointCount();
  const size_t pointCountTarget = target.getPointCount();
  if ( !pointCountSource || !pointCountTarget || !source.hasColors() ) { return false; }
  target.addColors();

  PCCKdTree kdtreeSource( source ), kdtreeTarget( target );

#if MULTIPLE_ATTRIBUTES
  for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
    std::vector<PCCColor3B>              refinedColors1;
    std::vector<std::vector<PCCColor3B>> refinedColors2;
    refinedColors1.resize( pointCountTarget );
    refinedColors2.resize( pointCountTarget );
    const size_t num_results = 1;
    PCCNNResult  result;
    for ( size_t index = 0; index < pointCountTarget; ++index ) {
      kdtreeSource.search( target[index], num_results, result );
      refinedColors1[index] = source.getColor( result.indices( 0 ), viewIndex );
    }
    for ( size_t index = 0; index < pointCountSource; ++index ) {
      const PCCColor3B color = source.getColor( index, viewIndex );
      kdtreeTarget.search( source[index], num_results, result );
      refinedColors2[result.indices( 0 )].push_back( color );
    }
    for ( size_t index = 0; index < pointCountTarget; ++index ) {
      const PCCColor3B              color1  = refinedColors1[index];
      const std::vector<PCCColor3B> colors2 = refinedColors2[index];
      if ( colors2.empty() ) {
        target.setColor( index, viewIndex, color1 );
      } else {
        double      s        = 1.0 / colors2.size();
        double      r1       = 1.0 / pointCountTarget;
        double      r2       = 1.0 / ( pointCountSource * colors2.size() );
        double      w1       = 0.0;
        double      minError = std::numeric_limits<double>::max();
        PCCVector3D bestColor;
        while ( w1 <= 1.0 ) {
          const double w2 = 1.0 - w1;
          PCCVector3D  color( 0.0 );
          for ( const auto color2 : colors2 ) {
            for ( size_t k = 0; k < 3; ++k ) { color[k] += color2[k]; }
          }
          for ( size_t k = 0; k < 3; ++k ) {
            color[k] = ( std::min )( round( w2 * s * color[k] + w1 * color1[k] ), 255.0 );
          }

          double e1 = 0.0;
          for ( size_t k = 0; k < 3; ++k ) {
            const double d = color[k] - color1[k];
            e1 += d * d;
          }
          e1 *= r1;

          double e2 = 0.0;
          for ( const auto color2 : colors2 ) {
            for ( size_t k = 0; k < 3; ++k ) {
              const double d = color[k] - color2[k];
              e2 += d * d;
            }
          }
          e2 *= r2;

          const double e = ( std::max )( e1, e2 );
          if ( e < minError ) {
            bestColor = color;
            minError  = e;
          }
          w1 += bestColorSearchStep;
        }
        target.setColor( index, viewIndex,
                         PCCColor3B( uint8_t( bestColor[0] ), uint8_t( bestColor[1] ), uint8_t( bestColor[2] ) ) );
      }
    }
  }
#else
    std::vector<PCCColor3B>              refinedColors1;
    std::vector<std::vector<PCCColor3B>> refinedColors2;
    refinedColors1.resize( pointCountTarget );
    refinedColors2.resize( pointCountTarget );
    const size_t num_results = 1;
    PCCNNResult  result;
    for ( size_t index = 0; index < pointCountTarget; ++index ) {
      kdtreeSource.search( target[index], num_results, result );
      refinedColors1[index] = source.getColor( result.indices( 0 ) );
    }
    for ( size_t index = 0; index < pointCountSource; ++index ) {
      const PCCColor3B color = source.getColor( index );
      kdtreeTarget.search( source[index], num_results, result );
      refinedColors2[result.indices( 0 )].push_back( color );
    }
    for ( size_t index = 0; index < pointCountTarget; ++index ) {
      const PCCColor3B              color1  = refinedColors1[index];
      const std::vector<PCCColor3B> colors2 = refinedColors2[index];
      if ( colors2.empty() ) {
        target.setColor( index, color1 );
      } else {
        double      s        = 1.0 / colors2.size();
        double      r1       = 1.0 / pointCountTarget;
        double      r2       = 1.0 / ( pointCountSource * colors2.size() );
        double      w1       = 0.0;
        double      minError = std::numeric_limits<double>::max();
        PCCVector3D bestColor;
        while ( w1 <= 1.0 ) {
          const double w2 = 1.0 - w1;
          PCCVector3D  color( 0.0 );
          for ( const auto color2 : colors2 ) {
            for ( size_t k = 0; k < 3; ++k ) { color[k] += color2[k]; }
          }
          for ( size_t k = 0; k < 3; ++k ) {
            color[k] = ( std::min )( round( w2 * s * color[k] + w1 * color1[k] ), 255.0 );
          }

          double e1 = 0.0;
          for ( size_t k = 0; k < 3; ++k ) {
            const double d = color[k] - color1[k];
            e1 += d * d;
          }
          e1 *= r1;

          double e2 = 0.0;
          for ( const auto color2 : colors2 ) {
            for ( size_t k = 0; k < 3; ++k ) {
              const double d = color[k] - color2[k];
              e2 += d * d;
            }
          }
          e2 *= r2;

          const double e = ( std::max )( e1, e2 );
          if ( e < minError ) {
            bestColor = color;
            minError  = e;
          }
          w1 += bestColorSearchStep;
        }
        target.setColor( index,
                         PCCColor3B( uint8_t( bestColor[0] ), uint8_t( bestColor[1] ), uint8_t( bestColor[2] ) ) );
      }
    }
#endif
  return true;
}

bool PCCPointSet3::transfertColorWeight( PCCPointSet3& target, const double bestColorSearchStep ) {
  const auto&  source           = *this;
  const size_t pointCountSource = source.getPointCount();
  const size_t pointCountTarget = target.getPointCount();
  if ( !pointCountSource || !pointCountTarget || !source.hasColors() ) { return false; }
  target.addColors();
  PCCKdTree    kdtreeSource( source );
  PCCNNResult  result;
  const size_t num_results = 5;
#if MULTIPLE_ATTRIBUTES
  for ( int viewIndex = 0; viewIndex < NUMBER_VIEWS; viewIndex++ ) {
    for ( size_t index = 0; index < pointCountTarget; ++index ) {
      kdtreeSource.search( target[index], num_results, result );
      double color[3] = {0., 0., 0.};
      double sum      = 0;
      if ( result.count() > 1 && result.dist( 0 ) > 0.0001 ) {
        for ( size_t i = 0; i < result.count(); i++ ) {
          const auto&  found = source.getColor( result.indices( i ), viewIndex );
          const double w     = 1.0 / pow( result.dist( i ), 2.0 );
          color[0] += found[0] * w;
          color[1] += found[1] * w;
          color[2] += found[2] * w;
          sum += w;
        }
        color[0] /= sum;
        color[1] /= sum;
        color[2] /= sum;
      } else {
        const auto& found = source.getColor( result.indices( 0 ), viewIndex );
        color[0]          = found[0];
        color[1]          = found[1];
        color[2]          = found[2];
      }
      target.setColor( index, viewIndex, PCCColor3B( uint8_t( color[0] ), uint8_t( color[1] ), uint8_t( color[2] ) ) );
    }
  }
#else
    for ( size_t index = 0; index < pointCountTarget; ++index ) {
      kdtreeSource.search( target[index], num_results, result );
      double color[3] = {0., 0., 0.};
      double sum      = 0;
      if ( result.count() > 1 && result.dist( 0 ) > 0.0001 ) {
        for ( size_t i = 0; i < result.count(); i++ ) {
          const auto&  found = source.getColor( result.indices( i ) );
          const double w     = 1.0 / pow( result.dist( i ), 2.0 );
          color[0] += found[0] * w;
          color[1] += found[1] * w;
          color[2] += found[2] * w;
          sum += w;
        }
        color[0] /= sum;
        color[1] /= sum;
        color[2] /= sum;
      } else {
        const auto& found = source.getColor( result.indices( 0 ) );
        color[0]          = found[0];
        color[1]          = found[1];
        color[2]          = found[2];
      }
      target.setColor( index, PCCColor3B( uint8_t( color[0] ), uint8_t( color[1] ), uint8_t( color[2] ) ) );
    }
#endif
  return true;
}

void PCCPointSet3::copyNormals( const PCCPointSet3& sourceWithNormal ) {
  if ( !sourceWithNormal.withNormals_ ) {
    std::cerr << "Normal object don't have normals \n" << std::endl;
    exit( -1 );
  }
  if ( sourceWithNormal.getPointCount() != getPointCount() ) {
    std::cerr << "Normal object and current object must have the same number of points \n" << std::endl;
    exit( -1 );
  }
  addNormals();

  std::map<double, std::map<double, std::map<double, size_t>>> map;
  for ( size_t i = 0; i < sourceWithNormal.positions_.size(); ++i ) {
    float x = sourceWithNormal.positions_[i][0], y = sourceWithNormal.positions_[i][1],
          z      = sourceWithNormal.positions_[i][2];
    map[x][y][z] = i;
  }
  for ( size_t i = 0; i < positions_.size(); ++i ) {
    float x = positions_[i][0], y = positions_[i][1], z = positions_[i][2];
    if ( map.find( x ) != map.end() && map[x].find( y ) != map[x].end() && map[x][y].find( z ) != map[x][y].end() ) {
      size_t index   = map[x][y][z];
      normals_[i][0] = sourceWithNormal.normals_[index][0];
      normals_[i][1] = sourceWithNormal.normals_[index][1];
      normals_[i][2] = sourceWithNormal.normals_[index][2];
    } else {
      std::cerr << "Error point " << i << " of the current points cloud is not present in the normal point cloud. \n"
                << std::endl;
      exit( -1 );
    }
  }
}

void PCCPointSet3::scaleNormals( const PCCPointSet3& sourceWithNormal ) {
  if ( !sourceWithNormal.withNormals_ ) {
    std::cerr << "Normal object don't have normals \n" << std::endl;
    exit( -1 );
  }
  addNormals();
  std::vector<size_t> count;
  count.resize( getPointCount(), 0 );
  const size_t num_results_max  = 30;
  const size_t num_results_incr = 5;
  PCCKdTree    kdtreeSrc( sourceWithNormal ), kdtreeDst( *this );
  PCCNNResult  result;
  for ( size_t i = 0; i < sourceWithNormal.getPointCount(); i++ ) {
    // For point 'i' in A, find its nearest neighbor in B. store it in 'j'
    size_t num_results = 0;
    do {
      num_results += num_results_incr;
      kdtreeDst.search( sourceWithNormal.positions_[i], num_results, result );
    } while ( result.dist( 0 ) == result.dist( num_results - 1 ) && num_results + num_results_incr <= num_results_max );
    for ( size_t j = 0; j < result.count(); j++ ) {
      if ( result.dist( 0 ) == result.dist( j ) ) {
        size_t index = result.indices( j );
        normals_[index][0] += sourceWithNormal.normals_[i][0];
        normals_[index][1] += sourceWithNormal.normals_[i][1];
        normals_[index][2] += sourceWithNormal.normals_[i][2];
        count[index]++;
      }
    }
  }

  for ( long i = 0; i < getPointCount(); i++ ) {
    if ( count[i] > 0 ) {
      normals_[i][0] /= count[i];
      normals_[i][1] /= count[i];
      normals_[i][2] /= count[i];
    } else {
      size_t num_results = 0;
      do {
        num_results += num_results_incr;
        kdtreeSrc.search( positions_[i], num_results, result );
      } while ( result.dist( 0 ) == result.dist( num_results - 1 ) &&
                num_results + num_results_incr <= num_results_max );
      size_t num = 0;
      for ( size_t j = 0; j < num_results; j++ ) {
        if ( result.dist( 0 ) == result.dist( j ) ) {
          size_t index = result.indices( j );
          normals_[i][0] += sourceWithNormal.normals_[index][0];
          normals_[i][1] += sourceWithNormal.normals_[index][1];
          normals_[i][2] += sourceWithNormal.normals_[index][2];
          num++;
        }
      }
      normals_[i][0] /= num;
      normals_[i][1] /= num;
      normals_[i][2] /= num;
    }
  }
}
