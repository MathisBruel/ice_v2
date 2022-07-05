#include "TransmitPlugin.h"

TransmitPlugin::TransmitPlugin(tmStdParms* ioStdParms, tmPluginInfo* outPluginInfo)
{
    size_t length = wcslen(PLUGIN_DISPLAY_NAME);
	wcscpy_s(reinterpret_cast<wchar_t*>(outPluginInfo->outDisplayName), length + 1, PLUGIN_DISPLAY_NAME);

	// The transmitter handles video only
	outPluginInfo->outAudioAvailable = kPrFalse;
	outPluginInfo->outAudioDefaultEnabled = kPrFalse;
	outPluginInfo->outClockAvailable = kPrFalse;
	outPluginInfo->outVideoAvailable = kPrTrue;
	outPluginInfo->outVideoDefaultEnabled = kPrTrue;
	outPluginInfo->outHasSetup = kPrTrue;

	// Acquire any suites needed!
	SPBasic = ioStdParms->piSuites->utilFuncs->getSPBasicSuite();
	SPBasic->AcquireSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion, (const void**)&PPixSuite);

    // -- default values
    conf = new TransmitInstance::Configuration();
    conf->remoteEnable = false;
    conf->ipRemote = "0.0.0.0";
    conf->portRemote = 0;
    conf->localEnable = false;
    conf->portLocal = 0;
    conf->version = 1;

    loadConfig();
    remoteSocket.setConfig(conf->ipRemote, conf->portRemote, conf->version);
    localSocket.setConfig("127.0.0.1", conf->portLocal, conf->version);
}
TransmitPlugin::~TransmitPlugin()
{
	SPBasic->ReleaseSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion);
}

tmResult TransmitPlugin::SetupDialog(tmStdParms* ioStdParms, prParentWnd inParentWnd)
{
    createDialog(inParentWnd);
    return tmResult_Success;
}

void* TransmitPlugin::CreateInstance(const tmStdParms* inStdParms, tmInstance* inInstance)
{
    TransmitInstance* instance = new TransmitInstance(inInstance, SPBasic, PPixSuite, conf);
    instance->setSockets(&remoteSocket, &localSocket);
    return instance;
}
void TransmitPlugin::DisposeInstance(const tmStdParms* inStdParms, tmInstance* inInstance)
{
    delete reinterpret_cast<TransmitInstance*>(inInstance->ioPrivateInstanceData);
}

void TransmitPlugin::saveConfig(bool remoteEnable, std::string ipRemote, int portRemote, bool localEnable, int portLocal, int version)
{
    bool hasChanged = false;
    bool needRemoteChange = false;
    bool needLocalChange = false;

    if (conf->remoteEnable != remoteEnable) {
        conf->remoteEnable = remoteEnable;
        hasChanged = true;
    }
    if (conf->ipRemote != ipRemote) {
        conf->ipRemote = ipRemote;
        hasChanged = true;
        needRemoteChange = true;
    }
    if (conf->portRemote != portRemote) {
        conf->portRemote = portRemote;
        hasChanged = true;
        needRemoteChange = true;
    }

    if (conf->localEnable != localEnable) {
        conf->localEnable = localEnable;
        hasChanged = true;
    }
    if (conf->portLocal != portLocal) {
        conf->portLocal = portLocal;
        hasChanged = true;
        needLocalChange = true;
    }

    if (conf->version != version) {
        conf->version = version;
        hasChanged = true;
        needRemoteChange = true;
        needLocalChange = true;
    }

    // -- save config if needed
    if (hasChanged) {
        saveConfig();
    }

    // -- disconnect and reset config info for socket
    if (needRemoteChange) {
        if (remoteSocket.isConnected()) {
            remoteSocket.close();
        }
        remoteSocket.setConfig(ipRemote, portRemote, version);
    }
    if (needLocalChange) {
        if (localSocket.isConnected()) {
            localSocket.close();
        }
        localSocket.setConfig("127.0.0.1", portLocal, version);
    }

    // -- disconnect if disabled
    if (!remoteEnable && remoteSocket.isConnected()) {
        remoteSocket.close();
    }
    if (!localEnable && localSocket.isConnected()) {
        localSocket.close();
    }
}

