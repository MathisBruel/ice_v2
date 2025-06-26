#include "DatabaseConnector.h"

DatabaseConnector::DatabaseConnector()
{
    // register SQLite connector
    Poco::Data::MySQL::Connector::registerConnector();

    // create a session
    session = new Poco::Data::Session("MySQL", "host=localhost;port=3306;db=ice;user=cinelight;password=R@qazwsx2");
    if (!session->isGood()) {
        Poco::Logger::get("DatabaseConnector").error("Could not connect to database !", __FILE__, __LINE__);
    }
}

DatabaseConnector::~DatabaseConnector()
{
    session->close();
    Poco::Data::MySQL::Connector::unregisterConnector();
}

ResultQuery* DatabaseConnector::executeQuery(Query* query)
{
    if (query == nullptr) {
        Poco::Logger::get("DatabaseConnector").error("Query is null !", __FILE__, __LINE__);
        return nullptr;
    }

    //Poco::Logger::get("DatabaseConnector").debug("Compute Query string !", __FILE__, __LINE__);
    std::string queryString = query->getQueryString();
    if (queryString.empty()) {
        Poco::Logger::get("DatabaseConnector").error("Query is empty !", __FILE__, __LINE__);
        return nullptr;
    }

    if (!session->isConnected() || !session->isGood()) {
        session->reconnect();
    }

    //Poco::Logger::get("DatabaseConnector").debug("Execute query : " + queryString, __FILE__, __LINE__);

    ResultQuery* result = new ResultQuery(query->getSchema());
    int nbAffected = 0;

    Poco::Data::Statement statement(*session);

    try {
        statement << queryString;
        nbAffected = statement.execute();
    }
    catch (Poco::Data::MySQL::ConnectionException &ce)
    {
        Poco::Logger::get("DatabaseConnector").error("Error at execution of query : " + queryString, __FILE__, __LINE__);
        result->setError(ce.code(), ce.message());
        return result;
    }
    catch (Poco::Data::MySQL::StatementException &se)
    {
        Poco::Logger::get("DatabaseConnector").error("Error at execution of query : " + queryString, __FILE__, __LINE__);

        const std::string WHITESPACE = " \n\r\t\f\v";
        bool parseError = false;
        std::string message;
        int code;

        int positionMessage = se.message().find("[mysql_stmt_error]:");
        if (positionMessage != std::string::npos) {
            int endPositionMessage = se.message().find("[", positionMessage+19);
            if (endPositionMessage != std::string::npos) {
                message = se.message().substr(positionMessage+19, endPositionMessage - positionMessage - 19);
                int start = message.find_first_not_of(WHITESPACE);
                if (start != std::string::npos) message = message.substr(start);
                int end = message.find_last_not_of(WHITESPACE);
                if (end != std::string::npos) message = message.substr(0, end + 1);
                parseError = true;
            }

            if (parseError) {
                int positionCode = se.message().find("[mysql_stmt_errno]:");
                if (positionCode != std::string::npos) {
                    int endPositionCode = se.message().find("[", positionCode+19);
                    if (endPositionCode != std::string::npos) {
                        std::string codeStr = se.message().substr(positionCode+19, endPositionCode - positionCode - 19);
                        int start = codeStr.find_first_not_of(WHITESPACE);
                        if (start != std::string::npos) codeStr = codeStr.substr(start);
                        int end = codeStr.find_last_not_of(WHITESPACE);
                        if (end != std::string::npos) codeStr = codeStr.substr(0, end + 1);
                        code = std::stoi(codeStr);
                    }
                }
                else {parseError = false;}
            }
        }

        if (parseError) {result->setError(code, message);}
        else {result->setError(se.code(), se.message());}    
        return result;
    }
    catch (std::exception &e) {
        Poco::Logger::get("DatabaseConnector").error(e.what(), __FILE__, __LINE__);
    }

    if (query->isSelectQuery() || query->getCustomSQL() != "") {
        Poco::Data::RecordSet records(statement);

        for (int i = 0; i < records.rowCount(); i++) {

            result->createRow();
            Poco::Data::Row row = records.row(i);

            for (int j = 0; j < row.fieldCount(); j++) {

                std::string key = row.names()->at(j);

                if (!row.get(j).isEmpty()) {
                    std::map<std::string, std::string>::iterator it = query->getSchema().find(key);
                    if (it->second == "int")
                    {
                        int* value  = new int;
                        row.get(j).convert(*value);
                        result->addParamToLastRow(key, (void*)value);
                    }
                    else if (it->second == "double")
                    {
                        double* value  = new double;
                        row.get(j).convert(*value);
                        result->addParamToLastRow(key, (void*)value);
                    }
                    else if (it->second == "string")
                    {
                        std::string* value = new std::string(row.get(j).toString());
                        result->addParamToLastRow(key, (void*)value);
                    }
                    else if (it->second == "date")
                    {
                        std::string valueStr = row.get(j).toString();
                        //Poco::Logger::get("DatabaseConnector").debug("Date string : " + valueStr, __FILE__, __LINE__);

                        int posSep = valueStr.find('/', 0);
                        int posSep2 = valueStr.find('/', posSep+1);

                        int year = std::stoi(valueStr.substr(0, posSep));
                        int month = std::stoi(valueStr.substr(posSep+1, posSep2-posSep-1));
                        int day = std::stoi(valueStr.substr(posSep2+1));

                        //Poco::Logger::get("DatabaseConnector").debug("Date : " + std::to_string(day) + ":" + std::to_string(month) + ":" + std::to_string(year), __FILE__, __LINE__);

                        Poco::DateTime* value = new Poco::DateTime(year, month, day);
                        result->addParamToLastRow(key, (void*)value);
                    }
                }
                else {
                    result->addParamToLastRow(key, (void*)nullptr);
                }
            }
        }
    
        result->setValid();
    }

    // -- execute query to get newly created primary key
    else if (query->isInsertQuery()) {
        Poco::Data::Statement statementSelectId(*session);
        statementSelectId << "SELECT last_insert_id()";

        try {
            statementSelectId.execute();
        }
        catch (std::exception &e) {
            Poco::Logger::get("DatabaseConnector").error("Error at execution of get inserted id", __FILE__, __LINE__);
            Poco::Logger::get("DatabaseConnector").error(e.what(), __FILE__, __LINE__);
            return result;
        }

        Poco::Data::RecordSet records(statementSelectId);
        Poco::Data::Row row = records.row(0);

        int last_insert_id = -1;
        row.get(0).convert(last_insert_id);

        if (last_insert_id != -1) {
            result->setLastInsertedId(last_insert_id);
            result->setValid();
        }
        //Poco::Logger::get("DatabaseConnector").debug("last_insert_id : " + std::to_string(last_insert_id), __FILE__, __LINE__);
    }
    else if (nbAffected >= 1) {
        result->setValid();
    }
    else {
        result->setError(0, "No rows affected !");
    }
    
    return result;
}