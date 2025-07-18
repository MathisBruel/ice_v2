#include "orm/databasemanager.hpp"

#include <range/v3/view/map.hpp>

#include "orm/concerns/hasconnectionresolver.hpp"
#include "orm/connectors/connectionfactory.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

/* public */

/* This is needed because of the std::unique_ptr is used in the m_connections
   data member 😲, and when this dtor is not defined in the cpp, it will be
   generated by the compiler as inline dtor, what causes a compile error. */
DatabaseManager::~DatabaseManager() = default;

/* private */

std::shared_ptr<DatabaseManager> DatabaseManager::m_instance;

DatabaseManager::DatabaseManager(const QString &defaultConnection)
{
    Configuration::defaultConnection = defaultConnection;

    // Set up the DatabaseManager as a connection resolver
    Concerns::HasConnectionResolver::setConnectionResolver(this);

    setupDefaultReconnector();

    /* I couldn't find a better place for this, even though this place is ideal because
       it guarantees that it will be called only once per whole application. */
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    registerQMetaTypesForQt5();
#endif
}

DatabaseManager::DatabaseManager(const QVariantHash &config, const QString &connection,
                                 const QString &defaultConnection)
    : DatabaseManager(defaultConnection)
{
    addConnection(config, connection);
}

DatabaseManager::DatabaseManager(const ConfigurationsType &configs,
                                 const QString &defaultConnection)
    : DatabaseManager(defaultConnection)
{
    *m_configuration = configs;
}

DatabaseManager &DatabaseManager::setupDefaultReconnector()
{
    m_reconnector = [this](const DatabaseConnection &connection)
    {
        reconnect(connection.getName());
    };

    return *this;
}

/* public */

/* DatabaseManager factories */

std::shared_ptr<DatabaseManager>
DatabaseManager::create(const QString &defaultConnection)
{
    checkInstance();

    // Can't use the std::make_shared<> as it calls the private constructor
    return m_instance = std::shared_ptr<DatabaseManager>(
                            new DatabaseManager(defaultConnection));
}

// CUR add 'createLazy = true' paramater / or open = false?, add support to create eager connection silverqx
std::shared_ptr<DatabaseManager>
DatabaseManager::create(const QVariantHash &config, const QString &connection)
{
    checkInstance();

    // Can't use the std::make_shared<> as it calls the private constructor
    return m_instance = std::shared_ptr<DatabaseManager>(
                            new DatabaseManager(config, connection, connection));
}

std::shared_ptr<DatabaseManager>
DatabaseManager::create(const ConfigurationsType &configs,
                        const QString &defaultConnection)
{
    checkInstance();

    // Can't use the std::make_shared<> as it calls the private constructor
    return m_instance = std::shared_ptr<DatabaseManager>(
                            new DatabaseManager(configs, defaultConnection));
}

/* Proxy methods to the DatabaseConnection */

std::shared_ptr<QueryBuilder>
DatabaseManager::table(const QString &table, const QString &connection)
{
    return this->connection(connection).table(table);
}

std::shared_ptr<QueryBuilder>
DatabaseManager::tableAs(const QString &table, const QString &as,
                         const QString &connection)
{
    return this->connection(connection).table(table, as);
}

std::shared_ptr<QueryBuilder>
DatabaseManager::query(const QString &connection)
{
    return this->connection(connection).query();
}

TSqlQuery DatabaseManager::qtQuery(const QString &connection)
{
    return this->connection(connection).getQtQuery();
}

SqlQuery
DatabaseManager::select(const QString &query, QVector<QVariant> bindings,
                        const QString &connection)
{
    return this->connection(connection).select(query, std::move(bindings));
}

SqlQuery
DatabaseManager::selectFromWriteConnection(
        const QString &query, QVector<QVariant> bindings,
        const QString &connection)
{
    return this->connection(connection)
                .selectFromWriteConnection(query, std::move(bindings));
}

