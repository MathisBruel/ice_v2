#include "Data/Release.h"

std::string Release::database = "ice";
std::string Release::table = "release";

Release::Release() 
{
    id = -1;
    id_feature = -1;
    name = "";
    in = nullptr;
    out = nullptr;
    globalPattern = "";
    specificPattern = "";
    id_parent_release = -1;
    finalized = false;

    // ---------------------
    // execution
    // ---------------------
    parentRelease = nullptr;
    featureScript = nullptr;
    loopScript = nullptr;
    sasScript = nullptr;
}

Release::~Release() {}

void Release::setDatas(int id_feature, std::string name)
{
    this->id_feature = id_feature;
    this->name = name;
}
void Release::setDate(std::shared_ptr<Poco::DateTime> in, std::shared_ptr<Poco::DateTime> out)
{
    this->in = in;
    this->out = out;
}
void Release::setPattern(std::string globalPattern, std::string specificPattern)
{
    this->globalPattern = globalPattern;
    this->specificPattern = specificPattern;
}

Query* Release::createQuery()
{
    if (id != -1) {
        return nullptr;
    }

    finalizedInt = (int)finalized;
    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter("id_feature", &id_feature, "int");
    createQuery->addParameter("date_in", in.get(), "date");
    createQuery->addParameter("date_out", out.get(), "date");
    createQuery->addParameter("name", &name, "string");
    createQuery->addParameter("id_parent_release", &id_parent_release, "int");
    createQuery->addParameter("global_pattern", &globalPattern, "string");
    createQuery->addParameter("specific_pattern", &specificPattern, "string");
    createQuery->addParameter("finalized", &finalizedInt, "int");
    return createQuery;
}
Query* Release::updateQuery()
{
    if (id == -1) {
        return nullptr;
    }

    finalizedInt = (int)finalized;
    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("id_feature", &id_feature, "int");
    updateQuery->addParameter("date_in", in.get(), "date");
    updateQuery->addParameter("date_out", out.get(), "date");
    updateQuery->addParameter("name", &name, "string");
    updateQuery->addParameter("id_parent_release", &id_parent_release, "int");
    updateQuery->addParameter("global_pattern", &globalPattern, "string");
    updateQuery->addParameter("specific_pattern", &specificPattern, "string");
    updateQuery->addParameter("finalized", &finalizedInt, "int");
    updateQuery->addWhereParameter("id", &id, "int");
    return updateQuery;
}
Query* Release::deleteQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id", &id, "int");
    return deleteQuery;
}
Query* Release::getQuery(int* id)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("id_feature", nullptr, "int");
    getQuery->addParameter("date_in", nullptr, "date");
    getQuery->addParameter("date_out", nullptr, "date");
    getQuery->addParameter("name", nullptr, "string");
    getQuery->addParameter("id_parent_release", nullptr, "int");
    getQuery->addParameter("global_pattern", nullptr, "string");
    getQuery->addParameter("specific_pattern", nullptr, "string");
    getQuery->addParameter("finalized", nullptr, "int");
    if (id != nullptr) {getQuery->addWhereParameter("id", id, "int");}
    return getQuery;
}
Query* Release::getQueryForFeature(int* id_feature)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("id_feature", nullptr, "int");
    getQuery->addParameter("date_in", nullptr, "date");
    getQuery->addParameter("date_out", nullptr, "date");
    getQuery->addParameter("name", nullptr, "string");
    getQuery->addParameter("id_parent_release", nullptr, "int");
    getQuery->addParameter("global_pattern", nullptr, "string");
    getQuery->addParameter("specific_pattern", nullptr, "string");
    getQuery->addParameter("finalized", nullptr, "int");
    getQuery->addWhereParameter("id_feature", id_feature, "int");
    return getQuery;
}
Query* Release::getQueryForParent(int* id_parent)
{
    if (id_parent == nullptr) {
        return nullptr;
    }

    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("id_feature", nullptr, "int");
    getQuery->addParameter("date_in", nullptr, "date");
    getQuery->addParameter("date_out", nullptr, "date");
    getQuery->addParameter("name", nullptr, "string");
    getQuery->addParameter("id_parent_release", nullptr, "int");
    getQuery->addParameter("global_pattern", nullptr, "string");
    getQuery->addParameter("specific_pattern", nullptr, "string");
    getQuery->addParameter("finalized", nullptr, "int");
    getQuery->addWhereParameter("id_parent_release", id_parent, "int");
    return getQuery;
}

