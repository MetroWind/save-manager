#pragma once

#include <memory>

#include <QSqlDatabase>

class DataSource
{
public:
    DataSource(); // Do NOT use!
    ~DataSource();
    DataSource(const DataSource&) = delete;
    DataSource& operator=(const DataSource&) = delete;

    static std::unique_ptr<DataSource> create(const char* db_path);

private:
    QSqlDatabase db;
};
