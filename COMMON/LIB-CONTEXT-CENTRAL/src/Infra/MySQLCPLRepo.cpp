#include "Infra/MySQLCPLRepo.h"

std::string MySQLCPLRepo::database = "ice";
std::string MySQLCPLRepo::table = "cpl";

Query* MySQLCPLRepo::MySQLcreate(CPL* cpl){
    int* cplIds = cpl->getId();
    if (cplIds[0] == -1 || cplIds[1] == -1 || cplIds[2] == -1 || cplIds[3] == -1) { return nullptr; }
    
    std::string uuid = cpl->getUuid();
    std::string name = cpl->getName();
    std::string cplPath = cpl->getCplPath();

    Query* createQuery = new Query(Query::INSERT, database, table);
    
    createQuery->addParameter("id_serv_pair_config", &cplIds[0], "int");
    createQuery->addParameter("id_movie", &cplIds[1], "int");
    createQuery->addParameter("id_type", &cplIds[2], "int");
    createQuery->addParameter("id_localisation", &cplIds[3], "int");
    createQuery->addParameter("CPL_name", &uuid, "string");
    createQuery->addParameter("CPL_uuid", &name, "string");
    createQuery->addParameter("CPL_path", &cplPath, "string");
    return createQuery;
}
Query* MySQLCPLRepo::MySQLread(CPL* cpl){
    int* cplIds = cpl->getId();
    Query* readQuery = new Query(Query::SELECT, database, table);
    
    readQuery->addParameter("id_serv_pair_config", nullptr, "int");
    readQuery->addParameter("id_movie",nullptr, "int");
    readQuery->addParameter("id_type", nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("CPL_name", nullptr, "string");
    readQuery->addParameter("CPL_uuid", nullptr, "string");
    readQuery->addParameter("CPL_path", nullptr, "string");
    if (&cplIds[0] != nullptr) {readQuery->addWhereParameter("id_serv_pair_config", &cplIds[0], "int");};
    if (&cplIds[1] != nullptr) {readQuery->addWhereParameter("id_movie", &cplIds[1], "int");};
    if (&cplIds[2] != nullptr) {readQuery->addWhereParameter("id_type", &cplIds[2], "int");};
    if (&cplIds[3] != nullptr) {readQuery->addWhereParameter("id_localisation", &cplIds[3], "int");};

    return readQuery;
}
Query* MySQLCPLRepo::MySQLupdate(CPL* cpl){
    int* cplIds = cpl->getId();
    if (cplIds[0] == -1 || cplIds[1] == -1 || cplIds[2] == -1 || cplIds[3] == -1) { return nullptr; }
    std::string uuid = cpl->getUuid();
    std::string name = cpl->getName();
    std::string cplPath = cpl->getCplPath();
    
    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("CPL_name", &uuid, "string");
    updateQuery->addParameter("CPL_uuid", &name, "string");
    updateQuery->addParameter("CPL_path", &cplPath, "string");
    updateQuery->addWhereParameter("id_serv_pair_config", &cplIds[0], "int");
    updateQuery->addWhereParameter("id_movie", &cplIds[1], "int");
    updateQuery->addWhereParameter("id_type", &cplIds[2], "int");
    updateQuery->addWhereParameter("id_localisation", &cplIds[3], "int");
    return updateQuery;
}
Query* MySQLCPLRepo::MySQLremove(CPL* cpl){
    int* cplIds = cpl->getId();
    if (cplIds[0] == -1 || cplIds[1] == -1 || cplIds[2] == -1 || cplIds[3] == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, database, table);
    removeQuery->addWhereParameter("id_serv_pair_config", &cplIds[0], "int");
    removeQuery->addWhereParameter("id_movie", &cplIds[1], "int");
    removeQuery->addWhereParameter("id_type", &cplIds[2], "int");
    removeQuery->addWhereParameter("id_localisation", &cplIds[3], "int");
    return removeQuery;
}