#include <format>

#include <QtCore>

#include "models.hpp"
#include "game.hpp"
#include "save_store.hpp"
#include "utils.hpp"

ActiveSaveModel::ActiveSaveModel(std::unique_ptr<GameInterface> g)
        : game(std::move(g))
{
    reload();
}

void ActiveSaveModel::reload()
{
    saves = game->saves();
}

int ActiveSaveModel::rowCount([[maybe_unused]] const QModelIndex &parent) const
{
    if(game == nullptr)
    {
        return 0;
    }
    return game->saveCount();
}

QVariant ActiveSaveModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        return std::format("Save {}\n{}", index.row(),
                           timeToStr(saves[index.row()].time)).c_str();
    }
    return QVariant();
}

QVariant ActiveSaveModel::headerData(
    [[maybe_unused]] int section, [[maybe_unused]] Qt::Orientation orientation,
    [[maybe_unused]] int role) const
{
    return "Save";
}

StoredSaveModel::StoredSaveModel(std::unique_ptr<SaveStoreInterface> s,
                                 std::string_view short_name)
        : store(std::move(s)), game_short_name(short_name)
{
}

int StoredSaveModel::rowCount(const QModelIndex &) const
{
    if(store == nullptr)
    {
        return 0;
    }
    return store->listSaves(game_short_name).size();
}

QVariant StoredSaveModel::data(const QModelIndex &index, int role) const
{
    auto saves = store->listSaves(game_short_name);
    switch(role)
    {
    case Qt::DisplayRole:
        return std::format("{}\n{}", saves[index.row()].title,
                           timeToStr(saves[index.row()].time_save)).c_str();
    case Qt::EditRole:
        return saves[index.row()].title.c_str();
    case Qt::UserRole:          // This carrys the ID of the save.
        return saves[index.row()].id;
    default:
        return QVariant();
    }
}

QVariant StoredSaveModel::headerData(int, Qt::Orientation, int) const
{
    return "Save";
}

Qt::ItemFlags StoredSaveModel::flags(const QModelIndex& index) const
{
    if(!index.isValid()) return Qt::ItemIsEnabled;
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool StoredSaveModel::setData(const QModelIndex &index,
                              const QVariant &value, int role)
{
    if(index.isValid() && role == Qt::EditRole)
    {
        auto saves = store->listSaves(game_short_name);
        store->changeTitle(data(index, Qt::UserRole).toInt(),
                           value.toString().toLocal8Bit().constData());
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}
