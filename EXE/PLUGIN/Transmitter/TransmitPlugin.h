#include "PrSDKTransmit.h"
#include "PrSDKTypes.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <windows.h>

#include "TransmitSocket.h"
#include "TransmitInstance.h"
#include "PrSDKPPixSuite.h"

#define ID_OK 1
#define ID_Cancel 2
#define ID_Remote 3
#define ID_Remote_HostName 4
#define ID_Remote_Port 5
#define ID_Local 6
#define ID_Local_Port 7
#define ID_Version 8

#pragma once

#define PLUGIN_DISPLAY_NAME L"Preview Transmitter"

class TransmitPlugin
{

public:

    TransmitPlugin(tmStdParms* ioStdParms, tmPluginInfo* outPluginInfo);
	~TransmitPlugin();

	tmResult SetupDialog(tmStdParms* ioStdParms, prParentWnd inParentWnd);

	void* CreateInstance(const tmStdParms* inStdParms, tmInstance* inInstance);
	void DisposeInstance(const tmStdParms* inStdParms, tmInstance* inInstance);

    void saveConfig(bool remoteEnable, std::string ipRemote, int portRemote, bool localEnable, int portLocal, int version);

    SPBasicSuite* getBasicSuite() {return SPBasic;}
    PrSDKPPixSuite* getPPixSuite() {return PPixSuite;}

    void sendToSockets();

private:

    void loadConfig();
    void saveConfig();

    void createDialog(prParentWnd inParentWnd);
    void createControls();

    HWND parentWindow;
    HWND hwnd;

    // -- suites 
    SPBasicSuite* SPBasic;
	PrSDKPPixSuite* PPixSuite;

    // -- configuration
    TransmitInstance::Configuration* conf;

    TransmitSocket remoteSocket;
    TransmitSocket localSocket;
};