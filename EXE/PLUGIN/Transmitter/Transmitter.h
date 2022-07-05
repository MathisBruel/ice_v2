#include <PrSDKErrorSuite.h>
#include "PrSDKTransmit.h"

#include "TransmitPlugin.h"

#pragma once

// Declare plug-in entry point with C linkage
extern "C" {
    DllExport PREMPLUGENTRY xTransmitEntry(csSDK_int32 inInterfaceVersion, prBool inLoadModule, piSuitesPtr piSuites, tmModule* outModule);
}

tmResult transmitStartup(tmStdParms* ioStdParms, tmPluginInfo* outPluginInfo);
tmResult transmitShutdown(tmStdParms* ioStdParms);
tmResult transmitQueryVideoMode(const tmStdParms* inStdParms, const tmInstance* inInstance, csSDK_int32 inQueryIterationIndex, tmVideoMode* outVideoMode);
tmResult transmitSetupDialog(tmStdParms* ioStdParms, prParentWnd inParent);
tmResult transmitCreateInstance(const tmStdParms* inStdParms, tmInstance* ioInstance);
tmResult transmitDisposeInstance(const tmStdParms* inStdParms, tmInstance* ioInstance);
tmResult transmitActivateDeactivate(const tmStdParms* inStdParms, const tmInstance* inInstance, PrActivationEvent inActivationEvent, prBool inAudioActive, prBool inVideoActive);
tmResult transmitPushVideo(const tmStdParms* inStdParms, const tmInstance* inInstance, const tmPushVideo* inPushVideo);