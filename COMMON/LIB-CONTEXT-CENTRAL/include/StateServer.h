#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

#pragma once

#include "Poco/Timestamp.h"

class StateServer
{

public:
    StateServer(int id, bool ims);
    ~StateServer();

    void setAnswering(bool answering, std::string comments = "");
    void addDeviceInError(std::string device);

    int getId() {return id;}
    long getLastTimeChecked() {return lastTimeCheck;}
    bool isAnswering() {return answering;}
    std::vector<std::string> getDevicesInError() {return devicesInErrors;}

    std::string toXmlString();

private:

    // -- id auditorium or id_server
    int id;
    // -- is ims or ice
    bool ims;

    long lastTimeCheck;
    std::vector<std::string> devicesInErrors;
    bool answering;
    std::string comments;
};