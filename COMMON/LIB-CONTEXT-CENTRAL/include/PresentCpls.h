#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

#pragma once

#include "Data/Cpl.h"

class PresentCpl
{

public:

    PresentCpl(int id_auditorium) {this->id_auditorium = id_auditorium;}
    ~PresentCpl() {}

    void addCpl(std::shared_ptr<Cpl> cpl) {cpls.push_back(cpl);}

    int getIdAuditorium() {return id_auditorium;}
    std::vector<std::shared_ptr<Cpl>> getCpls() {return cpls;}

private:

    int id_auditorium;
    std::vector<std::shared_ptr<Cpl>> cpls;
};