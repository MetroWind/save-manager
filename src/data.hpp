#pragma once

#include <QSqlDatabase>

class DataSource
{
public:
    DataSource(const char* db_path);
    ~DataSource();
    DataSource(const DataSource&) = delete;
    DataSource& operator=(const DataSource&) = delete;

private:
    QSqlDatabase db;
};