SqlQuery
DatabaseManager::selectOne(const QString &query, QVector<QVariant> bindings,
                           const QString &connection)
{
    return this->connection(connection).selectOne(query, std::move(bindings));
}

QVariant
DatabaseManager::scalar(const QString &query, QVector<QVariant> bindings,
                        const QString &connection)
{
    return this->connection(connection).scalar(query, std::move(bindings));
}

SqlQuery
DatabaseManager::insert(const QString &query, QVector<QVariant> bindings,
                        const QString &connection)
{
    return this->connection(connection).insert(query, std::move(bindings));
}

std::tuple<int, TSqlQuery>
DatabaseManager::update(const QString &query, QVector<QVariant> bindings,
                        const QString &connection)
{
    return this->connection(connection).update(query, std::move(bindings));
}

std::tuple<int, TSqlQuery>
DatabaseManager::remove(const QString &query, QVector<QVariant> bindings,
                        const QString &connection)
{
    return this->connection(connection).remove(query, std::move(bindings));
}

SqlQuery
DatabaseManager::statement(const QString &query, QVector<QVariant> bindings,
                           const QString &connection)
{
    return this->connection(connection).statement(query, std::move(bindings));
}

std::tuple<int, TSqlQuery>
DatabaseManager::affectingStatement(const QString &query, QVector<QVariant> bindings,
                                    const QString &connection)
{
    return this->connection(connection).affectingStatement(query, std::move(bindings));
}

SqlQuery DatabaseManager::unprepared(const QString &query, const QString &connection)
{
    return this->connection(connection).unprepared(query);
}

bool DatabaseManager::beginTransaction(const QString &connection)
{
    return this->connection(connection).beginTransaction();
}

bool DatabaseManager::commit(const QString &connection)
{
    return this->connection(connection).commit();
}

bool DatabaseManager::rollBack(const QString &connection)
{
    return this->connection(connection).rollBack();
}

bool DatabaseManager::savepoint(const QString &id, const QString &connection)
{
    return this->connection(connection).savepoint(id);
}

bool DatabaseManager::savepoint(const std::size_t id, const QString &connection)
{
    return this->connection(connection).savepoint(id);
}

bool DatabaseManager::rollbackToSavepoint(const QString &id, const QString &connection)
{
    return this->connection(connection).rollbackToSavepoint(id);
}

bool DatabaseManager::rollbackToSavepoint(const std::size_t id, const QString &connection)
{
    return this->connection(connection).rollbackToSavepoint(id);
}

size_t DatabaseManager::transactionLevel(const QString &connection)
{
    return this->connection(connection).transactionLevel();
}

bool DatabaseManager::isOpen(const QString &connection)
{
    return this->connection(connection).isOpen();
}

bool DatabaseManager::pingDatabase(const QString &connection)
{
    return this->connection(connection).pingDatabase();
}

/* The TinyDrivers uses smart pointers everywhere and because of this the driverWeak()
   method exists. The reason why the TSqlDriver *driver() also exists is because that's
   how the QtSql module works so it can't be removed.
   So if the TinyOrm library is linked against the QtSql module, only the driver() method
   will be defined. If the TinyOrm library is linked against the TinyDrivers library,
   both methods will be defined. */

const TSqlDriver *DatabaseManager::driver(const QString &connection)
{
    return this->connection(connection).driver();
}

#ifdef TINYORM_USING_TINYDRIVERS
std::weak_ptr<const TSqlDriver> DatabaseManager::driverWeak(const QString &connection)
{
    return this->connection(connection).driverWeak();
}
#endif

/* DatabaseManager */

namespace
{
    const auto *const InstanceExceptionMessage =
            "The DatabaseManager instance has not yet been created, create it "
            "by the DB::create() method.";
} // namespace

std::shared_ptr<DatabaseManager> DatabaseManager::instance()
{
    if (m_instance)
        return m_instance;

    throw Exceptions::RuntimeError(InstanceExceptionMessage);
}