std::map<int, std::shared_ptr<Release>> Release::loadListFromResult(ResultQuery* result)
{
    std::map<int, std::shared_ptr<Release>> list;
    for (int i = 0; i < result->getNbRows(); i++) {
        std::shared_ptr<Release> release = std::make_shared<Release>(Release());
        release->id = *result->getIntValue(i, "id");
        release->id_feature = *result->getIntValue(i, "id_feature");
        Poco::DateTime* in = result->getDateValue(i, "date_in");
        if (in != nullptr) {
            release->in = std::make_shared<Poco::DateTime>(*in);
        }
        Poco::DateTime* out = result->getDateValue(i, "date_out");
        if (out != nullptr) {
            release->out = std::make_shared<Poco::DateTime>(*out);
        }
        release->name = *result->getStringValue(i, "name");
        int* parent = result->getIntValue(i, "id_parent_release");
        release->id_parent_release = parent == nullptr ? -1 : *parent;
        release->globalPattern = *result->getStringValue(i, "global_pattern");
        std::string* specific = result->getStringValue(i, "specific_pattern");
        release->specificPattern = specific == nullptr ? "" : *specific;
        release->finalized = *result->getIntValue(i, "finalized");
        list.insert_or_assign(release->id, release);
    }
    return list;
}
std::shared_ptr<Release> Release::loadFromResult(ResultQuery* result)
{
    if (result->getNbRows() == 0) {
        return nullptr;
    }
    std::shared_ptr<Release> release = std::make_shared<Release>(Release());
    release->id = *result->getIntValue(0, "id");
    release->id_feature = *result->getIntValue(0, "id_feature");
    Poco::DateTime* in = result->getDateValue(0, "date_in");
    if (in != nullptr) {
        release->in = std::make_shared<Poco::DateTime>(*in);
    }
    Poco::DateTime* out = result->getDateValue(0, "date_out");
    if (out != nullptr) {
        release->out = std::make_shared<Poco::DateTime>(*out);
    }
    release->name = *result->getStringValue(0, "name");
    int* parent = result->getIntValue(0, "id_parent_release");
    release->id_parent_release = parent == nullptr ? -1 : *parent;
    release->globalPattern = *result->getStringValue(0, "global_pattern");
    std::string* specific = result->getStringValue(0, "specific_pattern");
    release->specificPattern = specific == nullptr ? "" : *specific;
    release->finalized = *result->getIntValue(0, "finalized");
    return release;
}

std::string Release::toXmlString(bool printChild)
{
    std::string xml = "<release";

    // -- general params
    xml += " id=\"" + std::to_string(id) + "\"";
    xml += " id_feature=\"" + std::to_string(id_feature) + "\"";
    xml += " name=\"" + name + "\"";
    xml += " date_in=\"" + std::to_string(in->day()) + "-" + std::to_string(in->month()) + "-" + std::to_string(in->year()) + "\"";
    if (out != nullptr) {
        xml += " date_out=\"" + std::to_string(out->day()) + "-" + std::to_string(out->month()) + "-" + std::to_string(out->year()) + "\"";
    }
    else {
        xml += " date_out=\"nullptr\"";
    }
    xml += " id_parent_release=\"" + std::to_string(id_parent_release) + "\"";
    xml += " globalPattern=\"" + globalPattern + "\"";
    xml += " specificPattern=\"" + specificPattern + "\"";
    std::string finalStr = finalized ? "true" : "false";
    xml += " finalized=\"" + finalStr + "\"";

    if (printChild) {
        xml += ">";

        // -- cpls
        xml += "<cpls>";
        for (std::shared_ptr<Cpl> cpl : featureCpls) {
            xml += cpl->toXmlString();
        }
        xml += "</cpls>";

        // -- cinemas
        xml += "<cinemas>";
        for (std::shared_ptr<Cinema> cinema : cinemas) {
            xml += cinema->toXmlString(false);
        }
        xml += "</cinemas>";

        // -- scripts
        if (featureScript != nullptr) {
            xml += "<feature>";
            xml += featureScript->toXmlString();
            xml += "</feature>";
        }
        if (loopScript != nullptr) {
            xml += "<loop>";
            xml += loopScript->toXmlString();
            xml += "</loop>";
        }
        if (sasScript != nullptr) {
            xml += "<sas>";
            xml += sasScript->toXmlString();
            xml += "</sas>";
        }

        // -- cuts
        xml += "<cuts>";
        for (std::shared_ptr<ReleaseCut> cut : cuts) {
            xml += cut->toXmlString();
        }
        xml += "</cuts>";

        // -- finish
        xml += "</release>";
    }
    else {
        xml += " />";
    }

    return xml;
}