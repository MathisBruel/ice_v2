#include "PrSDKTransmit.h"
#include "PrSDKTypes.h"
#include "PrSDKPPixSuite.h"

#include "TransmitSocket.h"

#pragma once

class TransmitInstance
{
public:

    struct Configuration
    {
        bool remoteEnable;
        std::string ipRemote;
        int portRemote;
        bool localEnable;
        int portLocal;
        int version;
    };

	TransmitInstance(const tmInstance* inInstance, SPBasicSuite* SPBasic, PrSDKPPixSuite* PPixSuite, Configuration* conf);
	~TransmitInstance();

    void setSockets(TransmitSocket* remote, TransmitSocket* local);

	tmResult QueryVideoMode(const tmStdParms* inStdParms, const tmInstance* inInstance, csSDK_int32 inQueryIterationIndex, tmVideoMode* outVideoMode);
	tmResult ActivateDeactivate(const tmStdParms* inStdParms, const tmInstance* inInstance, PrActivationEvent inActivationEvent, prBool inAudioActive, prBool inVideoActive);
	tmResult PushVideo(const tmStdParms* inStdParms, const tmInstance* inInstance, const tmPushVideo* inPushVideo);

private:
	
    SPBasicSuite* SPBasic;
    PrSDKPPixSuite* PPixSuite;

    TransmitSocket* remote;
    TransmitSocket* local;

    Configuration* conf;
    
	bool sendOverSocket(char* buffer, int width, int height, long paddedsize);
};