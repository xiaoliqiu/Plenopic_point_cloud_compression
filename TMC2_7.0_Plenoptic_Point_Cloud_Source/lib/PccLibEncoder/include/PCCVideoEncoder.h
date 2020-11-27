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
#ifndef PCCVideoEncoder_h
#define PCCVideoEncoder_h

#include "PCCCommon.h"
#include "PCCVideoBitstream.h"
#include "PCCSystem.h"
#include "PCCVideo.h"
#include "PCCContext.h"
#include "PCCFrameContext.h"
#include "PCCPatch.h"

namespace pcc {

class PCCContext;
class PCCFrameContext;

#if MULTIPLE_ATTRIBUTES
class PCCMultiViewEncoder {
 public:
  PCCMultiViewEncoder();
  ~PCCMultiViewEncoder();
  template <typename T>
  bool compress( PCCVideo<T, 3>& video0,
                 PCCVideo<T, 3>& video1,
                 PCCVideo<T, 3>& video2,
                 PCCVideo<T, 3>& video3,
                 PCCVideo<T, 3>& video4,
                 PCCVideo<T, 3>& video5,
                 PCCVideo<T, 3>& video6,
                 PCCVideo<T, 3>& video7,
                 PCCVideo<T, 3>& video8,
                 PCCVideo<T, 3>& video9,
                 PCCVideo<T, 3>& video10,
                 PCCVideo<T, 3>& video11,
#if NUMBER_VIEWS == 13
                 PCCVideo<T, 3>& video12,
#endif
                 const std::string& path,
                 const int          qp,
                 PCCVideoBitstream& bitstream,
                 const std::string& encoderConfig,
                 const std::string& encoderPath,
                 PCCContext&        contexts,
                 const size_t       nbyte,
                 const bool         use444CodecIo,
                 const bool         use3dmv,
                 const size_t       internalBitDepth,
                 const bool         useConversion,
                 const bool         keepIntermediateFiles             = false,
                 const std::string& colorSpaceConversionConfig        = "",
                 const std::string& inverseColorSpaceConversionConfig = "",
                 const std::string& colorSpaceConversionPath          = "",
                 const size_t       downsamplingFilter                = 4,
                 const size_t       upsamplingFilter                  = 0,
                 const bool         patchColorSubsampling             = false ) {

    auto& frames0  = video0.getFrames();
    auto& frames1  = video1.getFrames();
    auto& frames2  = video2.getFrames();
    auto& frames3  = video3.getFrames();
    auto& frames4  = video4.getFrames();
    auto& frames5  = video5.getFrames();
    auto& frames6  = video6.getFrames();
    auto& frames7  = video7.getFrames();
    auto& frames8  = video8.getFrames();
    auto& frames9  = video9.getFrames();
    auto& frames10 = video10.getFrames();
    auto& frames11 = video11.getFrames();
#if NUMBER_VIEWS == 13
    auto& frames12 = video12.getFrames();
#endif

    if ( frames0.empty() ) { return false; }
    const size_t width      = frames0[0].getWidth();
    const size_t height     = frames0[0].getHeight();
    const size_t depth      = nbyte == 1 ? 8 : 10;
    const size_t frameCount = video0.getFrameCount();
    if ( frames0[0].getChannelCount() != 3 ) { return false; }

    const std::string type                 = bitstream.getExtension();
    const std::string format               = use444CodecIo ? "444" : "420";
    const std::string fileName             = path + type;
    const std::string binFileName          = fileName + ".bin";
    const std::string blockToPatchFileName = path + "blockToPatch.txt";
    const std::string occupancyMapFileName = path + "occupancy.txt";
    const std::string patchInfoFileName    = path + "patchInfo.txt";
    const std::string srcYuvFileName0  = addVideoFormat( fileName + "_view0" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                        width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcYuvFileName1  = addVideoFormat( fileName + "_view1" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                        width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcYuvFileName2  = addVideoFormat( fileName + "_view2" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                        width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcYuvFileName3  = addVideoFormat( fileName + "_view3" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                        width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcYuvFileName4  = addVideoFormat( fileName + "_view4" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                        width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcYuvFileName5  = addVideoFormat( fileName + "_view5" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                        width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcYuvFileName6  = addVideoFormat( fileName + "_view6" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                        width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcYuvFileName7  = addVideoFormat( fileName + "_view7" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                        width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcYuvFileName8  = addVideoFormat( fileName + "_view8" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                        width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcYuvFileName9  = addVideoFormat( fileName + "_view9" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                        width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcYuvFileName10 = addVideoFormat( fileName + "_view10" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                         width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcYuvFileName11 = addVideoFormat( fileName + "_view11" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                         width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
#if NUMBER_VIEWS == 13
    const std::string srcYuvFileName12 = addVideoFormat( fileName + "_view12" + ( use444CodecIo ? ".rgb" : ".yuv" ),
                                                         width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
#endif

    const std::string srcRgbFileName0 =
        addVideoFormat( fileName + "_view0" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcRgbFileName1 =
        addVideoFormat( fileName + "_view1" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcRgbFileName2 =
        addVideoFormat( fileName + "_view2" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcRgbFileName3 =
        addVideoFormat( fileName + "_view3" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcRgbFileName4 =
        addVideoFormat( fileName + "_view4" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcRgbFileName5 =
        addVideoFormat( fileName + "_view5" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcRgbFileName6 =
        addVideoFormat( fileName + "_view6" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcRgbFileName7 =
        addVideoFormat( fileName + "_view7" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcRgbFileName8 =
        addVideoFormat( fileName + "_view8" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcRgbFileName9 =
        addVideoFormat( fileName + "_view9" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcRgbFileName10 =
        addVideoFormat( fileName + "_view10" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcRgbFileName11 =
        addVideoFormat( fileName + "_view11" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
#if NUMBER_VIEWS == 13
    const std::string srcRgbFileName12 =
        addVideoFormat( fileName + "_view12" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
#endif

    const std::string recYuvFileName0 =
        addVideoFormat( fileName + "_view0" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recYuvFileName1 =
        addVideoFormat( fileName + "_view1" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recYuvFileName2 =
        addVideoFormat( fileName + "_view2" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recYuvFileName3 =
        addVideoFormat( fileName + "_view3" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recYuvFileName4 =
        addVideoFormat( fileName + "_view4" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recYuvFileName5 =
        addVideoFormat( fileName + "_view5" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recYuvFileName6 =
        addVideoFormat( fileName + "_view6" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recYuvFileName7 =
        addVideoFormat( fileName + "_view7" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recYuvFileName8 =
        addVideoFormat( fileName + "_view8" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recYuvFileName9 =
        addVideoFormat( fileName + "_view9" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recYuvFileName10 =
        addVideoFormat( fileName + "_view10" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recYuvFileName11 =
        addVideoFormat( fileName + "_view11" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
#if NUMBER_VIEWS == 13
    const std::string recYuvFileName12 =
        addVideoFormat( fileName + "_view12" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, nbyte == 2 ? "10" : "8" );
#endif

    const std::string recRgbFileName0 =
        addVideoFormat( fileName + "_view0" + "_rec" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recRgbFileName1 =
        addVideoFormat( fileName + "_view1" + "_rec" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recRgbFileName2 =
        addVideoFormat( fileName + "_view2" + "_rec" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recRgbFileName3 =
        addVideoFormat( fileName + "_view3" + "_rec" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recRgbFileName4 =
        addVideoFormat( fileName + "_view4" + "_rec" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recRgbFileName5 =
        addVideoFormat( fileName + "_view5" + "_rec" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recRgbFileName6 =
        addVideoFormat( fileName + "_view6" + "_rec" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recRgbFileName7 =
        addVideoFormat( fileName + "_view7" + "_rec" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recRgbFileName8 =
        addVideoFormat( fileName + "_view8" + "_rec" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recRgbFileName9 =
        addVideoFormat( fileName + "_view9" + "_rec" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recRgbFileName10 = addVideoFormat( fileName + "_view10" + "_rec" + ".rgb", width, height,
                                                         !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recRgbFileName11 = addVideoFormat( fileName + "_view11" + "_rec" + ".rgb", width, height,
                                                         !use444CodecIo, nbyte == 2 ? "10" : "8" );
#if NUMBER_VIEWS == 13
    const std::string recRgbFileName12 = addVideoFormat( fileName + "_view12" + "_rec" + ".rgb", width, height,
                                                         !use444CodecIo, nbyte == 2 ? "10" : "8" );
#endif

    const bool yuvVideo = colorSpaceConversionConfig.empty() || use444CodecIo;
    printf( "Encoder convert : yuvVideo = %d colorSpaceConversionConfig = %s \n", yuvVideo,
            colorSpaceConversionConfig.c_str() );
    printf( "Encoder convert : colorSpaceConversionPath = %s \n", colorSpaceConversionPath.c_str() );
    if ( yuvVideo ) {
      if ( use444CodecIo ) {
        if ( !video0.write( srcYuvFileName0, nbyte ) ) { return false; }
        if ( !video1.write( srcYuvFileName1, nbyte ) ) { return false; }
        if ( !video2.write( srcYuvFileName2, nbyte ) ) { return false; }
        if ( !video3.write( srcYuvFileName3, nbyte ) ) { return false; }
        if ( !video4.write( srcYuvFileName4, nbyte ) ) { return false; }
        if ( !video5.write( srcYuvFileName5, nbyte ) ) { return false; }
        if ( !video6.write( srcYuvFileName6, nbyte ) ) { return false; }
        if ( !video7.write( srcYuvFileName7, nbyte ) ) { return false; }
        if ( !video8.write( srcYuvFileName8, nbyte ) ) { return false; }
        if ( !video9.write( srcYuvFileName9, nbyte ) ) { return false; }
        if ( !video10.write( srcYuvFileName10, nbyte ) ) { return false; }
        if ( !video11.write( srcYuvFileName11, nbyte ) ) { return false; }
#if NUMBER_VIEWS == 13
        if ( !video12.write( srcYuvFileName12, nbyte ) ) { return false; }
#endif
      } else {
        printf( "Encoder convert : write420 without conversion \n" );
        if ( !video0.write420( srcYuvFileName0, nbyte ) ) { return false; }
        if ( !video1.write420( srcYuvFileName1, nbyte ) ) { return false; }
        if ( !video2.write420( srcYuvFileName2, nbyte ) ) { return false; }
        if ( !video3.write420( srcYuvFileName3, nbyte ) ) { return false; }
        if ( !video4.write420( srcYuvFileName4, nbyte ) ) { return false; }
        if ( !video5.write420( srcYuvFileName5, nbyte ) ) { return false; }
        if ( !video6.write420( srcYuvFileName6, nbyte ) ) { return false; }
        if ( !video7.write420( srcYuvFileName7, nbyte ) ) { return false; }
        if ( !video8.write420( srcYuvFileName8, nbyte ) ) { return false; }
        if ( !video9.write420( srcYuvFileName9, nbyte ) ) { return false; }
        if ( !video10.write420( srcYuvFileName10, nbyte ) ) { return false; }
        if ( !video11.write420( srcYuvFileName11, nbyte ) ) { return false; }
#if NUMBER_VIEWS == 13
        if ( !video12.write420( srcYuvFileName12, nbyte ) ) { return false; }
#endif
      }
    } else {
      if ( colorSpaceConversionPath.empty() ) {
        printf( "Encoder convert : write420 with conversion \n" );
        // if ( keepIntermediateFiles ) { video.write( srcRgbFileName, nbyte ); }
        if ( !video0.write420( srcYuvFileName0, nbyte, true, downsamplingFilter ) ) { return false; }
        if ( !video1.write420( srcYuvFileName1, nbyte, true, downsamplingFilter ) ) { return false; }
        if ( !video2.write420( srcYuvFileName2, nbyte, true, downsamplingFilter ) ) { return false; }
        if ( !video3.write420( srcYuvFileName3, nbyte, true, downsamplingFilter ) ) { return false; }
        if ( !video4.write420( srcYuvFileName4, nbyte, true, downsamplingFilter ) ) { return false; }
        if ( !video5.write420( srcYuvFileName5, nbyte, true, downsamplingFilter ) ) { return false; }
        if ( !video6.write420( srcYuvFileName6, nbyte, true, downsamplingFilter ) ) { return false; }
        if ( !video7.write420( srcYuvFileName7, nbyte, true, downsamplingFilter ) ) { return false; }
        if ( !video8.write420( srcYuvFileName8, nbyte, true, downsamplingFilter ) ) { return false; }
        if ( !video9.write420( srcYuvFileName9, nbyte, true, downsamplingFilter ) ) { return false; }
        if ( !video10.write420( srcYuvFileName10, nbyte, true, downsamplingFilter ) ) { return false; }
        if ( !video11.write420( srcYuvFileName11, nbyte, true, downsamplingFilter ) ) { return false; }
#if NUMBER_VIEWS == 13
        if ( !video12.write420( srcYuvFileName12, nbyte, true, downsamplingFilter ) ) { return false; }
#endif
      } else {
        printf( "Encoder convert : write + hdrtools conversion \n" );
        if ( !video0.write( srcRgbFileName0, nbyte ) ) { return false; }
        if ( !video1.write( srcRgbFileName1, nbyte ) ) { return false; }
        if ( !video2.write( srcRgbFileName2, nbyte ) ) { return false; }
        if ( !video3.write( srcRgbFileName3, nbyte ) ) { return false; }
        if ( !video4.write( srcRgbFileName4, nbyte ) ) { return false; }
        if ( !video5.write( srcRgbFileName5, nbyte ) ) { return false; }
        if ( !video6.write( srcRgbFileName6, nbyte ) ) { return false; }
        if ( !video7.write( srcRgbFileName7, nbyte ) ) { return false; }
        if ( !video8.write( srcRgbFileName8, nbyte ) ) { return false; }
        if ( !video9.write( srcRgbFileName9, nbyte ) ) { return false; }
        if ( !video10.write( srcRgbFileName10, nbyte ) ) { return false; }
        if ( !video11.write( srcRgbFileName11, nbyte ) ) { return false; }
#if NUMBER_VIEWS == 13
        if ( !video12.write( srcRgbFileName12, nbyte ) ) { return false; }
#endif

        std::stringstream cmd0;
        cmd0 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
             << srcRgbFileName0 << "\""
             << " -p OutputFile=\"" << srcYuvFileName0 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd0.str() << '\n';
        if ( pcc::system( cmd0.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd1;
        cmd1 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
             << srcRgbFileName1 << "\""
             << " -p OutputFile=\"" << srcYuvFileName1 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd1.str() << '\n';
        if ( pcc::system( cmd1.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd2;
        cmd2 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
             << srcRgbFileName2 << "\""
             << " -p OutputFile=\"" << srcYuvFileName2 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd2.str() << '\n';
        if ( pcc::system( cmd2.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd3;
        cmd3 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
             << srcRgbFileName3 << "\""
             << " -p OutputFile=\"" << srcYuvFileName3 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd3.str() << '\n';
        if ( pcc::system( cmd3.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd4;
        cmd4 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
             << srcRgbFileName4 << "\""
             << " -p OutputFile=\"" << srcYuvFileName4 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd4.str() << '\n';
        if ( pcc::system( cmd4.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd5;
        cmd5 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
             << srcRgbFileName5 << "\""
             << " -p OutputFile=\"" << srcYuvFileName5 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd5.str() << '\n';
        if ( pcc::system( cmd5.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd6;
        cmd6 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
             << srcRgbFileName6 << "\""
             << " -p OutputFile=\"" << srcYuvFileName6 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd6.str() << '\n';
        if ( pcc::system( cmd6.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd7;
        cmd7 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
             << srcRgbFileName7 << "\""
             << " -p OutputFile=\"" << srcYuvFileName7 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd7.str() << '\n';
        if ( pcc::system( cmd7.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd8;
        cmd8 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
             << srcRgbFileName8 << "\""
             << " -p OutputFile=\"" << srcYuvFileName8 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd8.str() << '\n';
        if ( pcc::system( cmd8.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd9;
        cmd9 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
             << srcRgbFileName9 << "\""
             << " -p OutputFile=\"" << srcYuvFileName9 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd9.str() << '\n';
        if ( pcc::system( cmd9.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd10;
        cmd10 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
              << srcRgbFileName10 << "\""
              << " -p OutputFile=\"" << srcYuvFileName10 << "\""
              << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd10.str() << '\n';
        if ( pcc::system( cmd10.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd11;
        cmd11 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
              << srcRgbFileName11 << "\""
              << " -p OutputFile=\"" << srcYuvFileName11 << "\""
              << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd11.str() << '\n';
        if ( pcc::system( cmd11.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

#if NUMBER_VIEWS == 13
        std::stringstream cmd12;
        cmd12 << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
              << srcRgbFileName12 << "\""
              << " -p OutputFile=\"" << srcYuvFileName12 << "\""
              << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd12.str() << '\n';
        if ( pcc::system( cmd12.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }
#endif
      }
    }

    std::stringstream cmd;
    if ( use444CodecIo ) {
      cmd << encoderPath << " -c " << encoderConfig << " --InputFile_0=" << srcYuvFileName0 << " --InputFile_1=" << srcYuvFileName1
          << " --InputFile_2=" << srcYuvFileName2 << " --InputFile_3=" << srcYuvFileName3
          << " --InputFile_4=" << srcYuvFileName4 << " --InputFile_5=" << srcYuvFileName5
          << " --InputFile_6=" << srcYuvFileName6 << " --InputFile_7=" << srcYuvFileName7
          << " --InputFile_8=" << srcYuvFileName8 << " --InputFile_9=" << srcYuvFileName9
          << " --InputFile_10=" << srcYuvFileName10 << " --InputFile_11=" << srcYuvFileName11
#if NUMBER_VIEWS == 13
          << " --InputFile_12=" << srcYuvFileName12
#endif
          << " --InputBitDepth=" << depth << " --InternalBitDepth=" << depth << " --InternalBitDepthC=" << depth
          << " --InputChromaFormat=" << format << " --FrameRate=30 "
          << " --FrameSkip=0 "
          << " --SourceWidth=" << width << " --SourceHeight=" << height << " --ConformanceWindowMode=1 "
          << " --FramesToBeEncoded=" << frameCount << " --BitstreamFile=" << binFileName
          << " --ReconFile_0=" << recYuvFileName0 << " --ReconFile_1=" << recYuvFileName1 
          << " --ReconFile_2=" << recYuvFileName2 << " --ReconFile_3=" << recYuvFileName3 
          << " --ReconFile_4=" << recYuvFileName4 << " --ReconFile_5=" << recYuvFileName5 
          << " --ReconFile_6=" << recYuvFileName6 << " --ReconFile_7=" << recYuvFileName7 
          << " --ReconFile_8=" << recYuvFileName8 << " --ReconFile_9=" << recYuvFileName9 
          << " --ReconFile_10=" << recYuvFileName10 << " --ReconFile_11=" << recYuvFileName11
#if NUMBER_VIEWS == 13
          << " --ReconFile_12=" << recYuvFileName12
#endif
          << " --QP=" << qp << " --InputColourSpaceConvert=RGBtoGBR";

#if MULTIVIEW_UNOCCUPIED_RDO
      cmd << " --BlockToPatchFile=" << blockToPatchFileName << " --OccupancyMapFile=" << occupancyMapFileName
          << " --PatchInfoFile=" << patchInfoFileName;
#endif
    } else {
      cmd << encoderPath << " -c " << encoderConfig
          << " --InputFile_0=" << srcYuvFileName0 << " --InputFile_1=" << srcYuvFileName1
          << " --InputFile_2=" << srcYuvFileName2 << " --InputFile_3=" << srcYuvFileName3
          << " --InputFile_4=" << srcYuvFileName4 << " --InputFile_5=" << srcYuvFileName5
          << " --InputFile_6=" << srcYuvFileName6 << " --InputFile_7=" << srcYuvFileName7
          << " --InputFile_8=" << srcYuvFileName8 << " --InputFile_9=" << srcYuvFileName9
          << " --InputFile_10=" << srcYuvFileName10 << " --InputFile_11=" << srcYuvFileName11
#if NUMBER_VIEWS == 13
          << " --InputFile_12=" << srcYuvFileName12
#endif
          << " --InputBitDepth=" << depth << " --InputChromaFormat=" << format << " --FrameRate=30 "
          << " --FrameSkip=0 "
          << " --SourceWidth=" << width << " --SourceHeight=" << height << " --ConformanceWindowMode=1 "
          << " --FramesToBeEncoded=" << frameCount << " --BitstreamFile=" << binFileName
          << " --ReconFile_0=" << recYuvFileName0
          << " --ReconFile_1=" << recYuvFileName1 << " --ReconFile_2=" << recYuvFileName2
          << " --ReconFile_3=" << recYuvFileName3 << " --ReconFile_4=" << recYuvFileName4
          << " --ReconFile_5=" << recYuvFileName5 << " --ReconFile_6=" << recYuvFileName6
          << " --ReconFile_7=" << recYuvFileName7 << " --ReconFile_8=" << recYuvFileName8
          << " --ReconFile_9=" << recYuvFileName9 << " --ReconFile_10=" << recYuvFileName10
          << " --ReconFile_11=" << recYuvFileName11
#if NUMBER_VIEWS == 13
          << " --ReconFile_12=" << recYuvFileName12
#endif
          << " --QP=" << qp;
      if ( internalBitDepth != 0 ) {
        cmd << " --InternalBitDepth=" << depth << " --InternalBitDepthC=" << depth;  // use the input depth temporally
      }

      cmd << " --OutputBitDepth=" << depth;
      cmd << " --OutputBitDepthC=" << depth;
#if MULTIVIEW_UNOCCUPIED_RDO
      cmd << " --BlockToPatchFile=" << blockToPatchFileName << " --OccupancyMapFile=" << occupancyMapFileName
          << " --PatchInfoFile=" << patchInfoFileName;
#endif
    }
    std::cout << cmd.str() << std::endl;
    if ( pcc::system( cmd.str().c_str() ) ) {
      std::cout << "Error: can't run system command!" << std::endl;
      return false;
    }

    std::ifstream file( binFileName, std::ios::binary | std::ios::ate );
    if ( !file.good() ) { return false; }
    const uint64_t fileSize = file.tellg();
    bitstream.resize( (size_t)fileSize );
    file.clear();
    file.seekg( 0 );
    file.read( reinterpret_cast<char*>( bitstream.buffer() ), fileSize );
    file.close();

    if ( yuvVideo ) {
      if ( use444CodecIo ) {
        video0.read( recYuvFileName0, width, height, frameCount, nbyte );
        video1.read( recYuvFileName1, width, height, frameCount, nbyte );
        video2.read( recYuvFileName2, width, height, frameCount, nbyte );
        video3.read( recYuvFileName3, width, height, frameCount, nbyte );
        video4.read( recYuvFileName4, width, height, frameCount, nbyte );
        video5.read( recYuvFileName5, width, height, frameCount, nbyte );
        video6.read( recYuvFileName6, width, height, frameCount, nbyte );
        video7.read( recYuvFileName7, width, height, frameCount, nbyte );
        video8.read( recYuvFileName8, width, height, frameCount, nbyte );
        video9.read( recYuvFileName9, width, height, frameCount, nbyte );
        video10.read( recYuvFileName10, width, height, frameCount, nbyte );
        video11.read( recYuvFileName11, width, height, frameCount, nbyte );
#if NUMBER_VIEWS == 13
        video12.read( recYuvFileName12, width, height, frameCount, nbyte );
#endif
      } else {
        video0.read420( recYuvFileName0, width, height, frameCount, nbyte );
        video1.read420( recYuvFileName1, width, height, frameCount, nbyte );
        video2.read420( recYuvFileName2, width, height, frameCount, nbyte );
        video3.read420( recYuvFileName3, width, height, frameCount, nbyte );
        video4.read420( recYuvFileName4, width, height, frameCount, nbyte );
        video5.read420( recYuvFileName5, width, height, frameCount, nbyte );
        video6.read420( recYuvFileName6, width, height, frameCount, nbyte );
        video7.read420( recYuvFileName7, width, height, frameCount, nbyte );
        video8.read420( recYuvFileName8, width, height, frameCount, nbyte );
        video9.read420( recYuvFileName9, width, height, frameCount, nbyte );
        video10.read420( recYuvFileName10, width, height, frameCount, nbyte );
        video11.read420( recYuvFileName11, width, height, frameCount, nbyte );
#if NUMBER_VIEWS == 13
        video12.read420( recYuvFileName12, width, height, frameCount, nbyte );
#endif
      }
    } else {
      if ( colorSpaceConversionPath.empty() ) {
        video0.read420( recYuvFileName0, width, height, frameCount, nbyte, true, upsamplingFilter );
        video1.read420( recYuvFileName1, width, height, frameCount, nbyte, true, upsamplingFilter );
        video2.read420( recYuvFileName2, width, height, frameCount, nbyte, true, upsamplingFilter );
        video3.read420( recYuvFileName3, width, height, frameCount, nbyte, true, upsamplingFilter );
        video4.read420( recYuvFileName4, width, height, frameCount, nbyte, true, upsamplingFilter );
        video5.read420( recYuvFileName5, width, height, frameCount, nbyte, true, upsamplingFilter );
        video6.read420( recYuvFileName6, width, height, frameCount, nbyte, true, upsamplingFilter );
        video7.read420( recYuvFileName7, width, height, frameCount, nbyte, true, upsamplingFilter );
        video8.read420( recYuvFileName8, width, height, frameCount, nbyte, true, upsamplingFilter );
        video9.read420( recYuvFileName9, width, height, frameCount, nbyte, true, upsamplingFilter );
        video10.read420( recYuvFileName10, width, height, frameCount, nbyte, true, upsamplingFilter );
        video11.read420( recYuvFileName11, width, height, frameCount, nbyte, true, upsamplingFilter );
#if NUMBER_VIEWS == 13
        video12.read420( recYuvFileName12, width, height, frameCount, nbyte, true, upsamplingFilter );
#endif
        if ( !keepIntermediateFiles ) {
          video0.write( recRgbFileName0, nbyte );
          video1.write( recRgbFileName1, nbyte );
          video2.write( recRgbFileName2, nbyte );
          video3.write( recRgbFileName3, nbyte );
          video4.write( recRgbFileName4, nbyte );
          video5.write( recRgbFileName5, nbyte );
          video6.write( recRgbFileName6, nbyte );
          video7.write( recRgbFileName7, nbyte );
          video8.write( recRgbFileName8, nbyte );
          video9.write( recRgbFileName9, nbyte );
          video10.write( recRgbFileName10, nbyte );
          video11.write( recRgbFileName11, nbyte );
#if NUMBER_VIEWS == 13
          video12.write( recRgbFileName12, nbyte );
#endif
        }
      } else {
        std::stringstream cmd0;
        cmd0 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << recYuvFileName0 << "\""
             << " -p OutputFile=\"" << recRgbFileName0 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd0.str() << '\n';
        if ( int ret = pcc::system( cmd0.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }

        std::stringstream cmd1;
        cmd1 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << recYuvFileName1 << "\""
             << " -p OutputFile=\"" << recRgbFileName1 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd1.str() << '\n';
        if ( int ret = pcc::system( cmd1.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }

        std::stringstream cmd2;
        cmd2 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << recYuvFileName2 << "\""
             << " -p OutputFile=\"" << recRgbFileName2 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd2.str() << '\n';
        if ( int ret = pcc::system( cmd2.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }

        std::stringstream cmd3;
        cmd3 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << recYuvFileName3 << "\""
             << " -p OutputFile=\"" << recRgbFileName3 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd3.str() << '\n';
        if ( int ret = pcc::system( cmd3.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }

        std::stringstream cmd4;
        cmd4 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << recYuvFileName4 << "\""
             << " -p OutputFile=\"" << recRgbFileName4 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd4.str() << '\n';
        if ( int ret = pcc::system( cmd4.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }

        std::stringstream cmd5;
        cmd5 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << recYuvFileName5 << "\""
             << " -p OutputFile=\"" << recRgbFileName5 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd5.str() << '\n';
        if ( int ret = pcc::system( cmd5.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }

        std::stringstream cmd6;
        cmd6 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << recYuvFileName6 << "\""
             << " -p OutputFile=\"" << recRgbFileName6 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd6.str() << '\n';
        if ( int ret = pcc::system( cmd6.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }

        std::stringstream cmd7;
        cmd7 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << recYuvFileName7 << "\""
             << " -p OutputFile=\"" << recRgbFileName7 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd7.str() << '\n';
        if ( int ret = pcc::system( cmd7.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }

        std::stringstream cmd8;
        cmd8 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << recYuvFileName8 << "\""
             << " -p OutputFile=\"" << recRgbFileName8 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd8.str() << '\n';
        if ( int ret = pcc::system( cmd8.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }

        std::stringstream cmd9;
        cmd9 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << recYuvFileName9 << "\""
             << " -p OutputFile=\"" << recRgbFileName9 << "\""
             << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd9.str() << '\n';
        if ( int ret = pcc::system( cmd9.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }

        std::stringstream cmd10;
        cmd10 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
              << recYuvFileName10 << "\""
              << " -p OutputFile=\"" << recRgbFileName10 << "\""
              << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd10.str() << '\n';
        if ( int ret = pcc::system( cmd10.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }

        std::stringstream cmd11;
        cmd11 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
              << recYuvFileName11 << "\""
              << " -p OutputFile=\"" << recRgbFileName11 << "\""
              << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd11.str() << '\n';
        if ( int ret = pcc::system( cmd11.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }

#if NUMBER_VIEWS == 13
        std::stringstream cmd12;
        cmd12 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
              << recYuvFileName12 << "\""
              << " -p OutputFile=\"" << recRgbFileName12 << "\""
              << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd12.str() << '\n';
        if ( int ret = pcc::system( cmd12.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }
#endif

        video0.read( recRgbFileName0, width, height, frameCount, nbyte );
        video1.read( recRgbFileName1, width, height, frameCount, nbyte );
        video2.read( recRgbFileName2, width, height, frameCount, nbyte );
        video3.read( recRgbFileName3, width, height, frameCount, nbyte );
        video4.read( recRgbFileName4, width, height, frameCount, nbyte );
        video5.read( recRgbFileName5, width, height, frameCount, nbyte );
        video6.read( recRgbFileName6, width, height, frameCount, nbyte );
        video7.read( recRgbFileName7, width, height, frameCount, nbyte );
        video8.read( recRgbFileName8, width, height, frameCount, nbyte );
        video9.read( recRgbFileName9, width, height, frameCount, nbyte );
        video10.read( recRgbFileName10, width, height, frameCount, nbyte );
        video11.read( recRgbFileName11, width, height, frameCount, nbyte );
#if NUMBER_VIEWS == 13
        video12.read( recRgbFileName12, width, height, frameCount, nbyte );
#endif
      }
    }
    if ( !keepIntermediateFiles ) {
      removeFile( binFileName );
      removeFile( srcYuvFileName0 );
      removeFile( srcRgbFileName0 );
      removeFile( recYuvFileName0 );
      removeFile( recRgbFileName0 );
      removeFile( srcYuvFileName1 );
      removeFile( srcRgbFileName1 );
      removeFile( recYuvFileName1 );
      removeFile( recRgbFileName1 );
      removeFile( srcYuvFileName2 );
      removeFile( srcRgbFileName2 );
      removeFile( recYuvFileName2 );
      removeFile( recRgbFileName2 );
      removeFile( srcYuvFileName3 );
      removeFile( srcRgbFileName3 );
      removeFile( recYuvFileName3 );
      removeFile( recRgbFileName3 );
      removeFile( srcYuvFileName4 );
      removeFile( srcRgbFileName4 );
      removeFile( recYuvFileName4 );
      removeFile( recRgbFileName4 );
      removeFile( srcYuvFileName5 );
      removeFile( srcRgbFileName5 );
      removeFile( recYuvFileName5 );
      removeFile( recRgbFileName5 );
      removeFile( srcYuvFileName6 );
      removeFile( srcRgbFileName6 );
      removeFile( recYuvFileName6 );
      removeFile( recRgbFileName6 );
      removeFile( srcYuvFileName7 );
      removeFile( srcRgbFileName7 );
      removeFile( recYuvFileName7 );
      removeFile( recRgbFileName7 );
      removeFile( srcYuvFileName8 );
      removeFile( srcRgbFileName8 );
      removeFile( recYuvFileName8 );
      removeFile( recRgbFileName8 );
      removeFile( srcYuvFileName9 );
      removeFile( srcRgbFileName9 );
      removeFile( recYuvFileName9 );
      removeFile( recRgbFileName9 );
      removeFile( srcYuvFileName10 );
      removeFile( srcRgbFileName10 );
      removeFile( recYuvFileName10 );
      removeFile( recRgbFileName10 );
      removeFile( srcYuvFileName11 );
      removeFile( srcRgbFileName11 );
      removeFile( recYuvFileName11 );
      removeFile( recRgbFileName11 );
#if NUMBER_VIEWS == 13
      removeFile( srcYuvFileName12 );
      removeFile( srcRgbFileName12 );
      removeFile( recYuvFileName12 );
      removeFile( recRgbFileName12 );
#endif
    }
    return true;
  }

 private:
};
#endif

class PCCVideoEncoder {
 public:
  PCCVideoEncoder();
  ~PCCVideoEncoder();
  template <typename T>
  bool compress( PCCVideo<T, 3>&    video,
                 const std::string& path,
                 const int          qp,
                 PCCVideoBitstream& bitstream,
                 const std::string& encoderConfig,
                 const std::string& encoderPath,
                 PCCContext&        contexts,
                 const size_t       nbyte,
                 const bool         use444CodecIo,
                 const bool         use3dmv,
                 const size_t       internalBitDepth,
                 const bool         useConversion,
                 const bool         keepIntermediateFiles             = false,
                 const std::string& colorSpaceConversionConfig        = "",
                 const std::string& inverseColorSpaceConversionConfig = "",
                 const std::string& colorSpaceConversionPath          = "",
                 const size_t       downsamplingFilter                = 4,
                 const size_t       upsamplingFilter                  = 0,
                 const bool         patchColorSubsampling             = false ) {
    auto& frames = video.getFrames();
    if ( frames.empty() ) { return false; }
    const size_t width      = frames[0].getWidth();
    const size_t height     = frames[0].getHeight();
    const size_t depth      = nbyte == 1 ? 8 : 10;
    const size_t frameCount = video.getFrameCount();
    if ( frames[0].getChannelCount() != 3 ) { return false; }

    const std::string type                 = bitstream.getExtension();
    const std::string format               = use444CodecIo ? "444" : "420";
    const std::string fileName             = path + type;
    const std::string binFileName          = fileName + ".bin";
    const std::string blockToPatchFileName = path + "blockToPatch.txt";
    const std::string occupancyMapFileName = path + "occupancy.txt";
    const std::string patchInfoFileName    = path + "patchInfo.txt";
    const std::string srcYuvFileName = addVideoFormat( fileName + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                                                       !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string srcRgbFileName =
        addVideoFormat( fileName + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recYuvFileName = addVideoFormat( fileName + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width,
                                                       height, !use444CodecIo, nbyte == 2 ? "10" : "8" );
    const std::string recRgbFileName =
        addVideoFormat( fileName + "_rec" + ".rgb", width, height, !use444CodecIo, nbyte == 2 ? "10" : "8" );

    const bool yuvVideo = colorSpaceConversionConfig.empty() || use444CodecIo;
    printf( "Encoder convert : yuvVideo = %d colorSpaceConversionConfig = %s \n", yuvVideo,
            colorSpaceConversionConfig.c_str() );
    printf( "Encoder convert : colorSpaceConversionPath = %s \n", colorSpaceConversionPath.c_str() );
    if ( yuvVideo ) {
      if ( use444CodecIo ) {
        if ( !video.write( srcYuvFileName, nbyte ) ) { return false; }
      } else {
        printf( "Encoder convert : write420 without conversion \n" );
        if ( !video.write420( srcYuvFileName, nbyte ) ) { return false; }
      }
    } else {
      if ( patchColorSubsampling ) {
        PCCVideo<T, 3> video420;
        // perform color-subsampling based on patch information
        video420.resize( video.getFrameCount() );
        for ( size_t frNum = 0; frNum < video.getFrameCount(); frNum++ ) {
          // context variable, contains the patch information
          auto& context = contexts[(int)( frNum / 2 )];
          // full resolution image (already filled by previous dilation
          auto& refImage = video.getFrame( frNum );
          // image that will contain the per-patch chroma sub-sampled image
          auto& destImage = video420.getFrame( frNum );
          destImage.resize( width, height );

          // iterate the patch information and perform chroma down-sampling on each patch individually
          std::vector<PCCPatch> patches      = context.getPatches();
          std::vector<size_t>   blockToPatch = context.getBlockToPatch();
          for ( int patchIdx = 0; patchIdx <= patches.size(); patchIdx++ ) {
            size_t occupancyResolution;
            size_t patch_left;
            size_t patch_top;
            size_t patch_width;
            size_t patch_height;
            if ( patchIdx == 0 ) {
              // background, does not have a corresponding patch
              auto& patch         = patches[0];
              occupancyResolution = patch.getOccupancyResolution();
              patch_left          = 0;
              patch_top           = 0;
              patch_width         = width;
              patch_height        = height;
            } else {
              auto& patch         = patches[patchIdx - 1];
              occupancyResolution = patch.getOccupancyResolution();
              patch_left          = patch.getU0() * occupancyResolution;
              patch_top           = patch.getV0() * occupancyResolution;
              if ( !( patch.isPatchDimensionSwitched() ) ) {
                patch_width  = patch.getSizeU0() * occupancyResolution;
                patch_height = patch.getSizeV0() * occupancyResolution;
              } else {
                patch_width  = patch.getSizeV0() * occupancyResolution;
                patch_height = patch.getSizeU0() * occupancyResolution;
              }
            }
            // initializing the image container with zeros
            PCCImage<T, 3> tmpImage;
            tmpImage.resize( patch_width, patch_height );
            // cut out the patch image
            refImage.copyBlock( patch_top, patch_left, patch_width, patch_height, tmpImage );

            // fill in the blocks by extending the edges
            for ( size_t i = 0; i < patch_height / occupancyResolution; i++ ) {
              for ( size_t j = 0; j < patch_width / occupancyResolution; j++ ) {
                if ( context
                         .getBlockToPatch()[( i + patch_top / occupancyResolution ) * ( width / occupancyResolution ) +
                                            j + patch_left / occupancyResolution] == patchIdx ) {
                  // do nothing
                  continue;
                } else {
                  // search for the block that contains texture information and extend the block edge
                  int              direction;
                  int              searchIndex;
                  std::vector<int> neighborIdx( 4, -1 );
                  std::vector<int> neighborDistance( 4, ( std::numeric_limits<int>::max )() );
                  // looking for the neighboring block to the left of the current block
                  searchIndex = (int)j;
                  while ( searchIndex >= 0 ) {
                    if ( context.getBlockToPatch()[( i + patch_top / occupancyResolution ) *
                                                       ( width / occupancyResolution ) +
                                                   searchIndex + patch_left / occupancyResolution] == patchIdx ) {
                      neighborIdx[0]      = searchIndex;
                      neighborDistance[0] = (int)j - searchIndex;
                      searchIndex         = 0;
                    }
                    searchIndex--;
                  }
                  // looking for the neighboring block to the right of the current block
                  searchIndex = (int)j;
                  while ( searchIndex < patch_width / occupancyResolution ) {
                    if ( context.getBlockToPatch()[( i + patch_top / occupancyResolution ) *
                                                       ( width / occupancyResolution ) +
                                                   searchIndex + patch_left / occupancyResolution] == patchIdx ) {
                      neighborIdx[1]      = searchIndex;
                      neighborDistance[1] = searchIndex - (int)j;
                      searchIndex         = (int)patch_width / occupancyResolution;
                    }
                    searchIndex++;
                  }
                  // looking for the neighboring block above the current block
                  searchIndex = (int)i;
                  while ( searchIndex >= 0 ) {
                    if ( context.getBlockToPatch()[( searchIndex + patch_top / occupancyResolution ) *
                                                       ( width / occupancyResolution ) +
                                                   j + patch_left / occupancyResolution] == patchIdx ) {
                      neighborIdx[2]      = searchIndex;
                      neighborDistance[2] = (int)i - searchIndex;
                      searchIndex         = 0;
                    }
                    searchIndex--;
                  }
                  // looking for the neighboring block below the current block
                  searchIndex = (int)i;
                  while ( searchIndex < patch_height / occupancyResolution ) {
                    if ( context.getBlockToPatch()[( searchIndex + patch_top / occupancyResolution ) *
                                                       ( width / occupancyResolution ) +
                                                   j + patch_left / occupancyResolution] == patchIdx ) {
                      neighborIdx[3]      = searchIndex;
                      neighborDistance[3] = searchIndex - (int)i;
                      searchIndex         = (int)patch_height / occupancyResolution;
                    }
                    searchIndex++;
                  }
                  // check if the candidate was found
                  assert( *( std::max )( neighborIdx.begin(), neighborIdx.end() ) > 0 );
                  // now fill in the block with the edge value coming from the nearest neighbor
                  direction =
                      std::min_element( neighborDistance.begin(), neighborDistance.end() ) - neighborDistance.begin();
                  if ( direction == 0 ) {
                    // copying from left neighboring block
                    for ( size_t iBlk = 0; iBlk < occupancyResolution; iBlk++ ) {
                      for ( size_t jBlk = 0; jBlk < occupancyResolution; jBlk++ ) {
                        tmpImage.setValue(
                            0, j * occupancyResolution + jBlk, i * occupancyResolution + iBlk,
                            tmpImage.getValue( 0, neighborIdx[0] * occupancyResolution + occupancyResolution - 1,
                                               i * occupancyResolution + iBlk ) );
                        tmpImage.setValue(
                            1, j * occupancyResolution + jBlk, i * occupancyResolution + iBlk,
                            tmpImage.getValue( 1, neighborIdx[0] * occupancyResolution + occupancyResolution - 1,
                                               i * occupancyResolution + iBlk ) );
                        tmpImage.setValue(
                            2, j * occupancyResolution + jBlk, i * occupancyResolution + iBlk,
                            tmpImage.getValue( 2, neighborIdx[0] * occupancyResolution + occupancyResolution - 1,
                                               i * occupancyResolution + iBlk ) );
                      }
                    }
                  } else if ( direction == 1 ) {
                    // copying block from right neighboring position
                    for ( size_t iBlk = 0; iBlk < occupancyResolution; iBlk++ ) {
                      for ( size_t jBlk = 0; jBlk < occupancyResolution; jBlk++ ) {
                        tmpImage.setValue( 0, j * occupancyResolution + jBlk, i * occupancyResolution + iBlk,
                                           tmpImage.getValue( 0, neighborIdx[1] * occupancyResolution,
                                                              i * occupancyResolution + iBlk ) );
                        tmpImage.setValue( 1, j * occupancyResolution + jBlk, i * occupancyResolution + iBlk,
                                           tmpImage.getValue( 1, neighborIdx[1] * occupancyResolution,
                                                              i * occupancyResolution + iBlk ) );
                        tmpImage.setValue( 2, j * occupancyResolution + jBlk, i * occupancyResolution + iBlk,
                                           tmpImage.getValue( 2, neighborIdx[1] * occupancyResolution,
                                                              i * occupancyResolution + iBlk ) );
                      }
                    }
                  } else if ( direction == 2 ) {
                    // copying block from above
                    for ( size_t iBlk = 0; iBlk < occupancyResolution; iBlk++ ) {
                      for ( size_t jBlk = 0; jBlk < occupancyResolution; jBlk++ ) {
                        tmpImage.setValue(
                            0, j * occupancyResolution + jBlk, i * occupancyResolution + iBlk,
                            tmpImage.getValue( 0, j * occupancyResolution + jBlk,
                                               neighborIdx[2] * occupancyResolution + occupancyResolution - 1 ) );
                        tmpImage.setValue(
                            1, j * occupancyResolution + jBlk, i * occupancyResolution + iBlk,
                            tmpImage.getValue( 1, j * occupancyResolution + jBlk,
                                               neighborIdx[2] * occupancyResolution + occupancyResolution - 1 ) );
                        tmpImage.setValue(
                            2, j * occupancyResolution + jBlk, i * occupancyResolution + iBlk,
                            tmpImage.getValue( 2, j * occupancyResolution + jBlk,
                                               neighborIdx[2] * occupancyResolution + occupancyResolution - 1 ) );
                      }
                    }
                  } else if ( direction == 3 ) {
                    // copying block from below
                    for ( size_t iBlk = 0; iBlk < occupancyResolution; iBlk++ ) {
                      for ( size_t jBlk = 0; jBlk < occupancyResolution; jBlk++ ) {
                        tmpImage.setValue( 0, j * occupancyResolution + jBlk, i * occupancyResolution + iBlk,
                                           tmpImage.getValue( 0, j * occupancyResolution + jBlk,
                                                              neighborIdx[3] * occupancyResolution ) );
                        tmpImage.setValue( 1, j * occupancyResolution + jBlk, i * occupancyResolution + iBlk,
                                           tmpImage.getValue( 1, j * occupancyResolution + jBlk,
                                                              neighborIdx[3] * occupancyResolution ) );
                        tmpImage.setValue( 2, j * occupancyResolution + jBlk, i * occupancyResolution + iBlk,
                                           tmpImage.getValue( 2, j * occupancyResolution + jBlk,
                                                              neighborIdx[3] * occupancyResolution ) );
                      }
                    }
                  } else {
                    printf( "This condition should never occur, report an error" );
                    return false;
                  }
                }
              }
            }

            // perform downsampling
            const std::string rgbFileNameTmp = addVideoFormat( fileName + "_tmp.rgb", patch_width, patch_height );
            const std::string yuvFileNameTmp = addVideoFormat( fileName + "_tmp.yuv", patch_width, patch_height, true );

            if ( !tmpImage.write( rgbFileNameTmp, nbyte ) ) { return false; }
            if ( colorSpaceConversionPath.empty() ) {
              tmpImage.read420( yuvFileNameTmp, width, height, nbyte, true, upsamplingFilter );
            } else {
              std::stringstream cmd;
              cmd << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
                  << rgbFileNameTmp << "\" -p OutputFile=\"" << yuvFileNameTmp << "\" -p SourceWidth=" << patch_width
                  << " -p SourceHeight=" << patch_height << " -p NumberOfFrames=" << video.getFrameCount();

              std::cout << cmd.str() << '\n';
              if ( pcc::system( cmd.str().c_str() ) ) {
                std::cout << "Error: can't run system command!" << std::endl;
                return false;
              }
              tmpImage.read420( yuvFileNameTmp, patch_width, patch_height, nbyte );
            }

            // removing intermediate files
            if ( !keepIntermediateFiles ) {
              removeFile( rgbFileNameTmp );
              removeFile( yuvFileNameTmp );
            }
            // substitute the pixels in the output image for compression
            for ( size_t i = 0; i < patch_height; i++ ) {
              for ( size_t j = 0; j < patch_width; j++ ) {
                if ( context.getBlockToPatch()[( ( i + patch_top ) / occupancyResolution ) *
                                                   ( width / occupancyResolution ) +
                                               ( j + patch_left ) / occupancyResolution] == patchIdx ) {
                  // do nothing
                  for ( size_t cc = 0; cc < 3; cc++ ) {
                    destImage.setValue( cc, j + patch_left, i + patch_top, tmpImage.getValue( cc, j, i ) );
                  }
                }
              }
            }
          }
        }
        // saving the video
        video420.write420( srcYuvFileName, nbyte );
      } else {
        if ( colorSpaceConversionPath.empty() ) {
          printf( "Encoder convert : write420 with conversion \n" );
          // if ( keepIntermediateFiles ) { video.write( srcRgbFileName, nbyte ); }
          if ( !video.write420( srcYuvFileName, nbyte, true, downsamplingFilter ) ) { return false; }
        } else {
          printf( "Encoder convert : write + hdrtools conversion \n" );
          if ( !video.write( srcRgbFileName, nbyte ) ) { return false; }
          std::stringstream cmd;
          cmd << colorSpaceConversionPath << " -f " << colorSpaceConversionConfig << " -p SourceFile=\""
              << srcRgbFileName << "\""
              << " -p OutputFile=\"" << srcYuvFileName << "\""
              << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
          std::cout << cmd.str() << '\n';
          if ( pcc::system( cmd.str().c_str() ) ) {
            std::cout << "Error: can't run system command!" << std::endl;
            return false;
          }
        }
      }
    }

    std::stringstream cmd;
    if ( use444CodecIo ) {
      cmd << encoderPath << " -c " << encoderConfig << " -i " << srcYuvFileName << " --InputBitDepth=" << depth
          << " --InternalBitDepth=" << depth << " --InternalBitDepthC=" << depth << " --InputChromaFormat=" << format
          << " --FrameRate=30 "
          << " --FrameSkip=0 "
          << " --SourceWidth=" << width << " --SourceHeight=" << height << " --ConformanceWindowMode=1 "
          << " --FramesToBeEncoded=" << frameCount << " --BitstreamFile=" << binFileName
          << " --ReconFile=" << recYuvFileName << " --QP=" << qp << " --InputColourSpaceConvert=RGBtoGBR";
      if ( use3dmv ) {
#if MULTIPLE_ATTRIBUTES
        cmd << " --BlockToPatchFile=" << blockToPatchFileName << " --OccupancyMapFile=" << occupancyMapFileName
            << " --PatchInfoFile=" << patchInfoFileName;
#else
        cmd << " --UsePccMotionEstimation=1 --BlockToPatchFile=" << blockToPatchFileName
            << " --OccupancyMapFile=" << occupancyMapFileName << " --PatchInfoFile=" << patchInfoFileName;
#endif
      }
    } else {
      cmd << encoderPath << " -c " << encoderConfig << " -i " << srcYuvFileName << " --InputBitDepth=" << depth
          << " --InputChromaFormat=" << format << " --FrameRate=30 "
          << " --FrameSkip=0 "
          << " --SourceWidth=" << width << " --SourceHeight=" << height << " --ConformanceWindowMode=1 "
          << " --FramesToBeEncoded=" << frameCount << " --BitstreamFile=" << binFileName
          << " --ReconFile=" << recYuvFileName << " --QP=" << qp;

      if ( internalBitDepth != 0 ) {
        cmd << " --InternalBitDepth=" << internalBitDepth << " --InternalBitDepthC=" << internalBitDepth;
      }

      cmd << " --OutputBitDepth=" << depth;
      cmd << " --OutputBitDepthC=" << depth;
      if ( use3dmv ) {
#if MULTIPLE_ATTRIBUTES
        cmd << " --BlockToPatchFile=" << blockToPatchFileName << " --OccupancyMapFile=" << occupancyMapFileName
            << " --PatchInfoFile=" << patchInfoFileName;
#else
        cmd << " --UsePccMotionEstimation=1 --BlockToPatchFile=" << blockToPatchFileName
            << " --OccupancyMapFile=" << occupancyMapFileName << " --PatchInfoFile=" << patchInfoFileName;
#endif
      }
    }
    std::cout << cmd.str() << std::endl;
    if ( pcc::system( cmd.str().c_str() ) ) {
      std::cout << "Error: can't run system command!" << std::endl;
      return false;
    }

    std::ifstream file( binFileName, std::ios::binary | std::ios::ate );
    if ( !file.good() ) { return false; }
    const uint64_t fileSize = file.tellg();
    bitstream.resize( (size_t)fileSize );
    file.clear();
    file.seekg( 0 );
    file.read( reinterpret_cast<char*>( bitstream.buffer() ), fileSize );
    file.close();

    if ( yuvVideo ) {
      if ( use444CodecIo ) {
        video.read( recYuvFileName, width, height, frameCount, nbyte );
      } else {
        video.read420( recYuvFileName, width, height, frameCount, nbyte );
      }
    } else {
      if ( colorSpaceConversionPath.empty() ) {
        video.read420( recYuvFileName, width, height, frameCount, nbyte, true, upsamplingFilter );
        if ( !keepIntermediateFiles ) { video.write( recRgbFileName, nbyte ); }
      } else {
        std::stringstream cmd;
        cmd << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
            << recYuvFileName << "\""
            << " -p OutputFile=\"" << recRgbFileName << "\""
            << " -p SourceWidth=" << width << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd.str() << '\n';
        if ( int ret = pcc::system( cmd.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return ret;
        }
        video.read( recRgbFileName, width, height, frameCount, nbyte );
      }
    }
    if ( !keepIntermediateFiles ) {
      removeFile( binFileName );
      removeFile( srcYuvFileName );
      removeFile( srcRgbFileName );
      removeFile( recYuvFileName );
      removeFile( recRgbFileName );
    }
    return true;
  }

 private:
};

};  // namespace pcc

#endif /* PCCVideoEncoder_h */
