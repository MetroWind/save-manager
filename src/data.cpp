#include <memory>

#include <QSqlDatabase>
#include <QSqlQuery>

#include "data.hpp"

DataSource::DataSource()
    :db(QSqlDatabase::addDatabase("QSQLITE"))
{
}

DataSource::~DataSource()
{
}

std::unique_ptr<DataSource> DataSource::create(const char* db_path)
{
    auto data = std::make_unique<DataSource>();
    data->db.setDatabaseName(db_path);
    if(!data->db.open())
    {
        return nullptr;
    }

    QSqlQuery query(data->db);
    bool result = query.exec(
        "CREATE TABLE IF NOT EXISTS Saves "
        "(id INTEGER PRIMARY KEY AUTOINCREMENT, description TEXT,"
        " time INTEGER DEFAULT 0, file TEXT);");
    if(!result) return nullptr;

    return data;
}
