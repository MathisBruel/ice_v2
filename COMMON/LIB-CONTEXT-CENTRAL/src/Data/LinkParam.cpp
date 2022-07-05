#include "Data/LinkParam.h"

std::string LinkParam::database = "ice";

LinkParam::LinkParam(TypeLink link) {
    id = -1;
    id_src = -1;
    id_dst = -1;

    this->link = link;

    switch (link)
    {
        case CINEMA_GROUP:
            table = "link_cinema_group";
            break;
        case CINEMA_RELEASE:
            table = "link_cinema_release";
            break;
        case CPL_RELEASE:
            table = "link_cpl_release";
            break;
        case GROUP_RELEASE:
            table = "link_group_release";
            break;
        case SCRIPT_RELEASE:
            table = "link_script_release";
            break;
        case CPL_SCRIPT:
            table = "link_cpl_script";
            break;
    };
}
LinkParam::~LinkParam() {}

void LinkParam::setDatas(int id_src, int id_dst)
{
    this->id_src = id_src;
    this->id_dst = id_dst;
}
Query* LinkParam::createQuery()
{
    if (id != -1) {
        return nullptr;
    }

    std::string srcName = "";
    std::string dstName = "";

    switch (link)
    {
        case CINEMA_GROUP:
            srcName = "id_cinema";
            dstName = "id_group";
            break;
        case CINEMA_RELEASE:
            srcName = "id_cinema";
            dstName = "id_release";
            break;
        case CPL_RELEASE:
            srcName = "id_cpl";
            dstName = "id_release";
            break;
        case GROUP_RELEASE:
            srcName = "id_group";
            dstName = "id_release";
            break;
        case SCRIPT_RELEASE:
            srcName = "id_script";
            dstName = "id_release";
            break;
        case CPL_SCRIPT:
            srcName = "id_cpl";
            dstName = "id_script";
            break;
    };

    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter(srcName, &id_src, "int");
    createQuery->addParameter(dstName, &id_dst, "int");
    return createQuery;
}
Query* LinkParam::deleteQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id", &id, "int");
    return deleteQuery;
}
Query* LinkParam::deletePreciseQuery()
{
    if (id_dst == -1 || id_src == -1) {
        return nullptr;
    }

    std::string srcName = "";
    std::string dstName = "";

    switch (link)
    {
        case CINEMA_GROUP:
            srcName = "id_cinema";
            dstName = "id_group";
            break;
        case CINEMA_RELEASE:
            srcName = "id_cinema";
            dstName = "id_release";
            break;
        case CPL_RELEASE:
            srcName = "id_cpl";
            dstName = "id_release";
            break;
        case GROUP_RELEASE:
            srcName = "id_group";
            dstName = "id_release";
            break;
        case SCRIPT_RELEASE:
            srcName = "id_script";
            dstName = "id_release";
            break;
        case CPL_SCRIPT:
            srcName = "id_cpl";
            dstName = "id_script";
            break;
    };

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter(srcName, &id_src, "int");
    deleteQuery->addWhereParameter(dstName, &id_dst, "int");
    return deleteQuery;
}
Query* LinkParam::deleteFromSrcQuery()
{
    if (id_src == -1) {
        return nullptr;
    }

    std::string srcName = "";
    std::string dstName = "";

    switch (link)
    {
        case CINEMA_GROUP:
            srcName = "id_cinema";
            dstName = "id_group";
            break;
        case CINEMA_RELEASE:
            srcName = "id_cinema";
            dstName = "id_release";
            break;
        case CPL_RELEASE:
            srcName = "id_cpl";
            dstName = "id_release";
            break;
        case GROUP_RELEASE:
            srcName = "id_group";
            dstName = "id_release";
            break;
        case SCRIPT_RELEASE:
            srcName = "id_script";
            dstName = "id_release";
            break;
        case CPL_SCRIPT:
            srcName = "id_cpl";
            dstName = "id_script";
            break;
    };

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter(srcName, &id_src, "int");
    return deleteQuery;
}
Query* LinkParam::deleteFromDstQuery()
{
    if (id_dst == -1) {
        return nullptr;
    }

    std::string srcName = "";
    std::string dstName = "";

    switch (link)
    {
        case CINEMA_GROUP:
            srcName = "id_cinema";
            dstName = "id_group";
            break;
        case CINEMA_RELEASE:
            srcName = "id_cinema";
            dstName = "id_release";
            break;
        case CPL_RELEASE:
            srcName = "id_cpl";
            dstName = "id_release";
            break;
        case GROUP_RELEASE:
            srcName = "id_group";
            dstName = "id_release";
            break;
        case SCRIPT_RELEASE:
            srcName = "id_script";
            dstName = "id_release";
            break;
        case CPL_SCRIPT:
            srcName = "id_cpl";
            dstName = "id_script";
            break;
    };

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter(dstName, &id_dst, "int");
    return deleteQuery;
}
Query* LinkParam::getQuery(TypeLink link)
{
    std::string table = "";
    std::string srcName = "";
    std::string dstName = "";

    switch (link)
    {
        case CINEMA_GROUP:
            table = "link_cinema_group";
            srcName = "id_cinema";
            dstName = "id_group";
            break;
        case CINEMA_RELEASE:
            table = "link_cinema_release";
            srcName = "id_cinema";
            dstName = "id_release";
            break;
        case CPL_RELEASE:
            table = "link_cpl_release";
            srcName = "id_cpl";
            dstName = "id_release";
            break;
        case GROUP_RELEASE:
            table = "link_group_release";
            srcName = "id_group";
            dstName = "id_release";
            break;
        case SCRIPT_RELEASE:
            table = "link_script_release";
            srcName = "id_script";
            dstName = "id_release";
            break;
        case CPL_SCRIPT:
            table = "link_cpl_script";
            srcName = "id_cpl";
            dstName = "id_script";
            break;
    };

    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter(srcName, nullptr, "int");
    getQuery->addParameter(dstName, nullptr, "int");
    return getQuery;
}

