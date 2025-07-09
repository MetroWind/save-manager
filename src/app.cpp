#include <exception>
#include <memory>
#include <filesystem>
#include <iostream>

#include <windows.h>

#include <QCoreApplication>
#include <QtWidgets>

#include "app.hpp"
#include "main_window.hpp"
#include "data.hpp"

int App::run(int argc, char** argv)
{
    MessageBox( 0, L"Preparing to run...", L"Info", MB_SETFOREGROUND );
    std::unique_ptr<DataSource> data =
        DataSource::create(ensureAndGetDBPath().c_str());
    MessageBox( 0, L"Got data object.", L"Info", MB_SETFOREGROUND );
    if(data == nullptr)
    {
        MessageBox( 0, L"Failed to create db.", L"Info", MB_SETFOREGROUND );
        throw std::runtime_error("Failed to open database");
    }
    MessageBox( 0, L"Created db.", L"Info", MB_SETFOREGROUND );

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    MessageBox( 0, L"Press OK", L"Running...", MB_SETFOREGROUND );
    return app.exec();
}

std::string App::ensureAndGetDBPath() const
{
    namespace fs = std::filesystem;
    // Locate the data dir
    fs::path exe_dir = QCoreApplication::applicationDirPath().toStdString();
    fs::path data_dir = exe_dir / "data";
    if(!fs::exists(data_dir))
    {
        if(!fs::create_directory(data_dir))
        {
            throw std::runtime_error("Failed to create data dir.");
        }
    }
    return (data_dir / "data.db").string();
}
