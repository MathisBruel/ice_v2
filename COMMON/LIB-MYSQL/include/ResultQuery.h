#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>

#include "Poco/Logger.h"
#include "Poco/DateTime.h"

#pragma once

class ResultQuery
{

public:

    ResultQuery(std::map<std::string, std::string> schemaParameter);
    ~ResultQuery();

    void createRow();
    void addParamToLastRow(std::string key, void* value);

    int getNbRows() {return valueParameter.size();}
    
    int* getIntValue(int idx, std::string key);
    double* getDoubleValue(int idx, std::string key);
    std::string* getStringValue(int idx, std::string key);
    Poco::DateTime* getDateValue(int idx, std::string key);

    void setValid() {validExecution = true;}
    bool isValid() {return validExecution;}

    void setLastInsertedId(int id) {this->lastInsertedId = id;}
    int getLastInsertedId() {return lastInsertedId;}

    void setError(int code, std::string message) {this->errorCode = code; this->errorMessage = message;}
    int getErrorCode() {return errorCode;}
    std::string getErrorMessage() {return errorMessage;}

private: 

    // -- association key / type parameters (string, int, double, date)
    std::map<std::string, std::string> schemaParameter;

    // -- values rows by rows
    std::vector<std::map<std::string, void*>> valueParameter;

    bool validExecution;
    int lastInsertedId;
    int errorCode;
    std::string errorMessage;
};