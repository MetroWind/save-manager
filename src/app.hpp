#pragma once

#include <string>

class App
{
public:
    int run(int argc, char** argv);
private:
    std::string ensureAndGetDBPath() const;
};
