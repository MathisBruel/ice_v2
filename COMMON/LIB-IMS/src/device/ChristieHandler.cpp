#include "device/ChristieHandler.h"

ChristieHandler::ChristieHandler() 
{
    sizeBuffer = 0;
}
ChristieHandler::~ChristieHandler() {}

ImsRequest* ChristieHandler::createLoginRequest(std::string username, std::string password)
{
    ByteArray* msg = createHeader(CommandCode::Set, ObjectID::LOGIN);
    int offset = 18;

    // -- create params
    ByteArray* loginParam = createElement(0, DataType::ASCII_STRING, username.data(), username.size());
    ByteArray* passwordParam = createElement(1, DataType::ASCII_STRING, password.data(), password.size());

    // -- get size of params and ber encoded
    int sizeElements = loginParam->getSize() + passwordParam->getSize();
    std::vector<char> berData = berEncoder(sizeElements);
    if (berData.size() == 1) {
        msg->setUInt8(berData.at(0), offset);
        offset += 1;
    }
    else if (berData.size() == 2) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        offset += 2;
    }
    else if (berData.size() == 3) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        msg->setUInt8(berData.at(2), offset+2);
        offset += 3;
    }
    else if (berData.size() == 4) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        msg->setUInt8(berData.at(2), offset+2);
        msg->setUInt8(berData.at(3), offset+3);
        offset += 4;
    }

    // -- resize msg in consequence
    msg->resize(18 + berData.size() + sizeElements);

    // -- add elements
    msg->set(loginParam->getData(), offset, loginParam->getSize());
    offset += loginParam->getSize();
    msg->set(passwordParam->getData(), offset, passwordParam->getSize());
    offset += passwordParam->getSize();

    delete loginParam;
    delete passwordParam;

    return new ImsRequest(msg->getData(), msg->getSize());
}
ImsRequest* ChristieHandler::createLogoutRequest()
{
    int offset = 0;
    ByteArray* msg = createHeader(CommandCode::Set, ObjectID::LOGOUT);
    offset += msg->getSize();

    std::vector<char> berData = berEncoder(0);
    if (berData.size() == 1) {
        msg->setUInt8(berData.at(0), offset);
        offset += 1;
    }
    else if (berData.size() == 2) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        offset += 2;
    }
    else if (berData.size() == 3) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        msg->setUInt8(berData.at(2), offset+2);
        offset += 3;
    }
    else if (berData.size() == 4) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        msg->setUInt8(berData.at(2), offset+2);
        msg->setUInt8(berData.at(3), offset+3);
        offset += 4;
    }

    // -- resize msg in consequence
    msg->resize(18 + berData.size());

    return new ImsRequest(msg->getData(), msg->getSize());
}

ImsRequest* ChristieHandler::createGetStatusItemRequest(uint32_t statusItemCode)
{
    ByteArray* msg = createHeader(CommandCode::Get, ObjectID::GET_STATUS_ITEM);
    int offset = 18;

    // -- create params
    ByteArray* statusParam = createElement(0, DataType::INT32, (char*)&statusItemCode, 4);

    // -- get size of params and ber encoded
    int sizeElements = statusParam->getSize();
    std::vector<char> berData = berEncoder(sizeElements);
    if (berData.size() == 1) {
        msg->setUInt8(berData.at(0), offset);
        offset += 1;
    }
    else if (berData.size() == 2) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        offset += 2;
    }
    else if (berData.size() == 3) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        msg->setUInt8(berData.at(2), offset+2);
        offset += 3;
    }
    else if (berData.size() == 4) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        msg->setUInt8(berData.at(2), offset+2);
        msg->setUInt8(berData.at(3), offset+3);
        offset += 4;
    }

    // -- resize msg in consequence
    msg->resize(18 + berData.size() + sizeElements);

    // -- add elements
    msg->set(statusParam->getData(), offset, statusParam->getSize());
    offset += statusParam->getSize();

    delete statusParam;

    return new ImsRequest(msg->getData(), msg->getSize());
}
ImsRequest* ChristieHandler::createGetStatusItemsRequest(std::string category)
{
    ByteArray* msg = createHeader(CommandCode::Get, ObjectID::GET_STATUS_ITEMS);
    int offset = 18;

    // -- create params
    ByteArray* statusParam = createElement(10000, DataType::ASCII_STRING, category.data(), category.size());

    // -- get size of params and ber encoded
    int sizeElements = statusParam->getSize();
    std::vector<char> berData = berEncoder(sizeElements);
    if (berData.size() == 1) {
        msg->setUInt8(berData.at(0), offset);
        offset += 1;
    }
    else if (berData.size() == 2) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        offset += 2;
    }
    else if (berData.size() == 3) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        msg->setUInt8(berData.at(2), offset+2);
        offset += 3;
    }
    else if (berData.size() == 4) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        msg->setUInt8(berData.at(2), offset+2);
        msg->setUInt8(berData.at(3), offset+3);
        offset += 4;
    }

    // -- resize msg in consequence
    msg->resize(18 + berData.size() + sizeElements);

    // -- add elements
    msg->set(statusParam->getData(), offset, statusParam->getSize());
    offset += statusParam->getSize();

    delete statusParam;

    return new ImsRequest(msg->getData(), msg->getSize());
}

