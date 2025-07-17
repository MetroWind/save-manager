#pragma once

class Unzipper
{
public:
    ~Unzipper();
    Unzipper(const Unzipper&) = delete;
    Unzipper& operator=(const Unzipper&) = delete;

    static void extract(const char* zip_file, const char* dest_dir);

private:
    Unzipper();
    void extractZipFile(const char* zip_file, const char* dest_dir);

    void* reader;
    void* stream;
};

class Zipper
{
public:
    Zipper(const char* zip_file);
    ~Zipper();
    Zipper(const Zipper&) = delete;
    Zipper& operator=(const Zipper&) = delete;

    Zipper& addFile(const char* path, const char* remove_prefix);

private:
    void* writer;
};
