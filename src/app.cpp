#include <exception>
#include <memory>
#include <filesystem>
#include <iostream>

#include <QCoreApplication>
#include <QtWidgets>

#include "app.hpp"
#include "main_window.hpp"
#include "data.hpp"

int App::run(int argc, char** argv)
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    std::unique_ptr<DataSource> data =
        DataSource::create(ensureAndGetDBPath().c_str());
    if(data == nullptr)
    {
        throw std::runtime_error("Failed to open database");
    }

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
