#include "TransmitInstance.h"

TransmitInstance::TransmitInstance(const tmInstance* inInstance, SPBasicSuite* SPBasic, PrSDKPPixSuite* PPixSuite, Configuration* conf)
{
    this->SPBasic = SPBasic;
    this->PPixSuite = PPixSuite;
    this->conf = conf;
	this->remote = nullptr;
	this->local = nullptr;
}

TransmitInstance::~TransmitInstance()
{

}

void TransmitInstance::setSockets(TransmitSocket* remoteS, TransmitSocket* localS)
{
    this->remote = remoteS;
    this->local = localS;
}

tmResult TransmitInstance::QueryVideoMode(const tmStdParms* inStdParms, const tmInstance* inInstance, csSDK_int32 inQueryIterationIndex, tmVideoMode* outVideoMode)
{
    memset(outVideoMode, 0, sizeof(tmVideoMode)); // Clear the structure to avoid bogus field contents.
	outVideoMode->outWidth = 720;
	outVideoMode->outHeight = 576;
	outVideoMode->outPARNum = 0;
	outVideoMode->outPARDen = 0;
	outVideoMode->outFieldType = prFieldsNone;
	outVideoMode->outPixelFormat = PrPixelFormat_BGRA_4444_8u;
	outVideoMode->outLatency = inInstance->inVideoFrameRate * 0; // Ask for 0 frames preroll
	return tmResult_Success;
}
tmResult TransmitInstance::ActivateDeactivate(const tmStdParms* inStdParms, const tmInstance* inInstance, PrActivationEvent inActivationEvent, prBool inAudioActive, prBool inVideoActive)
{
    if (!inVideoActive) {
		if (remote->isConnected()) {
			remote->close();
		}
        if (local->isConnected()) {
			local->close();
		}
	}
    else {
        if (!conf->remoteEnable && remote->isConnected()) {
            remote->close();
        }
        if (!conf->localEnable && local->isConnected()) {
            local->close();
        }
    }
    return tmResult_Success;
}
tmResult TransmitInstance::PushVideo(const tmStdParms* inStdParms, const tmInstance* inInstance, const tmPushVideo* inPushVideo)
{
    // Send the video frames to the hardware.  We also log frame info to the debug console.
	prRect frameBounds;
	csSDK_uint32 parNum = 0;
	csSDK_uint32 parDen = 0;
	PrPixelFormat pixelFormat = PrPixelFormat_Invalid;
	tmResult returnValue = tmResult_Success;

	PPixSuite->GetBounds(inPushVideo->inFrames[0].inFrame, &frameBounds);
	PPixSuite->GetPixelAspectRatio(inPushVideo->inFrames[0].inFrame, &parNum, &parDen);
	PPixSuite->GetPixelFormat(inPushVideo->inFrames[0].inFrame, &pixelFormat);
	int width = abs(frameBounds.right - frameBounds.left);
	int height = abs(frameBounds.top - frameBounds.bottom);

	//
	// This is where a transmit plug-in could queue up the frame to an actual hardware device.
	//

	char* pixelBuffer;
	if (PPixSuite->GetPixels(inPushVideo->inFrames[0].inFrame, PrPPixBufferAccess_ReadOnly, &pixelBuffer) ==
	    suiteError_NoError) {
		
        if (!sendOverSocket(pixelBuffer, width, height, width * height * 4)) {
            returnValue = tmResult_ErrorUnknown;
		}
	}

	// Dispose of the PPix(els) when done!
	for (csSDK_size_t i = 0; i < inPushVideo->inFrameCount; ++i) {
		PPixSuite->Dispose(inPushVideo->inFrames[i].inFrame);
	}

	return returnValue;
}

bool TransmitInstance::sendOverSocket(char* buffer, int width, int height, long paddedsize)
{
    bool success = true;

    if (conf->remoteEnable) {
        if (!remote->sendVideoFrame(buffer, width, height, paddedsize)) {
            success = false;
        }
    }

    if (success && conf->localEnable) {
        if (!local->sendVideoFrame(buffer, width, height, paddedsize)) {
            success = false;
        }
    }

    return success;
}