DatabaseManager &DatabaseManager::reference()
{
    if (m_instance)
        return *m_instance;

    throw Exceptions::RuntimeError(InstanceExceptionMessage);
}

DatabaseConnection &DatabaseManager::connection(const QString &name) // NOLINT(google-default-arguments)
{
    const auto &connectionName = parseConnectionName(name);

    /* If we haven't created this connection, we'll create it based on the provided
       config. Once we've created the connections we will configure it. */
    if (!m_connections->contains(connectionName))
        m_connections->emplace(connectionName,
                               configure(makeConnection(connectionName)));

    return *(*m_connections)[connectionName];
}

DatabaseManager &
DatabaseManager::addConnection(const QVariantHash &config, const QString &name)
{
    if (m_configuration->contains(name))
        throw Exceptions::InvalidArgumentError(
                QStringLiteral("The database connection '%1' already exists.")
                .arg(name));

    m_configuration->emplace(name, config);

    return *this;
}

DatabaseManager &
DatabaseManager::addConnections(const ConfigurationsType &configs)
{
    for (const auto &[name, config] : configs)
        addConnection(config, name);

    return *this;
}

DatabaseManager &
DatabaseManager::addConnections(const ConfigurationsType &configs,
                                const QString &defaultConnection)
{
    addConnections(configs);

    Configuration::defaultConnection = defaultConnection;

    return *this;
}

bool DatabaseManager::removeConnection(const QString &name)
{
    const auto &name_ = parseConnectionName(name);

    // Connection with this name doesn't exist
    if (!connectionNames().contains(name_))
        return false;

    /* If currently removed connection is the default connection, then reset default
       connection. */
    const auto resetDefaultConnection_ = [this, &name]
    {
        if (Configuration::defaultConnection == name)
            resetDefaultConnection();
    };

    // Not connected
    if (!m_connections->contains(name_)) {
        m_configuration->erase(name_);
        resetDefaultConnection_();
        return true;
    }

    // Disconnect first to be nice 😁 and safe 😂
    m_connections->find(name_)->second->disconnect();

    /* If connection was not removed, return false and don't remove Qt's database
       connection and also don't remove connection configuration. */
    if (m_connections->erase(name_) == 0)
        return false;

    // Remove TinyORM configuration
    m_configuration->erase(name_);
    // Remove Qt's database connection, ~QSqlDatabase() internally also calls close()
    TSqlDatabase::removeDatabase(name_);

    resetDefaultConnection_();

    return true;
}

bool DatabaseManager::containsConnection(const QString &name)
{
    return m_connections->contains(name);
}

DatabaseConnection &DatabaseManager::reconnect(const QString &name)
{
    const auto &name_ = parseConnectionName(name);

    disconnect(name_);

    if (!m_connections->contains(name_))
        return connection(name_);

    return refreshQtConnection(name_);
}

void DatabaseManager::disconnect(const QString &name) const
{
    const auto &name_ = parseConnectionName(name);

    if (!m_connections->contains(name_))
        return;

    m_connections->find(name_)->second->disconnect();
}

void DatabaseManager::connectEagerly(const QString &name)
{
    connection(name).connectEagerly();
}

QStringList DatabaseManager::connectionNames() const
{
    return *m_configuration | ranges::views::keys | ranges::to<QStringList>();
}

QStringList DatabaseManager::openedConnectionNames() const
{
    QStringList names;
    names.reserve(static_cast<decltype (names)::size_type>(m_connections->size()));

    for (const auto &connection : *m_connections)
        names << connection.first;

    return names;
}

std::size_t DatabaseManager::openedConnectionsSize() const noexcept
{
    return m_connections->size();
}