Query* LinkParam::getQuerySrc(TypeLink link, int* id_src)
{
    std::string table = "";
    std::string srcName = "";
    std::string dstName = "";

    switch (link)
    {
        case CINEMA_GROUP:
            table = "link_cinema_group";
            srcName = "id_cinema";
            dstName = "id_group";
            break;
        case CINEMA_RELEASE:
            table = "link_cinema_release";
            srcName = "id_cinema";
            dstName = "id_release";
            break;
        case CPL_RELEASE:
            table = "link_cpl_release";
            srcName = "id_cpl";
            dstName = "id_release";
            break;
        case GROUP_RELEASE:
            table = "link_group_release";
            srcName = "id_group";
            dstName = "id_release";
            break;
        case SCRIPT_RELEASE:
            table = "link_script_release";
            srcName = "id_script";
            dstName = "id_release";
            break;
        case CPL_SCRIPT:
            table = "link_cpl_script";
            srcName = "id_cpl";
            dstName = "id_script";
            break;
    };

    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter(srcName, nullptr, "int");
    getQuery->addParameter(dstName, nullptr, "int");
    getQuery->addWhereParameter(srcName, id_src, "int");
    return getQuery;
}
Query* LinkParam::getQueryDst(TypeLink link, int* id_dst)
{
    std::string table = "";
    std::string srcName = "";
    std::string dstName = "";

    switch (link)
    {
        case CINEMA_GROUP:
            table = "link_cinema_group";
            srcName = "id_cinema";
            dstName = "id_group";
            break;
        case CINEMA_RELEASE:
            table = "link_cinema_release";
            srcName = "id_cinema";
            dstName = "id_release";
            break;
        case CPL_RELEASE:
            table = "link_cpl_release";
            srcName = "id_cpl";
            dstName = "id_release";
            break;
        case GROUP_RELEASE:
            table = "link_group_release";
            srcName = "id_group";
            dstName = "id_release";
            break;
        case SCRIPT_RELEASE:
            table = "link_script_release";
            srcName = "id_script";
            dstName = "id_release";
            break;
        case CPL_SCRIPT:
            table = "link_cpl_script";
            srcName = "id_cpl";
            dstName = "id_script";
            break;
    };

    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter(srcName, nullptr, "int");
    getQuery->addParameter(dstName, nullptr, "int");
    getQuery->addWhereParameter(dstName, id_dst, "int");
    return getQuery;
}

std::vector<std::shared_ptr<LinkParam>> LinkParam::loadListFromResult(ResultQuery* result, TypeLink link)
{
    std::string srcName = "";
    std::string dstName = "";

    switch (link)
    {
        case CINEMA_GROUP:
            srcName = "id_cinema";
            dstName = "id_group";
            break;
        case CINEMA_RELEASE:
            srcName = "id_cinema";
            dstName = "id_release";
            break;
        case CPL_RELEASE:
            srcName = "id_cpl";
            dstName = "id_release";
            break;
        case GROUP_RELEASE:
            srcName = "id_group";
            dstName = "id_release";
            break;
        case SCRIPT_RELEASE:
            srcName = "id_script";
            dstName = "id_release";
            break;
        case CPL_SCRIPT:
            srcName = "id_cpl";
            dstName = "id_script";
            break;
    };

    std::vector<std::shared_ptr<LinkParam>> list;
    for (int i = 0; i < result->getNbRows(); i++) {
        std::shared_ptr<LinkParam> linkParam = std::make_shared<LinkParam>(LinkParam(link));
        linkParam->id = *result->getIntValue(i, "id");
        linkParam->id_src = *result->getIntValue(i, srcName);
        linkParam->id_dst = *result->getIntValue(i, dstName);
        list.push_back(linkParam);
    }

    return list;
}