ImsRequest* ChristieHandler::createGetListCplsRequest()
{
    ByteArray* msg = createHeader(CommandCode::Get, ObjectID::LIST_CONTENT);
    int offset = 18;

    // -- create params
    uint32 typeContent = 0x01;
    ByteArray* typeParam = createElement(1, DataType::INT32, (char*)&typeContent, 4);

    // -- get size of params and ber encoded
    int sizeElements = typeParam->getSize();
    std::vector<char> berData = berEncoder(sizeElements);
    if (berData.size() == 1) {
        msg->setUInt8(berData.at(0), offset);
        offset += 1;
    }
    else if (berData.size() == 2) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        offset += 2;
    }
    else if (berData.size() == 3) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        msg->setUInt8(berData.at(2), offset+2);
        offset += 3;
    }
    else if (berData.size() == 4) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        msg->setUInt8(berData.at(2), offset+2);
        msg->setUInt8(berData.at(3), offset+3);
        offset += 4;
    }

    // -- resize msg in consequence
    msg->resize(18 + berData.size() + sizeElements);

    // -- add elements
    msg->set(typeParam->getData(), offset, typeParam->getSize());
    offset += typeParam->getSize();

    delete typeParam;

    return new ImsRequest(msg->getData(), msg->getSize());
}
ImsRequest* ChristieHandler::createGetCplRequest(std::string uuid)
{
    ByteArray* msg = createHeader(CommandCode::Get, ObjectID::GET_CPL);
    int offset = 18;

    // -- create params
    ByteArray* uuidParam = createElement(1, DataType::ASCII_STRING, uuid.data(), uuid.size());

    // -- get size of params and ber encoded
    int sizeElements = uuidParam->getSize();
    std::vector<char> berData = berEncoder(sizeElements);
    if (berData.size() == 1) {
        msg->setUInt8(berData.at(0), offset);
        offset += 1;
    }
    else if (berData.size() == 2) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        offset += 2;
    }
    else if (berData.size() == 3) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        msg->setUInt8(berData.at(2), offset+2);
        offset += 3;
    }
    else if (berData.size() == 4) {
        msg->setUInt8(berData.at(0), offset);
        msg->setUInt8(berData.at(1), offset+1);
        msg->setUInt8(berData.at(2), offset+2);
        msg->setUInt8(berData.at(3), offset+3);
        offset += 4;
    }

    // -- resize msg in consequence
    msg->resize(18 + berData.size() + sizeElements);

    // -- add elements
    msg->set(uuidParam->getData(), offset, uuidParam->getSize());
    offset += uuidParam->getSize();

    delete uuidParam;

    return new ImsRequest(msg->getData(), msg->getSize());
}

