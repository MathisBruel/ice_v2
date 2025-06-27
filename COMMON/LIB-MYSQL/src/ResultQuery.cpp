#include "ResultQuery.h"

ResultQuery::ResultQuery(std::map<std::string, std::string> schemaParameter)
{
    this->schemaParameter = std::map<std::string, std::string>(schemaParameter);
    validExecution = false;
}
ResultQuery::~ResultQuery()
{
    for (int i = 0; i < valueParameter.size(); i++) {
        std::map<std::string, void*>::iterator it;
        for (it = valueParameter.at(i).begin(); it != valueParameter.at(i).end(); it++) 
        {
            std::string type = schemaParameter.find(it->first)->second;
            if (type == "string") {
                std::string* param = static_cast<std::string*>(it->second);
                delete param;
            }
            else if (type == "int") {
                int* param = static_cast<int*>(it->second);
                delete param;
            }
            else if (type == "double") {
                double* param = static_cast<double*>(it->second);
                delete param;
            }
            else if (type == "date") {
                Poco::DateTime* param = static_cast<Poco::DateTime*>(it->second);
                delete param;
            }
        }
    }
}

void ResultQuery::createRow()
{
    std::map<std::string, void*> newmap;
    valueParameter.push_back(newmap);
}
void ResultQuery::addParamToLastRow(std::string key, void* value)
{
    valueParameter.at(valueParameter.size()-1).insert_or_assign(key, value);
}

int* ResultQuery::getIntValue(int idx, std::string key)
{
    if (idx < 0 || idx >= valueParameter.size()) {
        Poco::Logger::get("ResultQuery").error("Index is outside array of rows !", __FILE__, __LINE__);
        return nullptr;
    }

    std::map<std::string, std::string>::iterator it = schemaParameter.find(key);
    if (it == schemaParameter.end()) {
        Poco::Logger::get("ResultQuery").error("Key for param is not existing !", __FILE__, __LINE__);
        return nullptr;
    }

    if (it->second != "int") {
        Poco::Logger::get("ResultQuery").error("Param is not int type !", __FILE__, __LINE__);
        return nullptr;
    }
    std::map<std::string, void* >::iterator itValue = valueParameter.at(idx).find(key);
    if (itValue->second == nullptr) {
        return nullptr;
    }
    int* param = static_cast<int*>(valueParameter.at(idx).find(key)->second);
    return param;
}
double* ResultQuery::getDoubleValue(int idx, std::string key)
{
    if (idx < 0 || idx >= valueParameter.size()) {
        Poco::Logger::get("ResultQuery").error("Index is outside array of rows !", __FILE__, __LINE__);
        return nullptr;
    }

    std::map<std::string, std::string>::iterator it = schemaParameter.find(key);
    if (it == schemaParameter.end()) {
        Poco::Logger::get("ResultQuery").error("Key for param is not existing !", __FILE__, __LINE__);
        return nullptr;
    }

    if (it->second != "double") {
        Poco::Logger::get("ResultQuery").error("Param is not double type !", __FILE__, __LINE__);
        return nullptr;
    }
    std::map<std::string, void* >::iterator itValue = valueParameter.at(idx).find(key);
    if (itValue->second == nullptr) {
        return nullptr;
    }
    double* param = static_cast<double*>(valueParameter.at(idx).find(key)->second);
    return param;
}
std::string* ResultQuery::getStringValue(int idx, std::string key)
{
    if (idx < 0 || idx >= valueParameter.size()) {
        Poco::Logger::get("ResultQuery").error("Index is outside array of rows !", __FILE__, __LINE__);
        return nullptr;
    }

    std::map<std::string, std::string>::iterator it = schemaParameter.find(key);
    if (it == schemaParameter.end()) {
        Poco::Logger::get("ResultQuery").error("Key for param is not existing !", __FILE__, __LINE__);
        return nullptr;
    }

    if (it->second != "string") {
        Poco::Logger::get("ResultQuery").error("Param is not double type !", __FILE__, __LINE__);
        return nullptr;
    }
    std::map<std::string, void* >::iterator itValue = valueParameter.at(idx).find(key);
    if (itValue->second == nullptr) {
        return nullptr;
    }
    std::string* param = static_cast<std::string*>(valueParameter.at(idx).find(key)->second);
    return param;
}
Poco::DateTime* ResultQuery::getDateValue(int idx, std::string key)
{
    if (idx < 0 || idx >= valueParameter.size()) {
        Poco::Logger::get("ResultQuery").error("Index is outside array of rows !", __FILE__, __LINE__);
        return nullptr;
    }

    std::map<std::string, std::string>::iterator it = schemaParameter.find(key);
    if (it == schemaParameter.end()) {
        Poco::Logger::get("ResultQuery").error("Key for param is not existing !", __FILE__, __LINE__);
        return nullptr;
    }

    if (it->second != "date") {
        Poco::Logger::get("ResultQuery").error("Param is not date type !", __FILE__, __LINE__);
        return nullptr;
    }
    std::map<std::string, void* >::iterator itValue = valueParameter.at(idx).find(key);
    if (itValue->second == nullptr) {
        return nullptr;
    }
    if (valueParameter.at(idx).find(key)->second == "") {
        return nullptr;
    }
    Poco::DateTime* param = static_cast<Poco::DateTime*>(valueParameter.at(idx).find(key)->second);
    return param;
}