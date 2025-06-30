#pragma once
#include <string>

class COD_Localisation
{
public:
    COD_Localisation();
    COD_Localisation(int id, std::string name);
    ~COD_Localisation();

    void SetId(int id) { this->_id = id; }
    void SetName(std::string name) { this->_name = name; }

    int GetId() const { return this->_id; }
    std::string GetName() const { return this->_name; }

private:
    int _id;
    std::string _name;
}; 