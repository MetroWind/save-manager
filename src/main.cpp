#include <iostream>

#include "app.hpp"

int main(int argc, char *argv[])
{
    std::cerr << "Starting..." << std::endl;
    App app;
    return app.run(argc, argv);
}
