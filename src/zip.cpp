#include <exception>
#include <stdexcept>
#include <string>

#include <mz.h>
#include <mz_strm.h>
#include <mz_strm_os.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include "zip.hpp"

Unzipper::Unzipper()
        : reader(mz_zip_reader_create()),
          stream(mz_stream_os_create())
{
}

Unzipper::~Unzipper()
{
    if(stream != nullptr)
    {
        mz_stream_os_delete(&stream);
        stream = nullptr;
    }
    if(reader != nullptr)
    {
        mz_zip_reader_delete(&reader);
        reader = nullptr;
    }
}

void Unzipper::extract(const char* zip_file, const char* dest_dir)
{
    Unzipper().extractZipFile(zip_file, dest_dir);
}

void Unzipper::extractZipFile(const char* zip_file, const char* dest_dir)
{
    if(mz_stream_os_open(stream, zip_file, MZ_OPEN_MODE_READ) != MZ_OK)
    {
        throw std::runtime_error(
            std::string("Failed to open zip file to read: ") + zip_file);
    }
    if (mz_zip_reader_open(reader, stream) != MZ_OK)
    {
        throw std::runtime_error(
            std::string("Failed to read zip file ") + zip_file);
    }
    if(mz_zip_reader_save_all(reader, dest_dir) != MZ_OK)
    {
        mz_zip_reader_close(reader);
        throw std::runtime_error(
            std::string("Failed to extract zip file ") + zip_file);
    }
    mz_zip_reader_close(reader);
}

Zipper::Zipper(const char* zip_file)
        : writer(mz_zip_writer_create())
{
    mz_zip_writer_set_compress_method(writer, MZ_COMPRESS_METHOD_DEFLATE);
    mz_zip_writer_set_compress_level(writer, MZ_COMPRESS_LEVEL_BEST);
    if(mz_zip_writer_open_file(writer, zip_file, 0, 0) != MZ_OK)
    {
        mz_zip_writer_delete(&writer);
        writer = nullptr;
        throw std::runtime_error(
            std::string("Failed to open zip file to write: ") + zip_file);
    }
}

Zipper::~Zipper()
{
    if(writer != nullptr)
    {
        if(mz_zip_writer_is_open(writer) == MZ_OK)
        {
            mz_zip_writer_close(writer);
        }
        mz_zip_writer_delete(&writer);
        writer = nullptr;
    }
}

Zipper& Zipper::addFile(const char* path, const char* remove_prefix)
{
    if(mz_zip_writer_add_path(writer, path, remove_prefix, 0, 1) != MZ_OK)
    {
        throw std::runtime_error(
            std::string("Failed to add file to zip: ") + path);
    }
    return *this;
}
