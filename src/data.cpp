#include "data.hpp"
#include <memory>


DataSource::DataSource()
    :db(QSqlDatabase::addDatabase("QSQLITE"))
{
}

std::unique_ptr<DataSource> DataSource::create(const char* db_path)
{
    auto data = std::make_unique<DataSource>();
    data->db.setDatabaseName("data.db");
    if(!data->db.open()) return nullptr;

    QSqlQuery query(data->db);
    bool result = query.exec(
        "CREATE TABLE IF NOT EXISTS Saves "
        "(id INTEGER PRIMARY KEY AUTOINCREMENT, description TEXT,"
        " time INTEGER DEFAULT 0, file TEXT);");
    if(!result) return nullptr;

    return data;
}
