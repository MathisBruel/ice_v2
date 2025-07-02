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

    void SetId(int id) { _id = id; }
    void SetName(const std::string& name) { _name = name; }
    void SetUuid(const std::string& uuid) { _uuid = uuid; }
    void SetPathSync(const std::string& pathSync) { _pathSync = pathSync; }
    void SetTypeCpl(int v) { _type_cpl = v; }
    void SetSha1Sync(const std::string& v) { _sha1_sync = v; }
    void SetPathCpl(const std::string& v) { _path_cpl = v; }
    void SetIdServPairConfig(long long v) { _id_serv_pair_config = v; }
    void SetIdContent(long long v) { _id_content = v; }
    void SetIdType(long long v) { _id_type = v; }
    void SetIdLocalisation(long long v) { _id_localisation = v; }

    int GetId() const { return _id; }
    std::string GetName() const { return _name; }
    std::string GetUuid() const { return _uuid; }
    std::string GetPathSync() const { return _pathSync; }
    int GetTypeCpl() const { return _type_cpl; }
    std::string GetSha1Sync() const { return _sha1_sync; }
    std::string GetPathCpl() const { return _path_cpl; }
    long long GetIdServPairConfig() const { return _id_serv_pair_config; }
    long long GetIdContent() const { return _id_content; }
    long long GetIdType() const { return _id_type; }
    long long GetIdLocalisation() const { return _id_localisation; }

private:
    int _id;
    std::string _name;
    std::string _uuid;
    int _type_cpl;
    std::string _sha1_sync;
    std::string _path_cpl;
    std::string _pathSync;
    long long _id_serv_pair_config;
    long long _id_content;
    long long _id_type;
    long long _id_localisation;
    std::shared_ptr<COD_CplRepo> _cplRepo;
};

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