QStringList DatabaseManager::supportedDrivers() const // NOLINT(readability-convert-member-functions-to-static)
{
#ifdef TINYORM_USING_QTSQLDRIVERS
    // aaaaaaaaaaaaaachjo 🤔😁 -- 4 months later, looks much better, right?
    return {QMYSQL, QPSQL, QSQLITE};
#elif defined(TINYORM_USING_TINYDRIVERS)
    // aaaaaaaaaaaaaachjo 🤔
    return {QMYSQL/*, QPSQL, QSQLITE*/};
#else
#  error Missing include "orm/macros/sqldrivermappings.hpp".
#endif
}

QStringList DatabaseManager::drivers() const // NOLINT(readability-convert-member-functions-to-static)
{
    return TSqlDatabase::drivers();
}

bool DatabaseManager::isDriverAvailable(const QString &driverName) const // NOLINT(readability-convert-member-functions-to-static)
{
    return TSqlDatabase::isDriverAvailable(driverName);
}

bool DatabaseManager::isConnectionDriverAvailable(const QString &connection)
{
    const auto driverName = configuration(connection)[driver_].value<QString>();

    if (!supportedDrivers().contains(driverName))
        throw Exceptions::LogicError(
                QStringLiteral("An unsupported driver name '%1' has been defined for "
                               "the '%2' connection.")
                .arg(driverName, connection));

    return TSqlDatabase::isDriverAvailable(driverName);
}

const QString &
DatabaseManager::getDefaultConnection() const noexcept
{
    return Configuration::defaultConnection;
}

void DatabaseManager::setDefaultConnection(const QString &defaultConnection)
{
    Configuration::defaultConnection = defaultConnection;
}

void DatabaseManager::resetDefaultConnection()
{
    Configuration::defaultConnection = Configuration::defaultConnectionName;
}

DatabaseManager &
DatabaseManager::setReconnector(const ReconnectorType &reconnector)
{
    m_reconnector = reconnector;

    return *this;
}

/* Getters / Setters */

QString DatabaseManager::driverName(const QString &connection)
{
    return this->connection(connection).driverName();
}

const QString &
DatabaseManager::driverNamePrintable(const QString &connection)
{
    return this->connection(connection).driverNamePrintable();
}

const QString &
DatabaseManager::databaseName(const QString &connection)
{
    return this->connection(connection).getDatabaseName();
}

const QString &
DatabaseManager::hostName(const QString &connection)
{
    return this->connection(connection).getHostName();
}

const QtTimeZoneConfig &
DatabaseManager::qtTimeZone(const QString &connection)
{
    return this->connection(connection).getQtTimeZone();
}

DatabaseConnection &
DatabaseManager::setQtTimeZone(const QVariant &timezone, const QString &connection)
{
    return this->connection(connection).setQtTimeZone(timezone);
}

DatabaseConnection &
DatabaseManager::setQtTimeZone(QtTimeZoneConfig &&timezone, const QString &connection)
{
    return this->connection(connection).setQtTimeZone(std::move(timezone));
}

bool DatabaseManager::isConvertingTimeZone(const QString &connection)
{
    return this->connection(connection).isConvertingTimeZone();
}

/* Connection configurations - saved in the DatabaseManager */

QVariant
DatabaseManager::originalConfigValue(const QString &option,
                                     const QString &connection) const
{
    return originalConfig(connection).value(option);
}

const QVariantHash &
DatabaseManager::originalConfig(const QString &connection) const
{
    const auto &connectionName = parseConnectionName(connection);

    throwIfNoConfiguration(connectionName);

    return m_configuration->at(connectionName);
}

size_t DatabaseManager::originalConfigsSize() const
{
    return m_configuration->size();
}

/* Connection configurations - proxies to the DatabaseConnection */

QVariant DatabaseManager::getConfigValue(const QString &option, const QString &connection)
{
    return this->connection(connection).getConfig(option);
}

const QVariantHash &DatabaseManager::getConfig(const QString &connection)
{
    return this->connection(connection).getConfig();
}

bool DatabaseManager::hasConfigValue(const QString &option, const QString &connection)
{
    return this->connection(connection).hasConfig(option);
}

