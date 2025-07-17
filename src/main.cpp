#include <exception>
#include <iostream>

#if _WIN32
#include <windows.h>
#endif

#include <QCoreApplication>

#include "app.hpp"
#include "utils.hpp"

int main(int argc, char *argv[])
{
    App app;
    try
    {
        app.run(argc, argv);
    }
    catch(const std::exception& e)
    {
        if(QCoreApplication::instance() == nullptr ||
           QCoreApplication::startingUp())
        {
#if _WIN32
            MessageBox(0, e.what(), "Error", 0);
#else
            std::cerr << "Error: " << e.what() << std::endl;
#endif
        }
        else
        {
            msgBox(QMessageBox::Icon::Critical, e.what());
        }
        return 1;
    }

    return 0;
}
