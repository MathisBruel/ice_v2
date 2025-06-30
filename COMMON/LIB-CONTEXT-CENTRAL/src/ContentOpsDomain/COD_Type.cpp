#include "ContentOpsDomain/COD_Type.h"

COD_Type::COD_Type()
{
    _id = -1;
    _name = "";
}

COD_Type::COD_Type(int id, std::string name)
{
    _id = id;
    _name = name;
}

COD_Type::~COD_Type()
{
} 