void TransmitPlugin::loadConfig()
{
    std::string filename = "C:/ICE-EFFECT/previewPlugin.ini";

    std::ifstream file(filename, std::ios_base::in);
    if (file.fail()) {
        saveConfig();
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    std::string line;

    while(std::getline(buffer, line, '\n')) {
        
        int posSpace = line.find(" ", 0);
        std::string key = line.substr(0, posSpace);
        std::string value = line.substr(posSpace+1);

        if (key.find("remoteEnable") != std::string::npos) {
            conf->remoteEnable = value == "true" ? true : false;
        }
        else if (key.find("ipRemote") != std::string::npos) {
            conf->ipRemote = value;
        }
        else if (key.find("portRemote") != std::string::npos) {
            conf->portRemote = std::stoi(value);
        }
        else if (key.find("localEnable") != std::string::npos) {
            conf->localEnable = value == "true" ? true : false;
        }
        else if (key.find("portLocal") != std::string::npos) {
            conf->portLocal = std::stoi(value);
        }
        else if (key.find("version") != std::string::npos) {
            conf->version = std::stoi(value);
        }
    }
}
void TransmitPlugin::saveConfig()
{
    std::string filename = "C:/ICE-EFFECT/previewPlugin.ini";

    std::ofstream file;
    file.open(filename, std::ios::out | std::ios::trunc);
    file << "remoteEnable " << (conf->remoteEnable ? "true" : "false") << "\n";
    file << "ipRemote " << conf->ipRemote << "\n";
    file << "portRemote " << conf->portRemote << "\n";
    file << "localEnable " << (conf->localEnable ? "true" : "false") << "\n";
    file << "portLocal " << conf->portLocal << "\n";
    file << "version " << conf->version << "\n";
    file.close();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
TransmitPlugin* reference;
void TransmitPlugin::createDialog(prParentWnd inParentWnd)
{
    parentWindow = inParentWnd;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	// Register the window class.
	LPCSTR CLASS_NAME = "Window";
	WNDCLASS wc = {};
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	// Create the window.
	hwnd = CreateWindowEx(0, CLASS_NAME, "Transmitter Settings", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 300, 215, parentWindow, NULL,  hInstance,  NULL);

	if (hwnd == NULL) {
		return;
	}

    reference = this;
	createControls();
	EnableWindow(parentWindow, FALSE);
	ShowWindow(hwnd, SW_SHOWNORMAL);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	EnableWindow(parentWindow, TRUE);
	SetFocus(parentWindow);
}
void TransmitPlugin::createControls()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // -- remove enable
	CreateWindowEx(WS_EX_WINDOWEDGE, "BUTTON", "Remote playout", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 10, 240, 20, hwnd, (HMENU)ID_Remote, hInstance, NULL);
	if (conf->remoteEnable) {
		CheckDlgButton(hwnd, ID_Remote, BST_CHECKED);
	}

    // -- ip remote
    CreateWindow("STATIC", "Remote Hostname:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 10, 35, 150, 20, hwnd, NULL, hInstance, NULL);
	CreateWindow("EDIT", conf->ipRemote.c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN, 180, 35, 150, 20, hwnd, (HMENU)ID_Remote_HostName, hInstance, NULL);
    
    // -- port remote
    CreateWindow("STATIC", "Remote Port:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 10, 60, 150, 20, hwnd, NULL, hInstance, NULL);
	CreateWindow("EDIT", std::to_string(conf->portRemote).c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN, 180, 60, 150, 20, hwnd, (HMENU)ID_Remote_Port, hInstance, NULL);

    // -- local enable
	CreateWindowEx(WS_EX_WINDOWEDGE, "BUTTON", "Local playout", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 90, 240, 20, hwnd, (HMENU)ID_Local, hInstance, NULL);
	if (conf->localEnable) {
		CheckDlgButton(hwnd, ID_Local, BST_CHECKED);
	}

	 // -- port local
    CreateWindow("STATIC", "Local Port:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 10, 115, 150, 20, hwnd, NULL, hInstance, NULL);
	CreateWindow("EDIT", std::to_string(conf->portLocal).c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN, 180, 115, 150, 20, hwnd, (HMENU)ID_Local_Port, hInstance, NULL);

    // -- Version
    CreateWindow("STATIC", "Version:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 10, 145, 150, 20, hwnd, NULL, hInstance, NULL);
	CreateWindow("EDIT", std::to_string(conf->version).c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN, 180, 145, 150, 20, hwnd, (HMENU)ID_Version, hInstance, NULL);

    // -- validation items
	CreateWindowEx(WS_EX_WINDOWEDGE, "BUTTON", "Cancel", WS_VISIBLE | WS_CHILD, 10, 170, 80, 20, hwnd, (HMENU)ID_Cancel, hInstance, NULL);
	CreateWindowEx(WS_EX_WINDOWEDGE, "BUTTON", "OK", WS_VISIBLE | WS_CHILD, 100, 170, 80, 20, hwnd, (HMENU)ID_OK, hInstance, NULL);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    char temp[254];
    std::string remoteHost;
    int remotePort;
    int localPort;
    int version;

	switch (uMsg) {
		case WM_CREATE:
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_OK:
					
                    GetDlgItemText(hwnd, ID_Remote_HostName, temp, 254);
                    remoteHost = temp;
                    GetDlgItemText(hwnd, ID_Remote_Port, temp, 254);
                    remotePort = std::atoi(temp);
                    GetDlgItemText(hwnd, ID_Local_Port, temp, 254);
                    localPort = std::atoi(temp);
                    GetDlgItemText(hwnd, ID_Version, temp, 254);
                    version = std::atoi(temp);
                    reference->saveConfig(IsDlgButtonChecked(hwnd, ID_Remote) == BST_CHECKED, remoteHost, remotePort, 
                                IsDlgButtonChecked(hwnd, ID_Local) == BST_CHECKED, localPort, version);
                    DestroyWindow(hwnd);
					break;

				case ID_Cancel:
					DestroyWindow(hwnd);
					break;

				default:
					res = DefWindowProc(hwnd, uMsg, wParam, lParam);
					break;
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			EndPaint(hwnd, &ps);
		} break;

		default:
			res = DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
	}
	return res;
}