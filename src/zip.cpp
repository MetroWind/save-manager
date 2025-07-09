#include <exception>
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
    }
    if(reader != nullptr)
    {
        mz_zip_reader_delete(&reader);
    }
}

void Unzipper::extract(const char* zip_file, const char* dest_dir)
{
    if(mz_stream_os_open(stream, zip_file, MZ_OPEN_MODE_READ) != MZ_OK)
    {
        throw std::runtime_error(
            std::string("Failed to open zip file ") + zip_file);
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
