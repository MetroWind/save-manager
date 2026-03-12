#pragma once

#include <string>
#include <filesystem>

#include <QtCore>

#include "game.hpp"
#include "main_window.hpp"
#include "models.hpp"
#include "save_store.hpp"

class App : public QObject
{
    Q_OBJECT

public:
    void run(int argc, char *argv[]);

    // Stores the selected active save.
    void storeActiveSave() const;
    void restoreFromStoredSave() const;
    void deleteStoredSave() const;

signals:
    void saveStored(const QModelIndex& index) const;

private slots:
    void onMidToolbarBtnClick(QAction *action);
    void onRightToolbarBtnClick(QAction* action);
    void onGameSelected();

private:
    std::filesystem::path dataDir() const;
    std::filesystem::path configPath() const;

    MainWindow* main_window = nullptr;
    GameInterface* game = nullptr;
    SaveStoreInterface* store = nullptr;
    ActiveSaveModel* model_active_save = nullptr;
    StoredSaveModel* model_stored_save = nullptr;
    std::vector<GameDefinition> game_definitions;
};
