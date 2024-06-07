#include "MySQLCISRepo.h"

std::string MySQLCISRepo::database = "ice";
std::string MySQLCISRepo::table = "release";

Query* MySQLCISRepo::create(CIS* cis)
{
    update(&cis);
}

Query* MySQLCISRepo::read(CIS* cis)
{
    const int* CISids = cis->getId();
    Query* readQuery = new Query(Query::SELECT, database, table);
    readQuery->addParameter("release_cis_path", nullptr, "string");
    if(&CISids[0] != nullptr) { readQuery->addWhereParameter("id_movie", &CISids[0], "int"); };
    if(&CISids[1] != nullptr) { readQuery->addWhereParameter("id_type", &CISids[1], "int"); };
    if(&CISids[2] != nullptr) { readQuery->addWhereParameter("id_localisation", &CISids[2], "int"); };
    return readQuery;
}

Query* MySQLCISRepo::update(CIS* cis)
{
    const int* CISids = cis->getId();
    if(CISids[0] == -1 || CISids[1] == -1 | CISids[2] == -1) { return nullptr; }
    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("release_cis_path", cis->getCISPath(), "string");
    updateQuery->addWhereParameter("id_movie", &CISids[0], "int");
    updateQuery->addWhereParameter("id_type", &CISids[1], "int");
    updateQuery->addWhereParameter("id_localisation", &CISids[2], "int");
}
Query* MySQLCISRepo::remove(CIS* cis)
{
    cis->setCISInfos(nullptr);
    update(&cis);
}