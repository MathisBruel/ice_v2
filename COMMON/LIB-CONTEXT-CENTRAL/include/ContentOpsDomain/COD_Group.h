#pragma once

#include <string>

class COD_GroupRepo;

class COD_Group
{
public:
    COD_Group();
    ~COD_Group();

    void SetGroupId(int groupId);
    void SetDatas(std::string groupName, int groupParent);

    // Versions const pour toXmlString()
    int GetGroupId() const {return this->_groupId;}
    std::string GetGroupName() const {return this->_groupName;}
    int GetGroupParent() const {return this->_groupParent;}

    // Versions non-const pour MySQLGroupRepo (compatibilité)
    int* GetGroupIdPtr() {return &this->_groupId;}
    std::string* GetGroupNamePtr() {return &this->_groupName;}
    int& GetGroupParentRef() {return this->_groupParent;}

private:
    COD_GroupRepo* _groupRepo;
    int _groupId;
    std::string _groupName;
    int _groupParent;
}; 