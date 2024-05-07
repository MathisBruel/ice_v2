#include <orm/db.hpp>

#include <tom/application.hpp>

#include "migrations/2024_05_03_902300_create_tables.hpp"

using Orm::DatabaseManager;
using Orm::DB;

using TomApplication = Tom::Application;

using namespace Migrations; // NOLINT(google-build-using-namespace)

/*! Create the database manager instance and add a database connection. */
std::shared_ptr<DatabaseManager> setupDatabaseManager();

/*! C++ main function. */
int main(int argc, char *argv[])
{
    try {
        // Ownership of the shared_ptr()
        auto db = setupDatabaseManager();

        return TomApplication(argc, argv, std::move(db), "ICE_MIGRATION")
                .migrations<CreatePostsTable>()   /*Pour rajouter une migration il faut l'ajouter entre les <> avec une virgule ex: .migrations<CreatePostsTable,CreatePostsTable>() */
                .seeders<>()
                .run();

    } catch (const std::exception &e) {

        TomApplication::logException(e);
    }

    return EXIT_FAILURE;
}

std::shared_ptr<DatabaseManager> setupDatabaseManager()
{
    using namespace Orm::Constants; // NOLINT(google-build-using-namespace)

    // Ownership of the shared_ptr()
    return DB::create({
        {driver_,     QMYSQL},
        {"unix_socket",    qEnvironmentVariable("DB_MYSQL_SOCKET", "/run/mysqld/mysqld.sock")},
        {database_,   qEnvironmentVariable("DB_MYSQL_DATABASE", "ice")},
        {username_,   qEnvironmentVariable("DB_MYSQL_USERNAME", "root")},
        {password_,   qEnvironmentVariable("DB_MYSQL_PASSWORD", "admin")},
        {charset_,    qEnvironmentVariable("DB_MYSQL_CHARSET", UTF8MB4)},
        {collation_,  qEnvironmentVariable("DB_MYSQL_COLLATION", UTF8MB40900aici)},
        {timezone_,   TZ00},
        /* Specifies what time zone all QDateTime-s will have, the overridden default is
           the Qt::UTC, set to the Qt::LocalTime or QtTimeZoneType::DontConvert to use
           the system local time. */
        {qt_timezone, QVariant::fromValue(Qt::UTC)},
    },
        QStringLiteral("ICE_MIGRATION_mysql")); // shell:connection
}