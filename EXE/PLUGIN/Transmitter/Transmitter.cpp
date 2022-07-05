#include "Transmitter.h"

DllExport PREMPLUGENTRY xTransmitEntry(csSDK_int32 inInterfaceVersion, prBool inLoadModule, piSuitesPtr piSuites, tmModule* outModule)
{
	if (inLoadModule) {
		// Register all required functions (not supported functions should be set to 0).
		outModule->Startup = transmitStartup;
		outModule->Shutdown = transmitShutdown;
		outModule->QueryAudioMode = 0;
		outModule->QueryVideoMode = transmitQueryVideoMode;
		outModule->SetupDialog = transmitSetupDialog;
		outModule->NeedsReset = 0;
		outModule->CreateInstance = transmitCreateInstance;
		outModule->DisposeInstance = transmitDisposeInstance;
		outModule->ActivateDeactivate = transmitActivateDeactivate;
		outModule->StartPlaybackClock = 0;
		outModule->StopPlaybackClock = 0;
		outModule->PushVideo = transmitPushVideo;
	} else {
		// The module is being unloaded. Nothing to do here in our implementation.
	}
	return tmResult_Success;
}

tmResult transmitStartup(tmStdParms* ioStdParms, tmPluginInfo* outPluginInfo)
{
    ioStdParms->ioPrivatePluginData = new TransmitPlugin(ioStdParms, outPluginInfo);
	if (ioStdParms->ioPrivatePluginData != 0) {
		return tmResult_Success;
	} else {
		return tmResult_ErrorMemory;
	}
}
tmResult transmitShutdown(tmStdParms* ioStdParms)
{
    TransmitPlugin* p = reinterpret_cast<TransmitPlugin*>(ioStdParms->ioPrivatePluginData);
	if (p != nullptr) {
		delete p;
	}
	return tmResult_Success;
}

tmResult transmitQueryVideoMode(const tmStdParms* inStdParms, const tmInstance* inInstance, csSDK_int32 inQueryIterationIndex, tmVideoMode* outVideoMode)
{
    return reinterpret_cast<TransmitInstance*>(inInstance->ioPrivateInstanceData)
	    ->QueryVideoMode(inStdParms, inInstance, inQueryIterationIndex, outVideoMode);
}
tmResult transmitSetupDialog(tmStdParms* ioStdParms, prParentWnd inParent)
{
    return reinterpret_cast<TransmitPlugin*>(ioStdParms->ioPrivatePluginData)->SetupDialog(ioStdParms, inParent);
}

tmResult transmitCreateInstance(const tmStdParms* inStdParms, tmInstance* ioInstance)
{
    ioInstance->ioPrivateInstanceData =
	    reinterpret_cast<TransmitPlugin*>(inStdParms->ioPrivatePluginData)->CreateInstance(inStdParms, ioInstance);
	return ioInstance->ioPrivateInstanceData != 0 ? tmResult_Success : tmResult_ErrorUnknown;
}
tmResult transmitDisposeInstance(const tmStdParms* inStdParms, tmInstance* ioInstance)
{
    reinterpret_cast<TransmitPlugin*>(inStdParms->ioPrivatePluginData)->DisposeInstance(inStdParms, ioInstance);
	ioInstance->ioPrivateInstanceData = 0;
	return tmResult_Success;
}
tmResult transmitActivateDeactivate(const tmStdParms* inStdParms, const tmInstance* inInstance, PrActivationEvent inActivationEvent, prBool inAudioActive, prBool inVideoActive)
{
    return reinterpret_cast<TransmitInstance*>(inInstance->ioPrivateInstanceData)
	    ->ActivateDeactivate(inStdParms, inInstance, inActivationEvent, inAudioActive, inVideoActive);
}
tmResult transmitPushVideo(const tmStdParms* inStdParms, const tmInstance* inInstance, const tmPushVideo* inPushVideo)
{
    return reinterpret_cast<TransmitInstance*>(inInstance->ioPrivateInstanceData)->PushVideo(inStdParms, inInstance, inPushVideo);
}