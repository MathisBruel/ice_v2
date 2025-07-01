#pragma once
#include <string>
#include <memory>
#include "COD_Sync.h"
#include <iostream>

class COD_CplRepo;

class COD_Cpl
{
public:
    COD_Cpl();
    COD_Cpl(int id, const std::string& name, const std::string& uuid, const std::string& pathSync = "");
    ~COD_Cpl();

    // Setters
    void SetId(int id) { _id = id; }
    void SetName(const std::string& name) { _name = name; }
    void SetUuid(const std::string& uuid) { _uuid = uuid; }
    void SetPathSync(const std::string& pathSync) { _pathSync = pathSync; }

    // Getters
    int GetId() const { return _id; }
    std::string GetName() const { return _name; }
    std::string GetUuid() const { return _uuid; }
    std::string GetPathSync() const { return _pathSync; }

private:
    int _id;
    std::string _name;
    std::string _uuid;
    std::string _pathSync;
    std::shared_ptr<COD_CplRepo> _cplRepo;
};

// Déclaration de la classe COD_CplRelease (issue de l'ancien COD_CPL.h)
class CPLRepo;
class COD_CplRelease
{
public:
    COD_CplRelease();
    ~COD_CplRelease();

    void CreateSync(std::string SyncPath);

    void SetCPLId(int id_serv_pair_config, int id_content, int id_type, int id_localisation);
    void SetDatas(std::string CPLUUID, std::string CPLName);
    void SetCplInfos(std::string CPLPath);
    void DeleteSync() {delete this->_sync;}
    std::string toXmlString();

    int* GetCPLId() {return this->_CPLId;}
    std::string GetCPLUUID() {return this->_CPLUUID;}
    std::string GetCPLName() {return this->_CPLName;}
    std::string GetCPLPath() {return this->_CPLPath;}
    COD_Sync* GetSync() {return this->_sync;}

private:
    CPLRepo* _cplRepo;
    COD_Sync* _sync;
    
    int _CPLId[4];
    std::string _CPLUUID;
    std::string _CPLName;
    std::string _CPLPath;
}; 