#include "TransmitSocket.h"

TransmitSocket::TransmitSocket()
{
    connected = false;
	lastFailedConnect = 0;
	connectionTimeoutDurationSeconds = 0;
}

TransmitSocket::~TransmitSocket()
{
    if (connected) {
		close();
	}
}

void TransmitSocket::setConfig(std::string host, uint16_t portNb, int version)
{
    this->hostName = host;
	this->portNr = portNb;
    this->version = version;
}

bool TransmitSocket::sendVideoFrame(char* buffer, int width, int height, int size)
{
    const int HEADER_SIZE = 2;
	uint16_t crc = 0xFFFF;
	uint32_t headerArray[HEADER_SIZE];
	if (!connected) {
		if (isInTimeOut()) {
			return false;
		}
		connected = connectSocket();
		if (!connected) {
			setTimeOut();
			return false;
		}
	}

	if (first) {
		sendHeader();
		first = false;
	}

	// generate header

	// fill the file header with data
	headerArray[0] = 0xB17EB1BE; // ID
	headerArray[1] = size + sizeof(crc); // Size of the image data + crc

	// change to (Network)Big Endian
	for (int i = 0; i < HEADER_SIZE; i++) {
		headerArray[i] = htonl(headerArray[i]);
	}

	// Send header
	bool sendResult = sendMessage((char*)headerArray, sizeof(headerArray));

	if (sendResult) {
		int lineSize = 4 * width;
		char* sendBuffer = (char*)buffer + lineSize * (height - 1);

		while (sendBuffer >= buffer) {
			// send data in blocks of width(line by line) from bottom to top as Premiere Pro draws from bottom to top and the
			// player assumes top to bottom.
			sendResult = sendMessage(sendBuffer, lineSize);
			if (!sendResult) {
				// Failed to send
				break;
			}
			crc = Crc16(sendBuffer, lineSize, crc);
			sendBuffer -= lineSize;
		}
		// send crc
		if (sendResult) {
			uint16_t nCrc = htons(crc);
			sendResult = sendMessage((char*)&nCrc, sizeof(nCrc));
		}
	}

	if (!sendResult) {
		close();
	} else {
		resetTimeOut();
	}
	return sendResult;
}

bool TransmitSocket::connectSocket()
{
    if (WSAStartup(MAKEWORD(2, 2), &WsaDat) != 0) {
		std::cout << "Winsock error - Winsock initialization failed\r\n";
		WSACleanup();
		return false;
	}

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET) {
		std::cout << "Winsock error - Socket creation Failed!\r\n";
		WSACleanup();
		return false;
	}

	struct hostent* host;
#pragma warning(push)
#pragma warning(disable : 4996)
	if ((host = gethostbyname(hostName.c_str())) == NULL) {
		int errorCode = WSAGetLastError();
		std::cout << "Failed to resolve hostname. Error code: " << errorCode << "\r\n";
		WSACleanup();
		return false;
	}
#pragma warning(pop)
	SOCKADDR_IN sockAddr;
	sockAddr.sin_port = htons(portNr);
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	u_long iMode = 0;
	ioctlsocket(Socket, FIONBIO, &iMode); // set socket to blocking mode

	// width * hight * bytes per pixel + some extra space. For performance this should be larger than 1 frame
	DWORD sendBufferSize = (720 * 576 * 4 + 50);
	setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufferSize, sizeof(sendBufferSize)); // Set send buffer size

	DWORD val = 1;
	setsockopt(Socket, SOL_SOCKET, TCP_NODELAY, (char*)&val, sizeof(val)); // turn off delay

	DWORD sendTimeout = 1000;
	setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&sendTimeout, sizeof(sendTimeout)); // set timeout to 1 second

	if (connect(Socket, (SOCKADDR*)(&sockAddr), sizeof(sockAddr)) != 0) {
		std::cout << "Failed to establish connection with server\r\n";
		WSACleanup();
		return false;
	}

	first = true;
	return true;
}
bool TransmitSocket::close()
{
    if (closesocket(Socket) != 0) {
		return false;
	}
	connected = false;
	setTimeOut();
	return true;
}

bool TransmitSocket::isConnected()
{
    return connected;
}

bool TransmitSocket::sendHeader()
{
    // generate header
	uint32_t* headerArray;
	int HEADER_SIZE = 4;

	HEADER_SIZE = 4;
	headerArray = new uint32_t[4];

	// fill the file header with data
	headerArray[0] = 0x4E10B1BE;
	headerArray[1] = 8;
	headerArray[2] = version; 			// -- version
	headerArray[3] = 1; 			// -- format

	// change to (Network)Big Endian
	for (int i = 0; i < HEADER_SIZE; i++) {
		headerArray[i] = htonl(headerArray[i]);
	}
	return sendMessage((char*)headerArray, HEADER_SIZE*4);
}
bool TransmitSocket::sendMessage(char* message, int length)
{
    send(Socket, message, length, 0);
	int nError = WSAGetLastError();

	if (nError != 0) {
		std::cout << "Sending error code: " << nError << "\r\n";
		return false;
	}
	return true;
}

bool TransmitSocket::isInTimeOut()
{
    double durationSeconds = difftime(time(NULL), lastFailedConnect);
	return durationSeconds < connectionTimeoutDurationSeconds;
}
uint16_t TransmitSocket::Crc16(char* data_p, int length, uint16_t crcin)
{
    unsigned char x;
	uint16_t crc = crcin;

	while (length--) {
		x = crc >> 8 ^ *data_p++;
		x ^= x >> 4;
		crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
	}
	return crc;
}
void TransmitSocket::setTimeOut()
{
    lastFailedConnect = time(NULL);
	if (connectionTimeoutDurationSeconds <= 0) {
		connectionTimeoutDurationSeconds = CONNECTION_TIMEOUT_INITIAL_SECONDS;
	} else {
		connectionTimeoutDurationSeconds =
		    min(connectionTimeoutDurationSeconds * CONNECTION_TIMEOUT_FACTOR, CONNECTION_TIMEOUT_MAX_SECONDS);
	}
}
void TransmitSocket::resetTimeOut()
{
    lastFailedConnect = 0;
	connectionTimeoutDurationSeconds = 0;
}