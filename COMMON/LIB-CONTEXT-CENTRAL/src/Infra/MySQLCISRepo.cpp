#include "Infra/MySQLCISRepo.h"

std::string MySQLCISRepo::_database = "ice";
std::string MySQLCISRepo::_table = "release";

Query* MySQLCISRepo::MySQLcreate(CIS* cis)
{
    return MySQLupdate(cis);
}

Query* MySQLCISRepo::MySQLread(CIS* cis)
{
    int* CISids = cis->GetCISId();
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id_movie", nullptr, "int");
    readQuery->addParameter("id_type", nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("release_cis_path", nullptr, "string");
    if(&CISids[0] != nullptr ) { readQuery->addWhereParameter("id_movie", &CISids[0], "int"); };
    if(&CISids[1] != nullptr ) { readQuery->addWhereParameter("id_type", &CISids[1], "int"); };
    if(&CISids[2] != nullptr ) { readQuery->addWhereParameter("id_localisation", &CISids[2], "int"); };
    return readQuery;
}

Query* MySQLCISRepo::MySQLupdate(CIS* cis)
{
    int* CISids = cis->GetCISId();
    std::string pathCIS = cis->GetCISPath();
    if(CISids[0] == -1 || CISids[1] == -1 || CISids[2] == -1) { return nullptr; }
    Query* updateQuery = new Query(Query::UPDATE, _database, _table);

    updateQuery->addParameter("release_cis_path", &pathCIS , "string");
    updateQuery->addWhereParameter("id_movie", &CISids[0], "int");
    updateQuery->addWhereParameter("id_type", &CISids[1], "int");
    updateQuery->addWhereParameter("id_localisation", &CISids[2], "int");
    return updateQuery;
}
Query* MySQLCISRepo::MySQLremove(CIS* cis)
{
    cis->SetCISInfos(nullptr);
    return MySQLupdate(cis);
}