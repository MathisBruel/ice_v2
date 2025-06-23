#pragma once
#include <iostream>

class CISRepo;
class CIS
{
public:
    CIS();
    ~CIS();

    void SetCISId(int id_content, int id_type, int id_localisation);
    void SetCISInfos(std::string CISPath);

    int* GetCISId() {return this->_CISId;}
    std::string GetCISPath() {return this->_CISPath;}

    
private:
    CISRepo* _CISRepo;

    int _CISId[3];
    std::string _CISPath;
}; 