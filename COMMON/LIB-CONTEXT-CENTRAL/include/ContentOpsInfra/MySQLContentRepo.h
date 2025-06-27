#pragma once

#include "ContentOpsDomain/COD_ContentRepo.h"
#include <iostream>
#include "Query.h"
#include "ResultQuery.h"
#include "ContentOpsDomain/COD_Content.h"
#include "ContentOpsBoundary/COB_Content.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include <memory>

class MySQLContentRepo : public COD_ContentRepo
{
public:
    MySQLContentRepo();
    virtual ~MySQLContentRepo();
    
    void Create(COD_Content* content) override;
    void Read(COD_Content* content) override;
    void Update(COD_Content* content) override;
    void Remove(COD_Content* content) override;

    std::unique_ptr<Query> MySQLcreate(COD_Content* content);
    std::unique_ptr<Query> MySQLread(COD_Content* content);
    std::unique_ptr<Query> MySQLread();
    std::unique_ptr<Query> MySQLupdate(COD_Content* content);
    std::unique_ptr<Query> MySQLremove(COD_Content* content);

    Query* GetQuery() { return _query.get(); }
    
    std::unique_ptr<ResultQuery> getContents() override;
    std::unique_ptr<ResultQuery> getContent(int contentId) override;

private:
    static std::string _database;
    static std::string _table;

    int* _contentIds;
    std::string _contentTitles;

    std::unique_ptr<Query> _query;
};