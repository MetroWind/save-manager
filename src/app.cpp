#include <memory>
#include <filesystem>
#include <stdexcept>

#include <QApplication>
#include <QtWidgets>

#include "app.hpp"
#include "main_window.hpp"
#include "models.hpp"
#include "save_store.hpp"
#include "game.hpp"

namespace fs = std::filesystem;

void App::run(int argc, char *argv[])
{
    QApplication app(argc, argv);

    fs::path data_dir = dataDir();
    if(!fs::exists(dataDir()))
    {
        if(!fs::create_directories(data_dir))
        {
            throw std::runtime_error(
                std::string("Failed to create data directory at ") +
                data_dir.string());
        }
    }

    std::unique_ptr<GameInterface> the_game =
        GameInterface::createFromDefinition(
            GameDefinition::loadFromYAML(configPath())[0]);

    if(the_game == nullptr)
    {
        throw std::runtime_error("Failed to load game.");
    }

    game = the_game.get();
    model_active_save = new ActiveSaveModel(std::move(the_game));
    std::unique_ptr<SaveStoreInterface> the_store =
        std::make_unique<SaveStoreFSSQlite>(data_dir.string());
    store = the_store.get();

    model_stored_save =
        new StoredSaveModel(std::move(the_store), game->shortName());
    MainWindow window(*model_active_save, *model_stored_save);
    main_window = &window;
    window.setGameName(game->name().c_str());

    QObject::connect(
        &main_window->midToolbar(), &CenteredToolbar::actionTriggered,
        this, &App::onMidToolbarBtnClick);

    QObject::connect(
        &main_window->rightToolbar(), &CenteredToolbar::actionTriggered,
        this, &App::onRightToolbarBtnClick);

    // When a save is stored, trigger edit to edit the title. Not sure
    // why SIGNAL and SLOT have to be used here but on the other
    // connections...
    QObject::connect(
        this, SIGNAL(saveStored(const QModelIndex&)),
        &main_window->panelRight().listView(), SLOT(edit(const QModelIndex&)));

    window.show();
    app.exec();
}

void App::storeActiveSave() const
{
    auto index = main_window->selectedActiveSave();
    if(!index.has_value()) return;
    ActiveSave save = game->saves()[*index];
    store->storeSave(*game, save, "New save");
    emit model_stored_save->dataChanged(
        model_stored_save->index(0, 0),
        model_stored_save->index(model_stored_save->rowCount(), 0));
    emit saveStored(model_stored_save->index(0, 0));
}

void App::restoreFromStoredSave() const
{
    auto index = main_window->selectedStoredSave();
    if(!index.has_value()) return;
    StoredSave stored_save = store->listSaves(game->shortName())[*index];
    store->loadSave(stored_save, game->getSaveDir().generic_string().c_str());
    model_active_save->reload();
    emit model_active_save->dataChanged(
        model_active_save->index(0, 0),
        model_active_save->index(model_active_save->rowCount(), 0));

}

void App::deleteStoredSave() const
{
    auto index = main_window->selectedStoredSave();
    if(!index.has_value()) return;

    const int deletion_count = 1;

    StoredSave stored_save = store->listSaves(game->shortName())[*index];
    store->deleteSave(std::move(stored_save));
    emit model_stored_save->dataChanged(
        model_stored_save->index(0, 0),
        model_stored_save->index(model_stored_save->rowCount() +
                                 deletion_count, 0));
}

void App::onMidToolbarBtnClick(QAction* action)
{
    if(action->objectName() == MainWindow::ACTION_STORE)
    {
        storeActiveSave();
    }
    else if(action->objectName() == MainWindow::ACTION_RESTORE)
    {
        restoreFromStoredSave();
    }
}

void App::onRightToolbarBtnClick(QAction* action)
{
    if(action->objectName() == MainWindow::ACTION_DELETE_STORED)
    {
        deleteStoredSave();
    }
}

fs::path App::dataDir() const
{
#ifdef _WIN32
    return fs::path(QCoreApplication::applicationDirPath().toStdString()) /
        "data";
#elif DEBUG
    return "data";
#else
    return expandPath("~/.config/simple-save-manager/data");
#endif
}

fs::path App::configPath() const
{
#ifdef _WIN32
    return fs::path(QCoreApplication::applicationDirPath().toStdString()) /
        "games.yaml";
#elif DEBUG
    return "games-debug.yaml";
#else
    return expandPath("~/.config/simple-save-manager/games.yaml");
#endif
}
