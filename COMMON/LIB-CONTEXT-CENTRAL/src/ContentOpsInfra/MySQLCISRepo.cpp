#include "ContentOpsInfra/MySQLCISRepo.h"

std::string MySQLCISRepo::_database = "ice";
std::string MySQLCISRepo::_table = "releases";

Query* MySQLCISRepo::MySQLcreate(CIS* cis)
{
    return MySQLupdate(cis);
}

Query* MySQLCISRepo::MySQLread(CIS* cis)
{
    _CISids = cis->GetCISId();
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id_content", nullptr, "int");
    readQuery->addParameter("id_type", nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("release_cis_path", nullptr, "string");
    if(*&_CISids[0] != -1 ) { readQuery->addWhereParameter("id_content", &_CISids[0], "int"); };
    if(*&_CISids[1] != -1 ) { readQuery->addWhereParameter("id_type", &_CISids[1], "int"); };
    if(*&_CISids[2] != -1 ) { readQuery->addWhereParameter("id_localisation", &_CISids[2], "int"); };
    return readQuery;
}

Query* MySQLCISRepo::MySQLupdate(CIS* cis)
{
    _CISids = cis->GetCISId();
    std::string _pathCIS = cis->GetCISPath();
    if(_CISids[0] == -1 || _CISids[1] == -1 || _CISids[2] == -1) { return nullptr; }
    Query* updateQuery = new Query(Query::UPDATE, _database, _table);

    updateQuery->addParameter("release_cis_path", &_pathCIS , "string");
    updateQuery->addWhereParameter("id_content", &_CISids[0], "int");
    updateQuery->addWhereParameter("id_type", &_CISids[1], "int");
    updateQuery->addWhereParameter("id_localisation", &_CISids[2], "int");
    return updateQuery;
}
Query* MySQLCISRepo::MySQLremove(CIS* cis)
{
    cis->SetCISInfos(nullptr);
    return MySQLupdate(cis);
}