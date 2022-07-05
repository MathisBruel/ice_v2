#include <stdlib.h>
#include <iostream>

#pragma once
#include "ImsMessageHandler.h"
#include "Converter.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/AutoPtr.h"

class ChristieHandler : public ImsMessageHandler
{

public:

    enum StatusCode : int32_t
    {
        PLAYBACK_TIMECODE = 759,
        PLAYBACK_STATE = 760,
        PLAYBACK_CPLID = 762,
        PLAYBACK_CPLNAME = 763
    };

    ChristieHandler();
    ~ChristieHandler();

    ImsRequest* createLoginRequest(std::string username, std::string password);
    ImsRequest* createLogoutRequest();

    ImsRequest* createGetStatusItemRequest(uint32_t statusItemCode);
    ImsRequest* createGetStatusItemsRequest(std::string category);
    ImsRequest* createGetListCplsRequest();
    ImsRequest* createGetCplRequest(std::string uuid);

    void handleResponse(uint8_t* buffer, uint16_t size, std::shared_ptr<ImsResponse> response);
    void tryDecodeBuffer(std::shared_ptr<ImsResponse> response);
    void parseLogin(ByteArray* arrayDecode, int startIndex, std::shared_ptr<ImsResponse> response);
    void parseGetStatus(ByteArray* arrayDecode, int startIndex, std::shared_ptr<ImsResponse> response);
    void parseListCpls(ByteArray* arrayDecode, int startIndex, std::shared_ptr<ImsResponse> response);
    void parseGetCpl(ByteArray* arrayDecode, int startIndex, std::shared_ptr<ImsResponse> response);

private:

    enum CommandCode
    {
        Undefined = 0,
        Set = 1,
        Get = 2,
        Update = 6
    };

    enum ObjectID
    {
        LOGIN = 1002,
        LOGOUT = 1003,
        GET_STATUS_ITEM = 2012,
        GET_STATUS_ITEMS = 2015,
        LIST_CONTENT = 41,
        GET_CPL = 58
    };

    enum DataType
    {
        ASCII_STRING = 12,
        UUID = 14,
        UTF8_STRING = 13,
        XML = 15,

        UINT8 = 1,
        INT8 = 2,
        BOOLEAN = 9,

        UINT16 = 3,
        INT16 = 4,

        UINT32 = 5,
        INT32 = 6,
        FLOAT = 10,

        UINT64 = 7,
        INT64 = 8,
        DOUBLE = 11
    };

    ByteArray* createHeader(uint8_t commandCode, uint16_t objectId);
    ByteArray* createElement(uint32_t key, DataType type, char* data, int sizeData);
    std::vector<char> berEncoder(int valueToEncode);
    uint32_t berDecoder(int* lengthBer, uint8_t* data);

    uint8_t bufferTemporary[524288];
    int sizeBuffer;
};