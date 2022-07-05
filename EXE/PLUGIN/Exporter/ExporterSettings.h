#include <stdio.h>
#include <iostream>
#include <codecvt>
#include <xlocbuf>
#include <thread>

#include "SPTypes.h"
#include "PrSDKTypes.h"
#include "SPBasic.h"
#include "PrSDKMemoryManagerSuite.h"
#include "PrSDKExportParamSuite.h"
#include "PrSDKExportInfoSuite.h"
#include "PrSDKExportFileSuite.h"
#include "PrSDKSequenceRenderSuite.h"
#include "PrSDKExportProgressSuite.h"
#include "PrSDKExporterUtilitySuite.h"
#include "PrSDKErrorSuite.h"
#include "PrSDKPPixSuite.h"
#include "PrSDKTimeSuite.h"
#include "PrSDKPixelFormat.h"
#include "PrSDKQuality.h"
#include "cisWriter.h"

#pragma once

void treatRendering(csSDK_uint32 inVideoRenderIDm, void* inCallbackRef, 
        PrTime inTime, PPixHand inRenderedFrame, SequenceRender_GetFrameReturnRec *inGetFrameReturn);

class ExporterSettings
{

public:

    ExporterSettings(SPBasicSuite *mSpBasic);
    ~ExporterSettings();

    // -- suite handler
    bool loadSuites();
    void releaseSuites();

    // -- parameters
    void setDefaultParams();

    // -- output settings
    bool setOutputSettings(exQueryOutputSettingsRec *outputSettingsP);

    // -- summary
    void setSummary(exParamSummaryRec *summaryRecP);

    // -- setters
    void setPluginId(csSDK_uint32 mPluginId) {this->pluginId = mPluginId;}
    
    // -- export
    bool exportToCis(exDoExportRec *exportInfoP);

    PrSDKExportParamSuite *getExportParamSuite() {return exportParamSuite;}

    // -- callback return from async rendering
    void handleRenderingFrame(PrTime inTime, PPixHand inRenderedFrame, SequenceRender_GetFrameReturnRec *inGetFrameReturn);

    static void removeRowPadding(char* srcFrame, char* dstFrame, csSDK_int32 rowBytes, csSDK_int32 pixelSize, csSDK_int32 widthL, csSDK_int32 heightL);
    static void reverseImage(char* srcFrame, char* dstFrame, csSDK_int32 rowBytes, csSDK_int32 pixelSize, csSDK_int32 widthL, csSDK_int32 heightL);

    // -- constant param
    csSDK_int32 widthF;
    csSDK_int32 heightF;
    csSDK_int64 frameRateF;

    // -- file to save
    CisWriter* cis;

private:


    // -- suite adobe
    SPBasicSuite *spBasic;
	PrSDKExportParamSuite *exportParamSuite;
    PrSDKExportInfoSuite *exportInfoSuite;
    PrSDKExportFileSuite *exportFileSuite;
    PrSDKExporterUtilitySuite *exportUtilitySuite;
    PrSDKSequenceRenderSuite *sequenceRenderSuite;
    PrSDKExportProgressSuite *exportProgressSuite;
    PrSDKPPixSuite* ppixSuite;
    PrSDKTimeSuite *timeSuite;

    // -- pkugin ID
    csSDK_uint32 pluginId;

    // -- video render ID
    csSDK_uint32 videoRenderId;

    // -- parameters for export
    std::string filePath;
    bool isLutApplied;

    // -- param to ckeck where we are
    PrTime startTimeF;
    PrTime endTimeF;
    PrTime totalDuration;
};