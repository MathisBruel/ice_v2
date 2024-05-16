#include <orm/db.hpp>

#include <tom/application.hpp>

#include "migrations/2024_05_03_902300_create_tables.hpp"

using Orm::DatabaseManager;
using Orm::DB;

using TomApplication = Tom::Application;

using namespace Migrations;

std::shared_ptr<DatabaseManager> setupDatabaseManager();

int main(int argc, char *argv[])
{
    try {
        auto db = setupDatabaseManager();

        return TomApplication(argc, argv, std::move(db), "ICE_MIGRATION")
                .migrations<CreatePostsTable>()
                .seeders<>()
                .run();

    } catch (const std::exception &e) {
        TomApplication::logException(e);
    }

    return EXIT_FAILURE;
}

std::shared_ptr<DatabaseManager> setupDatabaseManager()
{
    using namespace Orm::Constants;

    return DB::create({
        {driver_,     QMYSQL},
        {"unix_socket",    qEnvironmentVariable("DB_MYSQL_SOCKET", "/run/mysqld/mysqld.sock")},
        {database_,   qEnvironmentVariable("DB_MYSQL_DATABASE", "ice")},
        {username_,   qEnvironmentVariable("DB_MYSQL_USERNAME", "root")},
        {password_,   qEnvironmentVariable("DB_MYSQL_PASSWORD", "admin")},
        {charset_,    qEnvironmentVariable("DB_MYSQL_CHARSET", UTF8MB4)},
        {collation_,  qEnvironmentVariable("DB_MYSQL_COLLATION", UTF8MB40900aici)},
        {timezone_,   TZ00},
        {qt_timezone, QVariant::fromValue(Qt::UTC)},
    },
        QStringLiteral("ICE_MIGRATION_mysql"));
}