void ChristieHandler::handleResponse(uint8_t* buffer, uint16_t size, std::shared_ptr<ImsResponse> response)
{
    response->setStatus(CommandResponse::UNKNOWN);
    std::memcpy(&bufferTemporary[sizeBuffer], buffer, size);
    sizeBuffer += size;
    tryDecodeBuffer(response);
}
void ChristieHandler::tryDecodeBuffer(std::shared_ptr<ImsResponse> response)
{
    bool endParse = false;
    while (!endParse && sizeBuffer > 0)
    {
        ByteArray* arrayDecode = new ByteArray(bufferTemporary, sizeBuffer);

        uint32_t messageStart = arrayDecode->getUInt32(0, false);
        if (messageStart == 0x7b02342b) {

            uint8_t method = arrayDecode->getUInt8(6);
            uint16_t objectId = arrayDecode->getUInt16(7, true);
            int lengthBer = 0;
            uint32_t lengthParameters = berDecoder(&lengthBer, &bufferTemporary[18]);
            uint32_t lengthMsg = 18+lengthBer+lengthParameters;

            // -- message not received entirely
            if (sizeBuffer < lengthMsg) {

                endParse = true;
                delete arrayDecode;
                continue;
            }

            // -- ignore update messages : remove message to buffer
            if (method == 6) {
                uint32_t newSizeBuffer = sizeBuffer - lengthMsg;
                uint8_t* tempCopy = new uint8_t[newSizeBuffer];
                std::memcpy(tempCopy, &bufferTemporary[lengthMsg], newSizeBuffer);
                std::memset(bufferTemporary, 0, 65536);
                std::memcpy(bufferTemporary, tempCopy, newSizeBuffer);
                sizeBuffer = newSizeBuffer;
                delete[] tempCopy;
                //Poco::Logger::get("ChristieHandler").debug("Response ignored", __FILE__, __LINE__);
            }
            else {
                uint16_t error = arrayDecode->getUInt16(9, true);
                if (error == 0) {

                    if (objectId == LOGIN) {
                        //Poco::Logger::get("ChristieHandler").debug("Parse login response", __FILE__, __LINE__);
                        parseLogin(arrayDecode, 18+lengthBer, response);
                    }
                    else if (objectId == LOGOUT) {
                        //Poco::Logger::get("ChristieHandler").debug("Parse logout response", __FILE__, __LINE__);
                    }
                    else if (objectId == GET_STATUS_ITEM) {
                        //Poco::Logger::get("ChristieHandler").debug("Parse getStatus response", __FILE__, __LINE__);
                        parseGetStatus(arrayDecode, 18+lengthBer, response);
                    }
                    else if (objectId == LIST_CONTENT) {
                        //Poco::Logger::get("ChristieHandler").debug("Parse listContent response", __FILE__, __LINE__);
                        parseListCpls(arrayDecode, 18+lengthBer, response);
                    }
                    else if (objectId == GET_CPL) {
                        //Poco::Logger::get("ChristieHandler").debug("Parse getCpl response", __FILE__, __LINE__);
                        parseGetCpl(arrayDecode, 18+lengthBer, response);
                    }
                }
                else {
                    // -- ERROR
                    Poco::Logger::get("ChristieHandler").error("Error on receive message : " + std::to_string(error), __FILE__, __LINE__);
                    response->setStatus(CommandResponse::KO);
                }

                // -- empty message from buffer
                uint32_t newSizeBuffer = sizeBuffer - lengthMsg;
                uint8_t* tempCopy = new uint8_t[newSizeBuffer];
                std::memcpy(tempCopy, &bufferTemporary[lengthMsg], newSizeBuffer);
                std::memset(bufferTemporary, 0, 65536);
                std::memcpy(bufferTemporary, tempCopy, newSizeBuffer);
                sizeBuffer = newSizeBuffer;
                delete[] tempCopy;
            }
        }
        else {
            // -- critical error : to restart
            Poco::Logger::get("ChristieHandler").error("It seems that messages are cutted : restarting buffer !");
            sizeBuffer = 0;
        }

        delete arrayDecode;
    }
}

