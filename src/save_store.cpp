#include <stdexcept>
#include <string_view>
#include <filesystem>

#include <QUuid>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "save_store.hpp"
#include "game.hpp"
#include "zip.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

SaveStoreFSSQlite::SaveStoreFSSQlite(std::string_view datadir)
        :db(QSqlDatabase::addDatabase("QSQLITE")),
         data_dir(datadir)
{
    namespace fs = std::filesystem;
    if(!db.isValid())
    {
        throw std::runtime_error("SQLite is not supported.");
    }
    db.setDatabaseName((fs::path(data_dir) / "data.db")
                       .generic_string().c_str());
    if(!db.open())
    {
        throw std::runtime_error(
            std::string("Failed to open database: ") +
            db.lastError().text().toStdString());
    }

    QSqlQuery query(db);
    bool result = query.exec(
        "CREATE TABLE IF NOT EXISTS Saves "
        "(id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT,"
        " time_save INTEGER, time_add INTEGER, file TEXT,"
        " game_short_name TEXT);");
    if(!result)
    {
        throw std::runtime_error("Failed to create table.");
    }
}

SaveStoreFSSQlite::~SaveStoreFSSQlite()
{
    if(db.isOpen())
    {
        db.close();
    }
}

StoredSave SaveStoreFSSQlite::storeSave(
        const GameInterface& game, const ActiveSave& save,
        const std::string& title)
{
    fs::path basename = QUuid::createUuid().toString(QUuid::WithoutBraces)
        .toStdString() + ".zip";
    fs::path stored_file = data_dir / game.shortName() / basename;
    {
        Zipper zipper(stored_file.generic_string().c_str());
        for(const fs::path& f: save.files)
        {
            try
            {
                zipper.addFile((game.getSaveDir() / f).generic_string().c_str(),
                               game.getSaveDir().generic_string().c_str());
            }
            catch(const std::runtime_error&)
            {
                if(fs::exists(stored_file))
                {
                    fs::remove(stored_file);
                }
                throw;
            }
        }
    }
    Time save_time = timeCast(fs::last_write_time(
                                  game.getSaveDir() / save.files[0]));
    auto now = Clock::now();
    {
        QSqlQuery query(db);
        query.prepare(
            "INSERT INTO Saves (title, time_save, time_add, file,"
            " game_short_name) VALUES (?, ?, ?, ?, ?);");
        query.addBindValue(title.c_str());
        query.addBindValue(timeToSeconds(save_time));
        query.addBindValue(timeToSeconds(now));
        query.addBindValue(basename.generic_string().c_str());
        query.addBindValue(game.shortName().c_str());
        bool result = query.exec();
        if(!result)
        {
            if(fs::exists(stored_file))
            {
                fs::remove(stored_file);
            }
            throw std::runtime_error("Failed to add save to database.");
        }
    }
    StoredSave result;
    QSqlQuery query(db);
    if(!query.exec("SELECT last_insert_rowid();"))
    {
        throw std::runtime_error("Failed to get last insert rowid.");
    }
    query.next();
    result.id = query.value(0).toInt();
    result.file = stored_file.generic_string();
    result.time_save = std::move(save_time);
    result.time_add = std::move(now);
    result.game_short_name = game.shortName();
    return result;
}

void SaveStoreFSSQlite::loadSave(const StoredSave& save, const char* into_dir)
{
    Unzipper::extract((data_dir / save.game_short_name / save.file)
                      .generic_string().c_str(), into_dir);
}

std::vector<StoredSave>
SaveStoreFSSQlite::listSaves(const std::string& game_short_name)
{
    std::vector<StoredSave> result;
    QSqlQuery query(db);
    query.prepare("SELECT id, title, time_save, time_add, file FROM Saves "
                  "WHERE game_short_name = ? ORDER BY time_add DESC;");
    query.addBindValue(game_short_name.c_str());
    if(!query.exec())
    {
        throw std::runtime_error(std::string("Failed to query for saves: ") +
                                 query.lastError().driverText().toStdString());
    }
    while(query.next())
    {
        StoredSave save;
        save.id = query.value(0).toInt();
        save.title = query.value(1).toString().toStdString();
        save.time_save = secondsToTime(query.value(2).toInt());
        save.time_add = secondsToTime(query.value(3).toInt());
        save.file = query.value(4).toString().toStdString();
        save.game_short_name = game_short_name;
        result.push_back(std::move(save));
    }
    return result;
}

void SaveStoreFSSQlite::deleteSave(StoredSave&& save)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM Saves WHERE id = ?;");
    query.addBindValue(save.id);
    if(!query.exec())
    {
        throw std::runtime_error("Failed to delete save from database.");
    }
    auto save_path = data_dir / save.game_short_name / save.file;
    if(!fs::remove(save_path))
    {
        throw std::runtime_error(
            std::string("Trying to delete non-existing file at ") +
            save_path.string());
    }
}

void SaveStoreFSSQlite::changeTitle(uint64_t id, const QString& new_title)
{
    QSqlQuery query(db);
    query.prepare("UPDATE OR FAIL Saves SET title = ? WHERE id = ?;");
    query.addBindValue(new_title);
    query.addBindValue(id);
    if(!query.exec())
    {
        throw std::runtime_error("Failed to change save title.");
    }
}
