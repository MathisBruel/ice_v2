#include <iostream>
#include <stdlib.h>
#include <vector>

#include "Query.h"
#include "ResultQuery.h"

#pragma once

class LinkParam
{

public:

    enum TypeLink
    {
        CINEMA_GROUP,
        CINEMA_RELEASE,
        CPL_RELEASE,
        GROUP_RELEASE,
        SCRIPT_RELEASE,
        CPL_SCRIPT
    };

    LinkParam(TypeLink link);
    ~LinkParam();

    void setId(int id) {this->id = id;}
    void setDatas(int id_src, int id_dst);

    Query* createQuery();
    Query* deleteQuery();
    Query* deletePreciseQuery();
    Query* deleteFromSrcQuery();
    Query* deleteFromDstQuery();
    static Query* getQuery(TypeLink link);
    static Query* getQuerySrc(TypeLink link, int* id_src);
    static Query* getQueryDst(TypeLink link, int* id_dst);

    static std::vector<std::shared_ptr<LinkParam>> loadListFromResult(ResultQuery* result, TypeLink link);

    int getSrcId() {return id_src;}
    int getDstId() {return id_dst;}

private:

    TypeLink link;

    int id;
    int id_src;
    int id_dst;

    static std::string database;
    std::string table;
};