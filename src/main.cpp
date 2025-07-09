#include <iostream>

#include <windows.h>

#include "app.hpp"

int main(int argc, char *argv[])
{
    std::cerr << "Starting..." << std::endl;
    MessageBox( 0, L"Press OK", L"Hi", MB_SETFOREGROUND );
    App app;
    return app.run(argc, argv);
}
