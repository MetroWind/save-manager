#include <exception>
#include <iostream>
#include <nl_types.h>
#include <stdexcept>

#include "app.hpp"
#include "utils.hpp"

int main(int argc, char *argv[])
{
    std::cerr << "Starting..." << std::endl;
    App app;
    try
    {
        int code = app.run(argc, argv);
    }
    catch(const std::exception& e)
    {
        msgBox(QMessageBox::Icon::Critical, e.what());
        return 1;
    }
}