/* Pretending */

QVector<Log>
DatabaseManager::pretend(const std::function<void()> &callback, const QString &connection)
{
    return this->connection(connection).pretend(callback);
}

QVector<Log>
DatabaseManager::pretend(const std::function<void(DatabaseConnection &)> &callback,
                         const QString &connection)
{
    return this->connection(connection).pretend(callback);
}

/* Records were modified */

bool DatabaseManager::getRecordsHaveBeenModified(const QString &connection)
{
    return this->connection(connection).getRecordsHaveBeenModified();
}

void DatabaseManager::recordsHaveBeenModified(const bool value,
                                              const QString &connection)
{
    this->connection(connection).recordsHaveBeenModified(value);
}

void DatabaseManager::forgetRecordModificationState(const QString &connection)
{
    this->connection(connection).forgetRecordModificationState();
}

/* Logging */

std::shared_ptr<QVector<Log>>
DatabaseManager::getQueryLog(const QString &connection)
{
    return this->connection(connection).getQueryLog();
}

void DatabaseManager::flushQueryLog(const QString &connection)
{
    this->connection(connection).flushQueryLog();
}

void DatabaseManager::enableQueryLog(const QString &connection)
{
    this->connection(connection).enableQueryLog();
}

void DatabaseManager::disableQueryLog(const QString &connection)
{
    this->connection(connection).disableQueryLog();
}

bool DatabaseManager::logging(const QString &connection)
{
    return this->connection(connection).logging();
}

std::size_t DatabaseManager::getQueryLogOrder() const noexcept // NOLINT(readability-convert-member-functions-to-static)
{
    return DatabaseConnection::getQueryLogOrder();
}

/* Queries execution time counter */

bool DatabaseManager::countingElapsed(const QString &connection)
{
    return this->connection(connection).countingElapsed();
}

DatabaseConnection &DatabaseManager::enableElapsedCounter(const QString &connection)
{
    return this->connection(connection).enableElapsedCounter();
}

DatabaseConnection &DatabaseManager::disableElapsedCounter(const QString &connection)
{
    return this->connection(connection).disableElapsedCounter();
}

qint64 DatabaseManager::getElapsedCounter(const QString &connection)
{
    return this->connection(connection).getElapsedCounter();
}

qint64 DatabaseManager::takeElapsedCounter(const QString &connection)
{
    return this->connection(connection).takeElapsedCounter();
}

DatabaseConnection &DatabaseManager::resetElapsedCounter(const QString &connection)
{
    return this->connection(connection).resetElapsedCounter();
}

bool DatabaseManager::anyCountingElapsed()
{
    return std::ranges::any_of(openedConnectionNames(),
                               [this](const auto &connectionName)
    {
        return connection(connectionName).countingElapsed();
    });
}

void DatabaseManager::enableAllElapsedCounters()
{
    enableElapsedCounters(openedConnectionNames());
}

void DatabaseManager::disableAllElapsedCounters()
{
    disableElapsedCounters(openedConnectionNames());
}

qint64 DatabaseManager::getAllElapsedCounters()
{
    return getElapsedCounters(openedConnectionNames());
}

qint64 DatabaseManager::takeAllElapsedCounters()
{
    return takeElapsedCounters(openedConnectionNames());
}

void DatabaseManager::resetAllElapsedCounters()
{
    resetElapsedCounters(openedConnectionNames());
}

void DatabaseManager::enableElapsedCounters(const QStringList &connections)
{
    for (const auto &connectionName : connections)
        connection(connectionName).enableElapsedCounter();
}

void DatabaseManager::disableElapsedCounters(const QStringList &connections)
{
    for (const auto &connectionName : connections)
        connection(connectionName).disableElapsedCounter();
}

