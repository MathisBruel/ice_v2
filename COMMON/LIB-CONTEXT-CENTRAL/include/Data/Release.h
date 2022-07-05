#include <stdlib.h>
#include <iostream>
#include <map>
#include <memory>
#include "Poco/DateTime.h"
#include "Poco/Mutex.h"

#include "Script.h"
#include "Cpl.h"
#include "ReleaseCut.h"
#include "Cinema.h"

#pragma once

class Release
{

public:

    Release();
    ~Release();

    void setId(int id) {this->id = id;}
    void setDatas(int id_feature, std::string name);
    void setDate(std::shared_ptr<Poco::DateTime> in, std::shared_ptr<Poco::DateTime> out);
    void setPattern(std::string global, std::string specific);
    void setFinalized(bool finalized) {this->finalized = finalized;}
    void setParent(int id_parent_release) {this->id_parent_release = id_parent_release;}

    Query* createQuery();
    Query* updateQuery();
    Query* deleteQuery();
    static Query* getQuery(int* id = nullptr);
    static Query* getQueryForFeature(int* id_feature);
    static Query* getQueryForParent(int* id_parent);

    static std::map<int, std::shared_ptr<Release>> loadListFromResult(ResultQuery* result);
    static std::shared_ptr<Release> loadFromResult(ResultQuery* result);

    std::string toXmlString(bool printChild);
    
    // -- execution setters
    void setFeatureScript(std::shared_ptr<Script> script) {this->featureScript = script;}
    void setLoopScript(std::shared_ptr<Script> script) {this->loopScript = script;}
    void setSasScript(std::shared_ptr<Script> script) {this->sasScript = script;}
    void addFeatureCpl(std::shared_ptr<Cpl> cpl) {featureCpls.push_back(cpl);}
    void addCut(std::shared_ptr<ReleaseCut> cut) {cuts.push_back(cut);}
    void setParentRelease(std::shared_ptr<Release> parent) {parentRelease = parent;}
    void addCinema(std::shared_ptr<Cinema> cinema) {cinemas.push_back(cinema);}

    // -- general getters
    int getId() {return id;}
    int getIdParent() {return id_parent_release;}
    std::string getName() {return name;}
    // -- date getters
    std::shared_ptr<Poco::DateTime> getInDate() {return in;}
    std::shared_ptr<Poco::DateTime> getOutDate() {return out;}
    // -- patterns
    std::string getGlobalPattern() {return globalPattern;}
    std::string getSpecificPattern() {return specificPattern;}
    bool isFinalized() {return finalized;}

    // -- execution getters
    std::shared_ptr<Script> getFeatureScript() {return featureScript;}
    std::shared_ptr<Script> getLoopScript() {return loopScript;}
    std::shared_ptr<Script> getSasScript() {return sasScript;}
    std::vector<std::shared_ptr<Cpl>> getFeatureCpls() {return featureCpls;}
    std::vector<std::shared_ptr<ReleaseCut>> getCuts() {return cuts;}
    std::shared_ptr<Release> getParentRelease() {return parentRelease;}
    std::vector<std::shared_ptr<Cinema>> getCinemas() {return cinemas;}

private:

    // ---------------------
    // database
    // ---------------------
    int id;
    int id_feature;
    std::string name;
    // -- date
    std::shared_ptr<Poco::DateTime> in;
    std::shared_ptr<Poco::DateTime> out;
    // -- pattern for searching CPL
    std::string globalPattern;
    std::string specificPattern;
    // -- parent release
    int id_parent_release;
    // -- determine whether auto synchro can be done on this release
    bool finalized;
    int finalizedInt;

    static std::string database;
    static std::string table;

    // ---------------------
    // execution
    // ---------------------
    // -- synchro
    std::vector<std::shared_ptr<Cpl>> featureCpls;
    // -- chaining release for synchronization
    std::shared_ptr<Release> parentRelease;
    // -- scripts
    std::shared_ptr<Script> featureScript;
    std::shared_ptr<Script> loopScript;
    std::shared_ptr<Script> sasScript;
    // -- cuts
    std::vector<std::shared_ptr<ReleaseCut>> cuts;
    // -- cinemas
    std::vector<std::shared_ptr<Cinema>> cinemas;
};