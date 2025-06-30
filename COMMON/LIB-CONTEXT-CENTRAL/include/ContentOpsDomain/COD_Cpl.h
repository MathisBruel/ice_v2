#pragma once
#include <string>
#include <memory>

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