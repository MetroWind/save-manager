#include <memory>

#include <windows.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>

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
    QMessageBox msgBox;
    msgBox.setText("Creating data source...");
    msgBox.exec();

    auto data = std::make_unique<DataSource>();
    data->db.setDatabaseName(db_path);
    MessageBox( 0, L"About to open db...", L"Info", MB_SETFOREGROUND );
    if(!data->db.open())
    {
        MessageBox( 0, L"DB open failed", L"Info", MB_SETFOREGROUND );
        return nullptr;
    }

    MessageBox( 0, L"DB opened", L"Info", MB_SETFOREGROUND );

    QSqlQuery query(data->db);
    bool result = query.exec(
        "CREATE TABLE IF NOT EXISTS Saves "
        "(id INTEGER PRIMARY KEY AUTOINCREMENT, description TEXT,"
        " time INTEGER DEFAULT 0, file TEXT);");
    if(!result) return nullptr;

    return data;
}