void ChristieHandler::parseLogin(ByteArray* arrayDecode, int startIndex, std::shared_ptr<ImsResponse> response)
{
    uint32_t keyId = arrayDecode->getUInt32(startIndex, true);

    int lengthBer = 0;
    uint32_t lengthParameter = berDecoder(&lengthBer, &bufferTemporary[startIndex+4]);
    uint32_t permission = arrayDecode->getUInt32(startIndex+4+lengthBer+1, true);
    
    if (permission >= 30) {
        response->setStatus(CommandResponse::OK);
        response->setComments("User authenticated !");
        //Poco::Logger::get("ChristieHandler").debug("User authenticated !", __FILE__, __LINE__);
    }
    else {
        response->setStatus(CommandResponse::KO);
        response->setComments("Permission level of user is too low !");
        Poco::Logger::get("ChristieHandler").error("Permission level of user is too low !", __FILE__, __LINE__);
    }
}
void ChristieHandler::parseGetStatus(ByteArray* arrayDecode, int startIndex, std::shared_ptr<ImsResponse> response)
{
    uint32_t keyId = arrayDecode->getUInt32(startIndex, true);

    int lengthBer = 0;
    uint32_t lengthParameter = berDecoder(&lengthBer, &bufferTemporary[startIndex+4]);
    std::string xmlValue = std::string((char*)arrayDecode->get(startIndex+4+lengthBer+1, lengthParameter-1), lengthParameter-1);

    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    
    try {
        doc = parser.parseString(xmlValue);
    } 
    catch (std::exception &e) {
        Poco::Logger::get("ChristieHandler").error("Error while parsing response", __FILE__, __LINE__);
        Poco::Logger::get("ChristieHandler").error(e.what(), __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("Error while parsing response");
        return;
    }

    Poco::XML::NodeList* listId = doc->getElementsByTagName("id");
    if (listId->length() == 0) {
        Poco::Logger::get("ChristieHandler").error("Response is not well formated : id is missing !", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("Response is not well formated : id is missing !");
        listId->release();
        return;
    }
    Poco::XML::Node* idElement = listId->item(0);

    Poco::XML::NodeList* listValue = doc->getElementsByTagName("value");
    if (listValue->length() == 0) {
        Poco::Logger::get("ChristieHandler").error("Response is not well formated : value is missing !", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("Response is not well formated : value is missing !");
        listId->release();
        listValue->release();
        return;
    }
    Poco::XML::Node* valueElement = listValue->item(0);

    std::shared_ptr<PlayerStatus> status = response->getPlayerStatus();

    // -- create if not exist
    if (status == nullptr) {

        PlayerStatus statusLocal;
        status = std::make_shared<PlayerStatus>(statusLocal);
        response->setStatusPlayer(status);
    }

    // -- check type parameter
    int id = std::stoi(idElement->innerText());
    uint32_t value;
    switch (id) {

        case PLAYBACK_TIMECODE:
        value = std::stoi(valueElement->innerText());
        value *= 24;
        status->setEditPosition(value);
        status->setspeedRate(24, 1);
        break;

        case PLAYBACK_CPLID:
        status->setCplId(valueElement->innerText());
        break;

        case PLAYBACK_CPLNAME:
        status->setCplTitle(valueElement->innerText());
        break;

        case PLAYBACK_STATE:
        value = std::stoi(valueElement->innerText());
        if (value == 0) {status->setState(PlayerStatus::Stop);}
        else if (value == 1) {status->setState(PlayerStatus::Play);}
        else if (value == 2) {status->setState(PlayerStatus::Pause);}
        else {status->setState(PlayerStatus::UnknownState);}
        break;
    };

    listId->release();
    listValue->release();
    response->setStatus(CommandResponse::OK);
}
void ChristieHandler::parseListCpls(ByteArray* arrayDecode, int startIndex, std::shared_ptr<ImsResponse> response)
{
    uint32_t keyId = arrayDecode->getUInt32(startIndex, true);

    int lengthBer = 0;
    uint32_t lengthParameter = berDecoder(&lengthBer, &bufferTemporary[startIndex+4]);
    std::string xmlValue = std::string((char*)arrayDecode->get(startIndex+4+lengthBer+1, lengthParameter-1), lengthParameter-1);

    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    
    try {
        doc = parser.parseString(xmlValue);
    } 
    catch (std::exception &e) {
        Poco::Logger::get("ChristieHandler").error("Error while parsing response", __FILE__, __LINE__);
        Poco::Logger::get("ChristieHandler").error(e.what(), __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("Error while parsing response");
        return;
    }

    // -- get Top node
    Poco::XML::NodeList* contentList = doc->getElementsByTagName("Content");
    if (contentList->length() == 0) {
        Poco::Logger::get("ChristieHandler").error("Response is not well formated : Content is missing !", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("Response is not well formated : Content is missing !");
        contentList->release();
        return;
    }

    // -- parse list of CPL
    Poco::XML::Node* idElement = contentList->item(0);
    Poco::XML::NodeList* listCpl = idElement->childNodes();
    for (int i = 0; i < listCpl->length(); i++) {

        if (listCpl->item(i)->nodeName() == "CPL") {
            std::shared_ptr<CplInfos> infos = std::make_shared<CplInfos>(CplInfos());
            int duration = -1;

            Poco::XML::NodeList* attributesCpl = listCpl->item(i)->childNodes();
            for (int j = 0; j < attributesCpl->length(); j++) {
            
                if (attributesCpl->item(j)->nodeName() == "Id") {
                    infos->setCplId(attributesCpl->item(j)->innerText());
                }
                else if (attributesCpl->item(j)->nodeName() == "Name") {
                    infos->setCplTitle(attributesCpl->item(j)->innerText());
                }
                else if (attributesCpl->item(j)->nodeName() == "duration") {
                    duration = std::stoi(attributesCpl->item(j)->innerText());
                }
                else if (attributesCpl->item(j)->nodeName() == "FrameRate") {
                    double speedRate = std::stod(attributesCpl->item(j)->innerText());
                    infos->setSpeedRate(speedRate);
                    int nbFrame = duration*speedRate/1000;
                    infos->setCplDuration(nbFrame);
                }
                else if (attributesCpl->item(j)->nodeName() == "ContentType") {
                    infos->setCplType(CplFile::parseCplTypeFromString(attributesCpl->item(j)->innerText()));
                }
                else if (attributesCpl->item(j)->nodeName() == "is3D") {
                    int id3D = std::stoi(attributesCpl->item(j)->innerText());
                    infos->set3D(id3D == 0 ? false : true);
                }
                else if (attributesCpl->item(j)->nodeName() == "isComplete") {
                    int isPlayable = std::stoi(attributesCpl->item(j)->innerText());
                    infos->setPlayable(isPlayable == 0 ? false : true);
                }
                else if (attributesCpl->item(j)->nodeName() == "is4K") {
                    int is4k = std::stoi(attributesCpl->item(j)->innerText());
                    infos->setPictureWidth(is4k == 0 ? 2500 : 4500);
                }
            }

            response->addNewCpl(infos->getCplId(), infos);
            attributesCpl->release();
        }
    }
    listCpl->release();
    response->setStatus(CommandResponse::OK);
    response->setComments("List of cpls infos success !");
}
void ChristieHandler::parseGetCpl(ByteArray* arrayDecode, int startIndex, std::shared_ptr<ImsResponse> response)
{
    uint32_t keyId = arrayDecode->getUInt32(startIndex, true);

    int lengthBer = 0;
    uint32_t lengthParameter = berDecoder(&lengthBer, &bufferTemporary[startIndex+4]);
    std::string xmlValue = (char*)arrayDecode->get(startIndex+4+lengthBer+1, lengthParameter-1);

    response->setStatus(CommandResponse::OK);
    response->setComments("Get Cpl success !");
    response->setContentFile(xmlValue);
}

ByteArray* ChristieHandler::createHeader(uint8_t commandCode, uint16_t objectId)
{
    ByteArray* array = new ByteArray(500);

    // -- start
    array->setUInt32(0x7b02342b, 0, false);
    // -- version
    array->setUInt16(0x0200, 4, false);
    // -- command Code
    array->setUInt8(commandCode, 6);
    // -- object ID
    array->setUInt16(objectId, 7, true);
    // -- error / dest / src
    array->setUInt32(0x00000000, 9, false);
    // -- security / CRC / Sequence
    array->setUInt32(0x00000000, 13, false);
    // -- reserved
    array->setUInt8(0x00, 17);

    return array;
}
ByteArray* ChristieHandler::createElement(uint32_t key, DataType type, char* data, int sizeData)
{
    // -- create intermediate array
    ByteArray* array = new ByteArray(150);
    int offset = 0;

    // -- set key
    array->setUInt32(key, offset, true);
    offset += 4;

    // -- calculate size of message
    int berLength = 1;
    switch (type)
    {
        case ASCII_STRING:
        case UUID:
        case UTF8_STRING:
        case XML:
        berLength += sizeData;
        break;

        case UINT8:
        case INT8:
        case BOOLEAN:
        berLength += 1;
        break;

        case UINT16:
        case INT16:
        berLength += 2;
        break;

        case UINT32:
        case INT32:
        case FLOAT:
        berLength += 4;
        break;

        case UINT64:
        case INT64:
        case DOUBLE:
        berLength += 8;
        break;
    };

    // -- ber encoded
    std::vector<char> berData = berEncoder(berLength);
    if (berData.size() == 1) {
        array->setUInt8(berData.at(0), offset);
        offset += 1;
    }
    else if (berData.size() == 2) {
        array->setUInt8(berData.at(0), offset);
        array->setUInt8(berData.at(1), offset+1);
        offset += 2;
    }
    else if (berData.size() == 3) {
        array->setUInt8(berData.at(0), offset);
        array->setUInt8(berData.at(1), offset+1);
        array->setUInt8(berData.at(2), offset+2);
        offset += 3;
    }
    else if (berData.size() == 4) {
        array->setUInt8(berData.at(0), offset);
        array->setUInt8(berData.at(1), offset+1);
        array->setUInt8(berData.at(2), offset+2);
        array->setUInt8(berData.at(3), offset+3);
        offset += 4;
    }

    // -- set type
    array->setUInt8(type, offset);
    offset += 1;

    uint16_t* temp16;
    uint32_t* temp32;
    uint64_t* temp64;

    // -- set value
    switch (type)
    {
        case ASCII_STRING:
        case UUID:
        case UTF8_STRING:
        case XML:
        array->set((uint8_t*)data, offset, sizeData);
        offset += sizeData;
        break;

        case UINT8:
        case INT8:
        case BOOLEAN:
        array->setUInt8(data[0], offset);
        offset += 1;
        break;

        case UINT16:
        case INT16:
        temp16 = (uint16_t*)data;
        array->setUInt16(*temp16, offset, true);
        offset += 2;
        break;

        case UINT32:
        case INT32:
        case FLOAT:
        temp32 = (uint32_t*)data;
        array->setUInt32(*temp32, offset, true);
        offset += 4;
        break;

        case UINT64:
        case INT64:
        case DOUBLE:
        temp64 = (uint64_t*)data;
        array->setUInt64(*temp64, offset, true);
        offset += 8;
        break;
    };

    array->resize(offset);
    return array;
}
std::vector<char> ChristieHandler::berEncoder(int valueToEncode) {

    if (valueToEncode < 128)
    {
        return {((char*)&valueToEncode)[0]};
    }
    else if (valueToEncode < 0x100)
    {
        return { (char)0x80 | (char)0x01, // 0x01 means next one byte contains length
                            (char) valueToEncode };
    }
    else if (valueToEncode < 0x10000)
    {
        return { (char)0x80 | (char)0x02, // 0x02 means next two bytes contain length
                            (char)(0xFF & (valueToEncode >> 8)),
                            (char)(0xFF & valueToEncode) };
    }
    else if (valueToEncode < 0x1000000)
    {
        return { (char)0x80 | (char)0x03, // 0x03 means next three bytes contain length
                            (char)(0xFF & (valueToEncode >> 16)),
                            (char)(0xFF & (valueToEncode >> 8)),
                            (char)(0xFF & valueToEncode) };
    }
    else
    {
        return {}; // empty buffer
        //error, over 1MB not supported
    }
}

uint32_t ChristieHandler::berDecoder(int* berLength, uint8_t* data)
{
    if (data[0] == 0x81)
    {
        *berLength = 2;
        return data[1];
    }
    else if (data[0] == 0x82)
    {
        *berLength = 3;
        return (data[1] << 8) + data[2];
    }
    else if (data[0] == 0x83)
    {
        *berLength = 4;
        return (data[1] << 16) + (data[2] << 8) + data[3];
    }
    else {
        *berLength = 1;
        return data[0];
    }
}