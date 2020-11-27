/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2017, ITU/ISO/IEC
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
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
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

#pragma once

#ifndef __SEIWRITE__
#define __SEIWRITE__

#include "SyntaxElementWriter.h"
#include "TLibCommon/SEI.h"

class TComBitIf;

//! \ingroup TLibEncoder
//! \{
class SEIWriter:public SyntaxElementWriter
{
public:
  SEIWriter() {};
  virtual ~SEIWriter() {};

  Void writeSEImessages(TComBitIf& bs, const SEIMessages &seiList, const TComSPS *sps, Bool isNested);

protected:
  Void xWriteSEIBufferingPeriod                   (const SEIBufferingPeriod& sei, const TComSPS *sps);
  Void xWriteSEIPictureTiming                     (const SEIPictureTiming& sei, const TComSPS *sps);
  Void xWriteSEIPanScanRect                       (const SEIPanScanRect& sei);
  Void xWriteSEIFillerPayload                     (const SEIFillerPayload& sei);
  Void xWriteSEIUserDataRegistered                (const SEIUserDataRegistered& sei);
  Void xWriteSEIUserDataUnregistered              (const SEIUserDataUnregistered &sei);
  Void xWriteSEIRecoveryPoint                     (const SEIRecoveryPoint& sei);
  Void xWriteSEISceneInfo                         (const SEISceneInfo& sei);
  Void xWriteSEIPictureSnapshot                   (const SEIPictureSnapshot& sei);
  Void xWriteSEIProgressiveRefinementSegmentStart (const SEIProgressiveRefinementSegmentStart& sei);
  Void xWriteSEIProgressiveRefinementSegmentEnd   (const SEIProgressiveRefinementSegmentEnd& sei);
  Void xWriteSEIFilmGrainCharacteristics          (const SEIFilmGrainCharacteristics& sei);
  Void xWriteSEIPostFilterHint                    (const SEIPostFilterHint& sei, const TComSPS *sps);
  Void xWriteSEIToneMappingInfo                   (const SEIToneMappingInfo& sei);
  Void xWriteSEIFramePacking                      (const SEIFramePacking& sei);
  Void xWriteSEIDisplayOrientation                (const SEIDisplayOrientation &sei);
  Void xWriteSEIGreenMetadataInfo                 (const SEIGreenMetadataInfo &sei);
  Void xWriteSEISOPDescription                    (const SEISOPDescription& sei);
  Void xWriteSEIActiveParameterSets               (const SEIActiveParameterSets& sei);
  Void xWriteSEIDecodingUnitInfo                  (const SEIDecodingUnitInfo& sei, const TComSPS *sps);
  Void xWriteSEITemporalLevel0Index               (const SEITemporalLevel0Index &sei);
  Void xWriteSEIDecodedPictureHash                (const SEIDecodedPictureHash& sei);
  Void xWriteSEIScalableNesting                   (TComBitIf& bs, const SEIScalableNesting& sei, const TComSPS *sps);
  Void xWriteSEIRegionRefreshInfo                 (const SEIRegionRefreshInfo &sei);
  Void xWriteSEINoDisplay                         (const SEINoDisplay &sei);
  Void xWriteSEITimeCode                          (const SEITimeCode& sei);
  Void xWriteSEIMasteringDisplayColourVolume      (const SEIMasteringDisplayColourVolume& sei);
  Void xWriteSEISegmentedRectFramePacking         (const SEISegmentedRectFramePacking& sei);
  Void xWriteSEITempMotionConstrainedTileSets     (const SEITempMotionConstrainedTileSets& sei);
  Void xWriteSEIChromaResamplingFilterHint        (const SEIChromaResamplingFilterHint& sei);
  Void xWriteSEIKneeFunctionInfo                  (const SEIKneeFunctionInfo &sei);
  Void xWriteSEIColourRemappingInfo               (const SEIColourRemappingInfo& sei);
  Void xWriteSEIDeinterlaceFieldIdentification    (const SEIDeinterlaceFieldIdentification& sei);
  Void xWriteSEIContentLightLevelInfo             (const SEIContentLightLevelInfo& sei);
  Void xWriteSEIDependentRAPIndication            (const SEIDependentRAPIndication& sei);
  Void xWriteSEICodedRegionCompletion             (const SEICodedRegionCompletion& sei);
  Void xWriteSEIAlternativeTransferCharacteristics(const SEIAlternativeTransferCharacteristics& sei);
  Void xWriteSEIAmbientViewingEnvironment         (const SEIAmbientViewingEnvironment& sei);
#if NH_MV
  Void xWriteSEILayersNotPresent              ( const SEILayersNotPresent& sei);
  Void xWriteSEIInterLayerConstrainedTileSets ( const SEIInterLayerConstrainedTileSets& sei);
#if NH_MV_SEI_TBD
  Void xWriteSEIBspNesting                    ( const SEIBspNesting& sei);
  Void xWriteSEIBspInitialArrivalTime         ( const SEIBspInitialArrivalTime& sei);
#endif
  Void xWriteSEISubBitstreamProperty          ( const SEISubBitstreamProperty& sei);
  Void xWriteSEIAlphaChannelInfo              ( const SEIAlphaChannelInfo& sei);
  Void xWriteSEIOverlayInfo                   ( const SEIOverlayInfo& sei);
  Void xWriteSEITemporalMvPredictionConstraints ( const SEITemporalMvPredictionConstraints& sei);
#if NH_MV_SEI_TBD
  Void xWriteSEIFrameFieldInfo                ( const SEIFrameFieldInfo& sei);
#endif
  Void xWriteSEIThreeDimensionalReferenceDisplaysInfo ( const SEIThreeDimensionalReferenceDisplaysInfo& sei);
  Void xWriteSEIDepthRepInfoElement           ( double f);
  Void xWriteSEIDepthRepresentationInfo       ( const SEIDepthRepresentationInfo& sei);
  Void xWriteSEIMultiviewSceneInfo            ( const SEIMultiviewSceneInfo& sei);
  Void xWriteSEIMultiviewAcquisitionInfo      ( const SEIMultiviewAcquisitionInfo& sei);
  Void xWriteSEIMultiviewViewPosition         ( const SEIMultiviewViewPosition& sei);
#if NH_3D
  Void xWriteSEIAlternativeDepthInfo          ( const SEIAlternativeDepthInfo& sei);
#endif
#endif

  Void xWriteSEIpayloadData(TComBitIf& bs, const SEI& sei, const TComSPS *sps);
  Void xWriteByteAlign();
};

//! \}

#endif