qint64 DatabaseManager::getElapsedCounters(const QStringList &connections)
{
    if (!anyCountingElapsed())
        return -1;

    qint64 elapsed = 0;

    for (const auto &connectionName : connections) {
        const auto &connection = this->connection(connectionName);

        if (connection.countingElapsed())
            elapsed += connection.getElapsedCounter();
    }

    return elapsed;
}

qint64 DatabaseManager::takeElapsedCounters(const QStringList &connections)
{
    if (!anyCountingElapsed())
        return -1;

    qint64 elapsed = 0;

    for (const auto &connectionName : connections) {
        auto &connection = this->connection(connectionName);

        if (connection.countingElapsed())
            elapsed += connection.takeElapsedCounter();
    }

    return elapsed;
}

void DatabaseManager::resetElapsedCounters(const QStringList &connections)
{
    for (const auto &connectionName : connections) {
        auto &connection = this->connection(connectionName);

        if (connection.countingElapsed())
            connection.resetElapsedCounter();
    }
}

/* Queries executed counter */

bool DatabaseManager::countingStatements(const QString &connection)
{
    return this->connection(connection).countingStatements();
}

DatabaseConnection &DatabaseManager::enableStatementsCounter(const QString &connection)
{
    return this->connection(connection).enableStatementsCounter();
}

DatabaseConnection &DatabaseManager::disableStatementsCounter(const QString &connection)
{
    return this->connection(connection).disableStatementsCounter();
}

const StatementsCounter &DatabaseManager::getStatementsCounter(const QString &connection)
{
    return this->connection(connection).getStatementsCounter();
}

StatementsCounter DatabaseManager::takeStatementsCounter(const QString &connection)
{
    return this->connection(connection).takeStatementsCounter();
}

DatabaseConnection &DatabaseManager::resetStatementsCounter(const QString &connection)
{
    return this->connection(connection).resetStatementsCounter();
}

bool DatabaseManager::anyCountingStatements()
{
    return std::ranges::any_of(openedConnectionNames(),
                               [this](const auto &connectionName)
    {
        return connection(connectionName).countingStatements();
    });
}

void DatabaseManager::enableAllStatementCounters()
{
    enableStatementCounters(openedConnectionNames());
}

void DatabaseManager::disableAllStatementCounters()
{
    disableStatementCounters(openedConnectionNames());
}

StatementsCounter DatabaseManager::getAllStatementCounters()
{
    return getStatementCounters(openedConnectionNames());
}

StatementsCounter DatabaseManager::takeAllStatementCounters()
{
    return takeStatementCounters(openedConnectionNames());
}

void DatabaseManager::resetAllStatementCounters()
{
    resetStatementCounters(openedConnectionNames());
}

void DatabaseManager::enableStatementCounters(const QStringList &connections)
{
    for (const auto &connectionName : connections)
        connection(connectionName).enableStatementsCounter();
}

void DatabaseManager::disableStatementCounters(const QStringList &connections)
{
    for (const auto &connectionName : connections)
        connection(connectionName).disableStatementsCounter();
}

StatementsCounter DatabaseManager::getStatementCounters(const QStringList &connections)
{
    StatementsCounter counter;

    if (!anyCountingStatements())
        return counter;

    for (const auto &connectionName : connections) {
        const auto &connection = this->connection(connectionName);

        if (connection.countingStatements()) {
            const auto &counter_ = connection.getStatementsCounter();

            counter.normal        += counter_.normal;
            counter.affecting     += counter_.affecting;
            counter.transactional += counter_.transactional;
        }
    }

    return counter;
}

StatementsCounter DatabaseManager::takeStatementCounters(const QStringList &connections)
{
    StatementsCounter counter;

    if (!anyCountingStatements())
        return counter;

    for (const auto &connectionName : connections) {
        auto &connection = this->connection(connectionName);

        if (connection.countingElapsed()) {
            const auto counter_ = connection.takeStatementsCounter();

            counter.normal        += counter_.normal;
            counter.affecting     += counter_.affecting;
            counter.transactional += counter_.transactional;
        }
    }

    return counter;
}

