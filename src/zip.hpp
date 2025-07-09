#pragma once

class Unzipper
{
public:
    Unzipper();
    ~Unzipper();
    Unzipper(const Unzipper&) = delete;
    Unzipper& operator=(const Unzipper&) = delete;

    void extract(const char* zip_file, const char* dest_dir);

private:
    void* reader;
    void* stream;
};
