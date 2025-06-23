#include "ContentOpsInfra/MySQLCPLRepo.h"

std::string MySQLCPLRepo::_database = "ice";
std::string MySQLCPLRepo::_table = "cpl";

Query* MySQLCPLRepo::MySQLcreate(CPLRelease* cpl){
    _cplIds = cpl->GetCPLId();
    if (_cplIds[0] == -1 || _cplIds[1] == -1 || _cplIds[2] == -1 || _cplIds[3] == -1) { return nullptr; }
    
    _uuid = cpl->GetCPLUUID();
    _name = cpl->GetCPLName();
    _cplPath = cpl->GetCPLPath();

    Query* createQuery = new Query(Query::INSERT, _database, _table);
    
    createQuery->addParameter("id_serv_pair_config", &_cplIds[0], "int");
    createQuery->addParameter("id_content", &_cplIds[1], "int");
    createQuery->addParameter("id_type", &_cplIds[2], "int");
    createQuery->addParameter("id_localisation", &_cplIds[3], "int");
    createQuery->addParameter("name", &_uuid, "string");
    createQuery->addParameter("uuid", &_name, "string");
    createQuery->addParameter("path_cpl", &_cplPath, "string");
    return createQuery;
}
Query* MySQLCPLRepo::MySQLread(CPLRelease* cpl){
    _cplIds = cpl->GetCPLId();
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    
    readQuery->addParameter("id_serv_pair_config", nullptr, "int");
    readQuery->addParameter("id_content",nullptr, "int");
    readQuery->addParameter("id_type", nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("uuid", nullptr, "string");
    readQuery->addParameter("path_cpl", nullptr, "string");
    if (*&_cplIds[0] != -1 ) {readQuery->addWhereParameter("id_serv_pair_config", &_cplIds[0], "int");};
    if (*&_cplIds[1] != -1 ) {readQuery->addWhereParameter("id_content", &_cplIds[1], "int");};
    if (*&_cplIds[2] != -1 ) {readQuery->addWhereParameter("id_type", &_cplIds[2], "int");};
    if (*&_cplIds[3] != -1 ) {readQuery->addWhereParameter("id_localisation", &_cplIds[3], "int");};

    return readQuery;
}
Query* MySQLCPLRepo::MySQLupdate(CPLRelease* cpl){
    _cplIds = cpl->GetCPLId();
    if (_cplIds[0] == -1 || _cplIds[1] == -1 || _cplIds[2] == -1 || _cplIds[3] == -1) { return nullptr; }
    _uuid = cpl->GetCPLUUID();
    _name = cpl->GetCPLName();
    _cplPath = cpl->GetCPLPath();
    
    Query* updateQuery = new Query(Query::UPDATE, _database, _table);
    updateQuery->addParameter("name", &_uuid, "string");
    updateQuery->addParameter("uuid", &_name, "string");
    updateQuery->addParameter("path_cpl", &_cplPath, "string");
    updateQuery->addWhereParameter("id_serv_pair_config", &_cplIds[0], "int");
    updateQuery->addWhereParameter("id_content", &_cplIds[1], "int");
    updateQuery->addWhereParameter("id_type", &_cplIds[2], "int");
    updateQuery->addWhereParameter("id_localisation", &_cplIds[3], "int");
    return updateQuery;
}
Query* MySQLCPLRepo::MySQLremove(CPLRelease* cpl){
    _cplIds = cpl->GetCPLId();
    if (_cplIds[0] == -1 || _cplIds[1] == -1 || _cplIds[2] == -1 || _cplIds[3] == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_serv_pair_config", &_cplIds[0], "int");
    removeQuery->addWhereParameter("id_content", &_cplIds[1], "int");
    removeQuery->addWhereParameter("id_type", &_cplIds[2], "int");
    removeQuery->addWhereParameter("id_localisation", &_cplIds[3], "int");
    return removeQuery;
}