void DatabaseManager::resetStatementCounters(const QStringList &connections)
{
    for (const auto &connectionName : connections) {
        auto &connection = this->connection(connectionName);

        if (connection.countingElapsed())
            connection.resetStatementsCounter();
    }
}

/* private */

const QString &
DatabaseManager::parseConnectionName(const QString &name) const
{
    return name.isEmpty() ? getDefaultConnection() : name;
}

std::shared_ptr<DatabaseConnection>
DatabaseManager::makeConnection(const QString &connection)
{
    auto &config = configuration(connection);

    // FUTURE add support for extensions silverqx

    return Connectors::ConnectionFactory::make(config, connection);
}

/* Can not be const because I'm modifying the Configuration (QVariantHash)
   in ConnectionFactory. */
QVariantHash &
DatabaseManager::configuration(const QString &connection)
{
    const auto &connectionName = parseConnectionName(connection);

    throwIfNoConfiguration(connectionName);

    return (*m_configuration)[connectionName]; // clazy:exclude=detaching-member

    // TODO add ConfigurationUrlParser silverqx
//    return ConfigurationUrlParser()
//            .parseConfiguration((*m_configuration)[connectionName]);
}

void DatabaseManager::throwIfNoConfiguration(const QString &connection) const
{
    /* Get the database connection configuration by the given name.
       If the configuration doesn't exist, we'll throw an exception and bail. */
    if (m_configuration->contains(connection))
        return;

    throw Exceptions::InvalidArgumentError(
                QStringLiteral("Database connection '%1' is not configured.")
                .arg(connection));
}

std::shared_ptr<DatabaseConnection>
DatabaseManager::configure(std::shared_ptr<DatabaseConnection> &&connection) const
{
    /* Here we'll set a reconnector lambda. This reconnector can be any callable
       so we will set a Closure to reconnect from this manager with the name of
       the connection, which will allow us to reconnect from OUR connections. */
    connection->setReconnector(m_reconnector);

    return std::move(connection);
}

DatabaseConnection &
DatabaseManager::refreshQtConnection(const QString &connection)
{
    const auto &connectionName = parseConnectionName(connection);

    /* Make OUR new connection and copy the connection resolver from this new
       connection to the current connection, this ensures that the connection
       will be again resolved/connected lazily. */
    auto fresh = configure(makeConnection(connectionName));

    return (*m_connections)[connectionName]->setQtConnectionResolver(
                fresh->getQtConnectionResolver());
}

void DatabaseManager::checkInstance()
{
    if (!m_instance)
        return;

    throw Exceptions::RuntimeError(
            "Only one instance of DatabaseManager is allowed per process.");
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void DatabaseManager::registerQMetaTypesForQt5()
{
    if (!QMetaType::hasRegisteredComparators<QtTimeZoneConfig>())
        QMetaType::registerEqualsComparator<QtTimeZoneConfig>();

    /* Comparing serialized models with the toMap() and toVector() */
    if (!QMetaType::hasRegisteredComparators<QVector<QVariantMap>>())
        QMetaType::registerEqualsComparator<QVector<QVariantMap>>();

#ifndef TINYORM_DISABLE_ORM
    using Tiny::AttributeItem;

    if (!QMetaType::hasRegisteredComparators<AttributeItem>())
        QMetaType::registerEqualsComparator<AttributeItem>();

    if (!QMetaType::hasRegisteredComparators<QVector<AttributeItem>>())
        QMetaType::registerEqualsComparator<QVector<AttributeItem>>();

    if (!QMetaType::hasRegisteredComparators<QVector<QVector<AttributeItem>>>())
        QMetaType::registerEqualsComparator<QVector<QVector<AttributeItem>>>();
#endif
}
#endif

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

// FUTURE add support for ::read and ::write db connections silverx
// TODO implement RepositoryFactory silverqx
