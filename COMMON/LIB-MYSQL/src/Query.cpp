#include "Query.h"

Query::Query(TypeQuery type, std::string database, std::string table)
{
    this->type = type;
    this->database = database;
    this->table = table;
}
Query::~Query()
{

}

void Query::addParameter(std::string name, void* value, std::string type)
{
    schemaParameter.insert_or_assign(name, type);
    valueParameter.insert_or_assign(name, value);
}
void Query::addWhereParameter(std::string name, void* value, std::string type)
{
    schemaWhereParameter.insert_or_assign(name, type);
    valueWhereParameter.insert_or_assign(name, value);
}

std::string Query::getQueryString()
{
    std::string query = "";

    // -- format : SELECT field1, field2, ... fieldN FROM table_name [WHERE Clause]
    if (type == SELECT) {
        query += "SELECT ";
        
        // -- add fields
        std::map<std::string, std::string>::iterator itSchema;
        if (schemaParameter.size() == 0) {
            query += "*";
        }
        else {
            for (itSchema = schemaParameter.begin(); itSchema != schemaParameter.end(); itSchema++) {
                query += itSchema->first + ", ";
            }
            query = query.substr(0, query.size()-2);
        }

        // -- add database and table
        query += " FROM " + database + "." + table;

        if (schemaWhereParameter.size() > 0) {

            query += " WHERE (";
            for (itSchema = schemaWhereParameter.begin(); itSchema != schemaWhereParameter.end(); itSchema++) {
                query += itSchema->first + " = ";
                void* value = valueWhereParameter.find(itSchema->first)->second;
                if (itSchema->second == "string") {
                    std::string* valueString = static_cast<std::string*>(value);
                    query += "\"" + *valueString + "\"";
                }
                else if (itSchema->second == "int") {
                    int* valueInt = static_cast<int*>(value);
                    query += std::to_string(*valueInt);
                }
                else if (itSchema->second == "double") {
                    double* valueDouble = static_cast<double*>(value);
                    query += std::to_string(*valueDouble);
                }
                else if (itSchema->second == "date") {
                    Poco::DateTime* valueDate = static_cast<Poco::DateTime*>(value);
                    query += "'" + std::to_string(valueDate->year()) + "-" + std::to_string(valueDate->month()) + "-" + std::to_string(valueDate->day()) + "'";
                }
                else if (itSchema->second == "null") {
                    Poco::DateTime* valueDate = static_cast<Poco::DateTime*>(value);
                    query += "NULL";
                }

                query += " AND ";
            }
            query = query.substr(0, query.size()-5);
            query += ")";
        }
    }
    
    // -- format : INSERT INTO table_name ( field1, field2,...fieldN ) VALUES ( value1, value2,...valueN );
    else if (type == INSERT) {
        query += "INSERT INTO " + database + "." + table + "(";

        // -- add fields
        std::map<std::string, std::string>::iterator itSchema;
        for (itSchema = schemaParameter.begin(); itSchema != schemaParameter.end(); itSchema++) {
            query += itSchema->first + ", ";
        }
        query = query.substr(0, query.size()-2);
        query += ") VALUES (";

        // -- add values
        for (itSchema = schemaParameter.begin(); itSchema != schemaParameter.end(); itSchema++) {
            void* value = valueParameter.find(itSchema->first)->second;
            if (itSchema->second == "string") {
                std::string* valueString = static_cast<std::string*>(value);
                //Poco::Logger::get("Query").debug("INSERT request value : " + *valueString, __FILE__, __LINE__);
                query += "\"" + *valueString + "\", ";
            }
            else if (itSchema->second == "int") {
                int* valueInt = static_cast<int*>(value);
                query += std::to_string(*valueInt) + ", ";
            }
            else if (itSchema->second == "double") {
                double* valueDouble = static_cast<double*>(value);
                query += std::to_string(*valueDouble) + ", ";
            }
            else if (itSchema->second == "date") {
                Poco::DateTime* valueDate = static_cast<Poco::DateTime*>(value);
                query += "'" + std::to_string(valueDate->year()) + "-" + std::to_string(valueDate->month()) + "-" + std::to_string(valueDate->day()) + "', ";
            }
            else if (itSchema->second == "null") {
                query += "NULL, ";
            }
        }

        query = query.substr(0, query.size()-2);
        query += ")";
    }
    
    // -- format : UPDATE table_name SET field1 = new-value1, field2 = new-value2 [WHERE Clause]
    else if (type == UPDATE) {
        query += "UPDATE " + database + "." + table + " SET ";

        // -- add values
        std::map<std::string, std::string>::iterator itSchema;
        for (itSchema = schemaParameter.begin(); itSchema != schemaParameter.end(); itSchema++) {

            query += itSchema->first + " = ";
            void* value = valueParameter.find(itSchema->first)->second;
            if (itSchema->second == "string") {
                std::string* valueString = static_cast<std::string*>(value);
                query += "\"" + *valueString + "\", ";
            }
            else if (itSchema->second == "int") {
                int* valueInt = static_cast<int*>(value);
                query += std::to_string(*valueInt) + ", ";
            }
            else if (itSchema->second == "double") {
                double* valueDouble = static_cast<double*>(value);
                query += std::to_string(*valueDouble) + ", ";
            }
            else if (itSchema->second == "date") {
                Poco::DateTime* valueDate = static_cast<Poco::DateTime*>(value);
                query += "'" + std::to_string(valueDate->year()) + "-" + std::to_string(valueDate->month()) + "-" + std::to_string(valueDate->day()) + "', ";
            }
            else if (itSchema->second == "null") {
                query += "NULL, ";
            }
        }

        query = query.substr(0, query.size()-2);

        if (schemaWhereParameter.size() > 0) {

            query += " WHERE (";
            for (itSchema = schemaWhereParameter.begin(); itSchema != schemaWhereParameter.end(); itSchema++) {
                query += itSchema->first + " = ";
                void* value = valueWhereParameter.find(itSchema->first)->second;
                if (itSchema->second == "string") {
                    std::string* valueString = static_cast<std::string*>(value);
                    query += "\"" + *valueString + "\"";
                }
                else if (itSchema->second == "int") {
                    int* valueInt = static_cast<int*>(value);
                    query += std::to_string(*valueInt);
                }
                else if (itSchema->second == "double") {
                    double* valueDouble = static_cast<double*>(value);
                    query += std::to_string(*valueDouble);
                }
                else if (itSchema->second == "date") {
                    Poco::DateTime* valueDate = static_cast<Poco::DateTime*>(value);
                    query += "'" + std::to_string(valueDate->year()) + "-" + std::to_string(valueDate->month()) + "-" + std::to_string(valueDate->day()) + "'";
                }
                else if (itSchema->second == "null") {
                    query += "NULL";
                }
                query += " AND ";
            }
        }
        query = query.substr(0, query.size()-5);
        query += ")";
    }
    
    // -- format : DELETE FROM table_name [WHERE Clause]
    else if (type == REMOVE) {
        query += "DELETE FROM " + database + "." + table;

        // -- add clause
        if (schemaWhereParameter.size() == 0) {
            Poco::Logger::get("Query").error("Error generation query : can't have no condition !", __FILE__, __LINE__);
            return "";
        }
        else {
            query += " WHERE (";
            std::map<std::string, std::string>::iterator itWhereSchema;
            for (itWhereSchema = schemaWhereParameter.begin(); itWhereSchema != schemaWhereParameter.end(); itWhereSchema++) {
                query += itWhereSchema->first + " = ";
                void* value = valueWhereParameter.find(itWhereSchema->first)->second;
                if (itWhereSchema->second == "string") {
                    std::string* valueString = static_cast<std::string*>(value);
                    query += "\"" + *valueString + "\"";
                }
                else if (itWhereSchema->second == "int") {
                    int* valueInt = static_cast<int*>(value);
                    query += std::to_string(*valueInt);
                }
                else if (itWhereSchema->second == "double") {
                    double* valueDouble = static_cast<double*>(value);
                    query += std::to_string(*valueDouble);
                }
                else if (itWhereSchema->second == "date") {
                    Poco::DateTime* valueDate = static_cast<Poco::DateTime*>(value);
                    query += "'" + std::to_string(valueDate->year()) + "-" + std::to_string(valueDate->month()) + "-" + std::to_string(valueDate->day()) + "'";
                }
                else if (itWhereSchema->second == "null") {
                    query += "NULL";
                }
                query += " AND ";
            }
        }
        query = query.substr(0, query.size()-5);
        query += ")";
    }
    
    // -- format : Requête SQL personnalisée
    else if (type == CUSTOM) {
        if (!customSQL.empty()) {
            query = customSQL;
            
            std::map<std::string, std::string>::iterator itSchema;
            for (itSchema = schemaParameter.begin(); itSchema != schemaParameter.end(); itSchema++) {
                size_t pos = query.find("?");
                if (pos != std::string::npos) {
                    void* value = valueParameter.find(itSchema->first)->second;
                    std::string replacement;
                    
                    if (itSchema->second == "string") {
                        std::string* valueString = static_cast<std::string*>(value);
                        replacement = "\"" + *valueString + "\"";
                    }
                    else if (itSchema->second == "int") {
                        int* valueInt = static_cast<int*>(value);
                        replacement = std::to_string(*valueInt);
                    }
                    else if (itSchema->second == "double") {
                        double* valueDouble = static_cast<double*>(value);
                        replacement = std::to_string(*valueDouble);
                    }
                    else if (itSchema->second == "date") {
                        Poco::DateTime* valueDate = static_cast<Poco::DateTime*>(value);
                        replacement = "'" + std::to_string(valueDate->year()) + "-" + std::to_string(valueDate->month()) + "-" + std::to_string(valueDate->day()) + "'";
                    }
                    else if (itSchema->second == "null") {
                        replacement = "NULL";
                    }
                    
                    query.replace(pos, 1, replacement);
                }
            }
        }
    }

    return query;
}