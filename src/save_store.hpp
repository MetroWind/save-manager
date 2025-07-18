#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <filesystem>

#include <QtCore>
#include <QSqlDatabase>

#include "game.hpp"
#include "utils.hpp"

struct StoredSave
{
    // A unique ID associated with the save. Not all storage backend
    // nessesarily need this.
    int64_t id;
    // The save file in the storage. Not all storage backend
    // nessesarily need this.
    std::string file;
    // The time the save was created by the game
    Time time_save;
    // The time the save was added to the store.
    Time time_add;
    std::string title;
    std::string game_short_name;
};

// Interface of the storage backend
class SaveStoreInterface
{
public:
    virtual ~SaveStoreInterface() = default;
    virtual StoredSave storeSave(
        const GameInterface& game, const ActiveSave& save,
        const std::string& title) = 0;
    virtual void loadSave(const StoredSave& save, const char* into_dir) = 0;
    // Get all stored saves, ordered by time stored from new to old.
    virtual std::vector<StoredSave> listSaves(
        const std::string& game_short_name) = 0;
    virtual void deleteSave(StoredSave&& save) = 0;
    virtual void changeTitle(uint64_t id, const QString& new_title) = 0;
};

// A storage backend that stores saves as zip files and metadata in
// SQLite database.
class SaveStoreFSSQlite : public SaveStoreInterface
{
public:
    SaveStoreFSSQlite(std::string_view data_dir);
    SaveStoreFSSQlite(const SaveStoreFSSQlite&) = delete;
    SaveStoreFSSQlite& operator=(const SaveStoreFSSQlite&) = delete;
    ~SaveStoreFSSQlite() override;

    StoredSave storeSave(
        const GameInterface& game, const ActiveSave& save,
        const std::string& title) override;
    void loadSave(const StoredSave& save, const char* into_dir) override;
    std::vector<StoredSave> listSaves(const std::string& game_short_name)
        override;
    void deleteSave(StoredSave&& save) override;
    void changeTitle(uint64_t id, const QString& new_title) override;

private:
    QSqlDatabase db;
    std::filesystem::path data_dir;
};
