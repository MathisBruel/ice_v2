#include <winsock2.h>
#include <stdint.h>
#include <time.h>
#include <string>
#include <iostream>

#pragma once

class TransmitSocket
{

public:

    TransmitSocket();
	~TransmitSocket();

	void setConfig(std::string hostName, uint16_t port, int version);

	bool sendVideoFrame(char* buffer, int width, int height, int size);

	bool connectSocket();
	bool close();

	bool isConnected();

private:

    bool sendHeader();
	bool sendMessage(char* message, int length);

	bool isInTimeOut();
	uint16_t Crc16(char* data_p, int length, uint16_t crcin);
	void setTimeOut();
	void resetTimeOut();

    // -- sending configuration
    const double CONNECTION_TIMEOUT_MAX_SECONDS = 60;
	const double CONNECTION_TIMEOUT_FACTOR = 2;
	const double CONNECTION_TIMEOUT_INITIAL_SECONDS = 1;
	WSADATA WsaDat;
	SOCKET Socket;

    // -- configuration connection
	std::string hostName;
	uint16_t portNr;
	int version;

    // -- status connection
	bool first;
	bool connected;
	time_t lastFailedConnect;
	double connectionTimeoutDurationSeconds;

};