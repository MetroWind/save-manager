#pragma once

#include <memory>

#include <QtCore>
#include <vector>

#include "game.hpp"
#include "save_store.hpp"

class ActiveSaveModel : public QAbstractListModel
{
public:
    ActiveSaveModel(std::unique_ptr<GameInterface>);
    int rowCount(const QModelIndex &parent=QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    void reload();
    void setGame(std::unique_ptr<GameInterface> g);

private:
    std::unique_ptr<GameInterface> game;
    std::vector<ActiveSave> saves;
};

class StoredSaveModel : public QAbstractListModel
{
public:
    StoredSaveModel(std::unique_ptr<SaveStoreInterface>,
                    std::string_view game_short_name);
    int rowCount(const QModelIndex &parent=QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value,
                 int role=Qt::EditRole) override;
    void setGameShortName(std::string_view short_name);
private:
    std::unique_ptr<SaveStoreInterface> store;
    std::string game_short_name;
};
