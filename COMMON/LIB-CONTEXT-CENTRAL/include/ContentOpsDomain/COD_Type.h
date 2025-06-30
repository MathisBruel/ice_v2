#pragma once
#include <string>

class COD_Type
{
public:
    COD_Type();
    COD_Type(int id, std::string name);
    ~COD_Type();

    void SetId(int id) { this->_id = id; }
    void SetName(std::string name) { this->_name = name; }

    int GetId() const { return this->_id; }
    std::string GetName() const { return this->_name; }

private:
    int _id;
    std::string _name;
}; 