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

#ifndef PCCVideoDecoder_h
#define PCCVideoDecoder_h

#include "PCCCommon.h"
#include "PCCVideoBitstream.h"
#include "PCCSystem.h"
#include "PCCVideo.h"

#include "PCCVideoDecoder.h"

#include "PCCContext.h"
#include "PCCFrameContext.h"
#include "PCCPatch.h"

#include "PCCHevcParser.h"

namespace pcc {

class PCCBitstream;

#if MULTIPLE_ATTRIBUTES
class PCCMultiViewDecoder {
 public:
  PCCMultiViewDecoder();
  ~PCCMultiViewDecoder();

  template <typename T>
  bool decompress( PCCVideo<T, 3>& video0,
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
                   const size_t       frameCount,
                   PCCVideoBitstream& bitstream,
                   const std::string& decoderPath,
                   PCCContext&        contexts,
                   size_t             bitDepth,
                   const bool         keepIntermediateFiles             = false,
                   const bool         use444CodecIo                     = false,
                   const bool         patchColorSubsampling             = false,
                   const std::string& inverseColorSpaceConversionConfig = "",
                   const std::string& colorSpaceConversionPath          = "",
                   const size_t       upsamplingFilter                  = 0 ) {
    const std::string type        = bitstream.getExtension();
    const std::string fileName    = path + type;
    const std::string binFileName = fileName + ".bin";
    size_t            width = 0, height = 0;
    // PCCHevcParser     hevcParser;
    // hevcParser.getVideoSize( bitstream.vector(), width, height );
    width = 5120;
    height = 4800;
    const std::string yuvRecFileName = 
        addVideoFormat( fileName + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string yuvRecFileName0 =
        addVideoFormat( fileName + "_view0" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string yuvRecFileName1 =
        addVideoFormat( fileName + "_view1" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string yuvRecFileName2 =
        addVideoFormat( fileName + "_view2" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string yuvRecFileName3 =
        addVideoFormat( fileName + "_view3" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string yuvRecFileName4 =
        addVideoFormat( fileName + "_view4" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string yuvRecFileName5 =
        addVideoFormat( fileName + "_view5" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string yuvRecFileName6 =
        addVideoFormat( fileName + "_view6" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string yuvRecFileName7 =
        addVideoFormat( fileName + "_view7" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string yuvRecFileName8 =
        addVideoFormat( fileName + "_view8" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string yuvRecFileName9 =
        addVideoFormat( fileName + "_view9" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string yuvRecFileName10 =
        addVideoFormat( fileName + "_view10" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string yuvRecFileName11 =
        addVideoFormat( fileName + "_view11" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
#if NUMBER_VIEWS == 13
    const std::string yuvRecFileName12 =
        addVideoFormat( fileName + "_view12" + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width, height,
                        !use444CodecIo, bitDepth == 10 ? "10" : "8" );
#endif

    const std::string rgbRecFileName0 =
        addVideoFormat( fileName + "_view0" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10  ? "10" : "8" );
    const std::string rgbRecFileName1 =
        addVideoFormat( fileName + "_view1" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10  ? "10" : "8" );
    const std::string rgbRecFileName2 =
        addVideoFormat( fileName + "_view2" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10  ? "10" : "8" );
    const std::string rgbRecFileName3 =
        addVideoFormat( fileName + "_view3" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10  ? "10" : "8" );
    const std::string rgbRecFileName4 =
        addVideoFormat( fileName + "_view4" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10  ? "10" : "8" );
    const std::string rgbRecFileName5 =
        addVideoFormat( fileName + "_view5" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10  ? "10" : "8" );
    const std::string rgbRecFileName6 =
        addVideoFormat( fileName + "_view6" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10  ? "10" : "8" );
    const std::string rgbRecFileName7 =
        addVideoFormat( fileName + "_view7" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10  ? "10" : "8" );
    const std::string rgbRecFileName8 =
        addVideoFormat( fileName + "_view8" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10  ? "10" : "8" );
    const std::string rgbRecFileName9 =
        addVideoFormat( fileName + "_view9" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10  ? "10" : "8" );
    const std::string rgbRecFileName10 = 
        addVideoFormat( fileName + "_view10" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string rgbRecFileName11 = 
      addVideoFormat( fileName + "_view11" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10 ? "10" : "8" );
#if NUMBER_VIEWS == 13
    const std::string rgbRecFileName12 = 
      addVideoFormat( fileName + "_view12" + "_rec" + ".rgb", width, height, !use444CodecIo, bitDepth == 10 ? "10" : "8" );
#endif

    std::ofstream     file( binFileName, std::ios::binary );
    const std::string format = use444CodecIo ? "444" : "420";
    if ( !file.good() ) { return false; }
    file.write( reinterpret_cast<char*>( bitstream.buffer() ), bitstream.size() );
    file.close();
    std::stringstream cmd;

    // to be modified
    if ( use444CodecIo ) {
      cmd << decoderPath << " --OutputColourSpaceConvert=GBRtoRGB" << " --BitstreamFile=" << binFileName << " --ReconFile=" << yuvRecFileName;
    } else {
      cmd << decoderPath << " --BitstreamFile=" << binFileName << " --ReconFile=" << yuvRecFileName;
      // if bitDepth == 8 ensure output bitdepth as 8bit. This is to cater for case if 10bit encoding was used for lossy
      // cases.
      if ( bitDepth == 8 ) { cmd << " --OutputBitDepth=8 --OutputBitDepthC=8"; }
    }

    std::cout << cmd.str() << '\n';
    if ( pcc::system( cmd.str().c_str() ) ) {
      std::cout << "Error: can't run system command!" << std::endl;
      return false;
    }

    if ( inverseColorSpaceConversionConfig.empty() || use444CodecIo ) {
      if ( use444CodecIo ) {
        if ( !video0.read( yuvRecFileName0, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video1.read( yuvRecFileName1, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video2.read( yuvRecFileName2, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video3.read( yuvRecFileName3, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video4.read( yuvRecFileName4, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video5.read( yuvRecFileName5, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video6.read( yuvRecFileName6, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video7.read( yuvRecFileName7, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video8.read( yuvRecFileName8, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video9.read( yuvRecFileName9, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video10.read( yuvRecFileName10, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video11.read( yuvRecFileName11, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
#if NUMBER_VIEWS == 13
        if ( !video12.read( yuvRecFileName12, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
#endif
      } else {
        if ( !video0.read420( yuvRecFileName0, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video1.read420( yuvRecFileName1, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video2.read420( yuvRecFileName2, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video3.read420( yuvRecFileName3, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video4.read420( yuvRecFileName4, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video5.read420( yuvRecFileName5, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video6.read420( yuvRecFileName6, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video7.read420( yuvRecFileName7, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video8.read420( yuvRecFileName8, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video9.read420( yuvRecFileName9, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video10.read420( yuvRecFileName10, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video11.read420( yuvRecFileName11, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
#if NUMBER_VIEWS == 13
        if ( !video12.read420( yuvRecFileName12, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
#endif
      }
    } else {
      if ( colorSpaceConversionPath.empty() ) {
        video0.read420( yuvRecFileName0, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
        video1.read420( yuvRecFileName1, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
        video2.read420( yuvRecFileName2, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
        video3.read420( yuvRecFileName3, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
        video4.read420( yuvRecFileName4, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
        video5.read420( yuvRecFileName5, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
        video6.read420( yuvRecFileName6, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
        video7.read420( yuvRecFileName7, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
        video8.read420( yuvRecFileName8, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
        video9.read420( yuvRecFileName9, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
        video10.read420( yuvRecFileName10, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
        video11.read420( yuvRecFileName11, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
#if NUMBER_VIEWS == 13
        video12.read420( yuvRecFileName12, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
#endif
        if ( !keepIntermediateFiles ) { video0.write( rgbRecFileName0, bitDepth == 8 ? 1 : 2 ); }
        if ( !keepIntermediateFiles ) { video1.write( rgbRecFileName1, bitDepth == 8 ? 1 : 2 ); }
        if ( !keepIntermediateFiles ) { video2.write( rgbRecFileName2, bitDepth == 8 ? 1 : 2 ); }
        if ( !keepIntermediateFiles ) { video3.write( rgbRecFileName3, bitDepth == 8 ? 1 : 2 ); }
        if ( !keepIntermediateFiles ) { video4.write( rgbRecFileName4, bitDepth == 8 ? 1 : 2 ); }
        if ( !keepIntermediateFiles ) { video5.write( rgbRecFileName5, bitDepth == 8 ? 1 : 2 ); }
        if ( !keepIntermediateFiles ) { video6.write( rgbRecFileName6, bitDepth == 8 ? 1 : 2 ); }
        if ( !keepIntermediateFiles ) { video7.write( rgbRecFileName7, bitDepth == 8 ? 1 : 2 ); }
        if ( !keepIntermediateFiles ) { video8.write( rgbRecFileName8, bitDepth == 8 ? 1 : 2 ); }
        if ( !keepIntermediateFiles ) { video9.write( rgbRecFileName9, bitDepth == 8 ? 1 : 2 ); }
        if ( !keepIntermediateFiles ) { video10.write( rgbRecFileName10, bitDepth == 8 ? 1 : 2 ); }
        if ( !keepIntermediateFiles ) { video11.write( rgbRecFileName11, bitDepth == 8 ? 1 : 2 ); }
#if NUMBER_VIEWS == 13
        if ( !keepIntermediateFiles ) { video12.write( rgbRecFileName12, bitDepth == 8 ? 1 : 2 ); }
#endif
      } else {
        std::stringstream cmd0;
        cmd0 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << yuvRecFileName0 << "\" -p OutputFile=\"" << rgbRecFileName0 << "\" -p SourceWidth=" << width
             << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd0.str() << '\n';
        if ( pcc::system( cmd0.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd1;
        cmd1 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << yuvRecFileName1 << "\" -p OutputFile=\"" << rgbRecFileName1 << "\" -p SourceWidth=" << width
             << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd1.str() << '\n';
        if ( pcc::system( cmd1.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd2;
        cmd2 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << yuvRecFileName2 << "\" -p OutputFile=\"" << rgbRecFileName2 << "\" -p SourceWidth=" << width
             << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd2.str() << '\n';
        if ( pcc::system( cmd2.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd3;
        cmd3 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << yuvRecFileName3 << "\" -p OutputFile=\"" << rgbRecFileName3 << "\" -p SourceWidth=" << width
             << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd3.str() << '\n';
        if ( pcc::system( cmd3.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd4;
        cmd4 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << yuvRecFileName4 << "\" -p OutputFile=\"" << rgbRecFileName4 << "\" -p SourceWidth=" << width
             << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd4.str() << '\n';
        if ( pcc::system( cmd4.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd5;
        cmd5 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << yuvRecFileName5 << "\" -p OutputFile=\"" << rgbRecFileName5 << "\" -p SourceWidth=" << width
             << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd5.str() << '\n';
        if ( pcc::system( cmd5.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd6;
        cmd6 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << yuvRecFileName6 << "\" -p OutputFile=\"" << rgbRecFileName6 << "\" -p SourceWidth=" << width
             << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd6.str() << '\n';
        if ( pcc::system( cmd6.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd7;
        cmd7 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << yuvRecFileName7 << "\" -p OutputFile=\"" << rgbRecFileName7 << "\" -p SourceWidth=" << width
             << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd7.str() << '\n';
        if ( pcc::system( cmd7.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd8;
        cmd8 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << yuvRecFileName8 << "\" -p OutputFile=\"" << rgbRecFileName8 << "\" -p SourceWidth=" << width
             << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd8.str() << '\n';
        if ( pcc::system( cmd8.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd9;
        cmd9 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
             << yuvRecFileName9 << "\" -p OutputFile=\"" << rgbRecFileName9 << "\" -p SourceWidth=" << width
             << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd9.str() << '\n';
        if ( pcc::system( cmd9.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd10;
        cmd10 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
              << yuvRecFileName10 << "\" -p OutputFile=\"" << rgbRecFileName10 << "\" -p SourceWidth=" << width
              << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd10.str() << '\n';
        if ( pcc::system( cmd10.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

        std::stringstream cmd11;
        cmd11 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
              << yuvRecFileName11 << "\" -p OutputFile=\"" << rgbRecFileName11 << "\" -p SourceWidth=" << width
              << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd11.str() << '\n';
        if ( pcc::system( cmd11.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }

#if NUMBER_VIEWS == 13
        std::stringstream cmd12;
        cmd12 << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
              << yuvRecFileName12 << "\" -p OutputFile=\"" << rgbRecFileName12 << "\" -p SourceWidth=" << width
              << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
        std::cout << cmd12.str() << '\n';
        if ( pcc::system( cmd12.str().c_str() ) ) {
          std::cout << "Error: can't run system command!" << std::endl;
          return false;
        }
#endif

        if ( !video0.read( rgbRecFileName0, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video1.read( rgbRecFileName1, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video2.read( rgbRecFileName2, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video3.read( rgbRecFileName3, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video4.read( rgbRecFileName4, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video5.read( rgbRecFileName5, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video6.read( rgbRecFileName6, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video7.read( rgbRecFileName7, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video8.read( rgbRecFileName8, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video9.read( rgbRecFileName9, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video10.read( rgbRecFileName10, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        if ( !video11.read( rgbRecFileName11, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
#if NUMBER_VIEWS == 13
        if ( !video12.read( rgbRecFileName12, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
#endif
      }
    }
    if ( !keepIntermediateFiles ) {
      removeFile( binFileName );
      removeFile( yuvRecFileName0 );
      removeFile( rgbRecFileName0 );
      removeFile( yuvRecFileName1 );
      removeFile( rgbRecFileName1 );
      removeFile( yuvRecFileName2 );
      removeFile( rgbRecFileName2 );
      removeFile( yuvRecFileName3 );
      removeFile( rgbRecFileName3 );
      removeFile( yuvRecFileName4 );
      removeFile( rgbRecFileName4 );
      removeFile( yuvRecFileName5 );
      removeFile( rgbRecFileName5 );
      removeFile( yuvRecFileName6 );
      removeFile( rgbRecFileName6 );
      removeFile( yuvRecFileName7 );
      removeFile( rgbRecFileName7 );
      removeFile( yuvRecFileName8 );
      removeFile( rgbRecFileName8 );
      removeFile( yuvRecFileName9 );
      removeFile( rgbRecFileName9 );
      removeFile( yuvRecFileName10 );
      removeFile( rgbRecFileName10 );
      removeFile( yuvRecFileName11 );
      removeFile( rgbRecFileName11 );
#if NUMBER_VIEWS == 13
      removeFile( yuvRecFileName12 );
      removeFile( rgbRecFileName12 );
#endif
    }
    return true;
  }

 private:
};
#endif

class PCCVideoDecoder {
 public:
  PCCVideoDecoder();
  ~PCCVideoDecoder();

  template <typename T>
  bool decompress( PCCVideo<T, 3>&    video,
                   const std::string& path,
                   const size_t       frameCount,
                   PCCVideoBitstream& bitstream,
                   const std::string& decoderPath,
                   PCCContext&        contexts,
                   size_t             bitDepth,
                   const bool         keepIntermediateFiles             = false,
                   const bool         use444CodecIo                     = false,
                   const bool         patchColorSubsampling             = false,
                   const std::string& inverseColorSpaceConversionConfig = "",
                   const std::string& colorSpaceConversionPath          = "",
                   const size_t       upsamplingFilter                  = 0 ) {
    const std::string type        = bitstream.getExtension();
    const std::string fileName    = path + type;
    const std::string binFileName = fileName + ".bin";
    size_t            width = 0, height = 0;
    PCCHevcParser     hevcParser;
    hevcParser.getVideoSize( bitstream.vector(), width, height );
    const std::string yuvRecFileName = addVideoFormat( fileName + "_rec" + ( use444CodecIo ? ".rgb" : ".yuv" ), width,
                                                       height, !use444CodecIo, bitDepth == 10 ? "10" : "8" );
    const std::string rgbRecFileName =
        addVideoFormat( fileName + "_rec.rgb", width, height, true, bitDepth == 10 ? "10" : "8" );
    std::ofstream     file( binFileName, std::ios::binary );
    const std::string format = use444CodecIo ? "444" : "420";
    if ( !file.good() ) { return false; }
    file.write( reinterpret_cast<char*>( bitstream.buffer() ), bitstream.size() );
    file.close();
    std::stringstream cmd;

    if ( use444CodecIo ) {
      cmd << decoderPath << " --OutputColourSpaceConvert=GBRtoRGB"
          << " --BitstreamFile=" << binFileName << " --ReconFile=" << yuvRecFileName;
    } else {
      cmd << decoderPath << " --BitstreamFile=" << binFileName << " --ReconFile=" << yuvRecFileName;
      // if bitDepth == 8 ensure output bitdepth as 8bit. This is to cater for case if 10bit encoding was used for lossy
      // cases.
      if ( bitDepth == 8 ) { cmd << " --OutputBitDepth=8 --OutputBitDepthC=8"; }
    }

    std::cout << cmd.str() << '\n';
    if ( pcc::system( cmd.str().c_str() ) ) {
      std::cout << "Error: can't run system command!" << std::endl;
      return false;
    }
    if ( inverseColorSpaceConversionConfig.empty() || use444CodecIo ) {
      if ( use444CodecIo ) {
        if ( !video.read( yuvRecFileName, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
      } else {
        if ( !video.read420( yuvRecFileName, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
      }
    } else {
      if ( patchColorSubsampling ) {
        PCCVideo<T, 3> video420;
        if ( !video420.read420( yuvRecFileName, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        // allocate the output
        video.resize( frameCount );
        // perform color-upsampling based on patch information
        for ( size_t frNum = 0; frNum < video.getFrameCount(); frNum++ ) {
          // context variable, contains the patch information
          auto& context = contexts[frNum / 2];
          // full resolution image (already filled by previous dilation
          auto& refImage = video420.getFrame( frNum );
          // image that will contain the per-patch chroma sub-sampled image
          auto& destImage = video.getFrame( frNum );
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
                  searchIndex = j;
                  while ( searchIndex >= 0 ) {
                    if ( context.getBlockToPatch()[( i + patch_top / occupancyResolution ) *
                                                       ( width / occupancyResolution ) +
                                                   searchIndex + patch_left / occupancyResolution] == patchIdx ) {
                      neighborIdx[0]      = searchIndex;
                      neighborDistance[0] = j - searchIndex;
                      searchIndex         = 0;
                    }
                    searchIndex--;
                  }
                  // looking for the neighboring block to the right of the current block
                  searchIndex = j;
                  while ( searchIndex < patch_width / occupancyResolution ) {
                    if ( context.getBlockToPatch()[( i + patch_top / occupancyResolution ) *
                                                       ( width / occupancyResolution ) +
                                                   searchIndex + patch_left / occupancyResolution] == patchIdx ) {
                      neighborIdx[1]      = searchIndex;
                      neighborDistance[1] = searchIndex - j;
                      searchIndex         = patch_width / occupancyResolution;
                    }
                    searchIndex++;
                  }
                  // looking for the neighboring block above the current block
                  searchIndex = i;
                  while ( searchIndex >= 0 ) {
                    if ( context.getBlockToPatch()[( searchIndex + patch_top / occupancyResolution ) *
                                                       ( width / occupancyResolution ) +
                                                   j + patch_left / occupancyResolution] == patchIdx ) {
                      neighborIdx[2]      = searchIndex;
                      neighborDistance[2] = i - searchIndex;
                      searchIndex         = 0;
                    }
                    searchIndex--;
                  }
                  // looking for the neighboring block below the current block
                  searchIndex = i;
                  while ( searchIndex < patch_height / occupancyResolution ) {
                    if ( context.getBlockToPatch()[( searchIndex + patch_top / occupancyResolution ) *
                                                       ( width / occupancyResolution ) +
                                                   j + patch_left / occupancyResolution] == patchIdx ) {
                      neighborIdx[3]      = searchIndex;
                      neighborDistance[3] = searchIndex - i;
                      searchIndex         = patch_height / occupancyResolution;
                    }
                    searchIndex++;
                  }
                  // check if the candidate was found
                  assert( *( std::max )( neighborIdx.begin(), neighborIdx.end() ) > 0 );
                  // now fill in the block with the edge value coming from the nearest neighbor
                  direction = ( std::min_element )( neighborDistance.begin(), neighborDistance.end() ) -
                              neighborDistance.begin();
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
            const std::string rgbRecFileNamePatch = addVideoFormat( fileName + "_tmp.rgb", patch_width, patch_height );
            const std::string yuvRecFileNamePatch =
                addVideoFormat( fileName + "_tmp.yuv", patch_width, patch_height, true );
            if ( !tmpImage.write420( yuvRecFileNamePatch, bitDepth == 8 ? 1 : 2 ) ) { return false; }
            if ( colorSpaceConversionPath.empty() ) {
              tmpImage.read420( yuvRecFileNamePatch, width, height, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
              if ( !keepIntermediateFiles ) { tmpImage.write( rgbRecFileNamePatch, bitDepth == 8 ? 1 : 2 ); }
            } else {
              std::stringstream cmd;
              cmd << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
                  << yuvRecFileNamePatch << "\" -p OutputFile=\"" << rgbRecFileNamePatch
                  << "\" -p SourceWidth=" << patch_width << " -p SourceHeight=" << patch_height
                  << " -p NumberOfFrames=1";
              std::cout << cmd.str() << '\n';
              if ( pcc::system( cmd.str().c_str() ) ) {
                std::cout << "Error: can't run system command!" << std::endl;
                return false;
              }
              tmpImage.read( rgbRecFileNamePatch, patch_width, patch_height, bitDepth == 8 ? 1 : 2 );
            }
            // removing intermediate files
            if ( !keepIntermediateFiles ) {
              removeFile( rgbRecFileNamePatch );
              removeFile( yuvRecFileNamePatch );
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
      } else {
        if ( colorSpaceConversionPath.empty() ) {
          video.read420( yuvRecFileName, width, height, frameCount, bitDepth == 8 ? 1 : 2, true, upsamplingFilter );
          if ( !keepIntermediateFiles ) { video.write( rgbRecFileName, bitDepth == 8 ? 1 : 2 ); }
        } else {
          std::stringstream cmd;
          cmd << colorSpaceConversionPath << " -f " << inverseColorSpaceConversionConfig << " -p SourceFile=\""
              << yuvRecFileName << "\" -p OutputFile=\"" << rgbRecFileName << "\" -p SourceWidth=" << width
              << " -p SourceHeight=" << height << " -p NumberOfFrames=" << frameCount;
          std::cout << cmd.str() << '\n';
          if ( pcc::system( cmd.str().c_str() ) ) {
            std::cout << "Error: can't run system command!" << std::endl;
            return false;
          }
          if ( !video.read( rgbRecFileName, width, height, frameCount, bitDepth == 8 ? 1 : 2 ) ) { return false; }
        }
      }
    }
    if ( !keepIntermediateFiles ) {
      removeFile( binFileName );
      removeFile( yuvRecFileName );
      removeFile( rgbRecFileName );
    }
    return true;
  }

 private:
};

};  // namespace pcc

#endif /* PCCVideoDecoder_h */
