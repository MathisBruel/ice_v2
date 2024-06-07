#include <iostream>
#include "CISRepo.h"

class CIS
{
public:
    CIS();
    ~CIS();

    void setId(int id_movie, int id_type, int id_localisation);
    void setCISInfos(std::string pathCIS);

    const int* getId() {return this->ids;}
    std::string getCISPath() {return this->pathCIS;}
private:
    CISRepo* cisRepo;

    int ids[4];
    std::string pathCIS;
};