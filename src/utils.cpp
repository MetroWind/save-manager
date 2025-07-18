#include <iostream>
#include <cstdlib>
#include <string>
#include <string_view>

#include "utils.hpp"

void msgBox(QMessageBox::Icon icon, const char* msg)
{
    QMessageBox msgBox;
    msgBox.setIcon(icon);
    switch(icon)
    {
    case::QMessageBox::Icon::Critical:
        msgBox.setText("Error!");
        break;
    case::QMessageBox::Icon::Warning:
        msgBox.setText("Warning!");
        break;
    default:
        break;
    }
    msgBox.setInformativeText(msg);
    msgBox.exec();
}


// @brief Expands common path shortcuts (~ on Unix, %VAR% on Windows)
// cross-platform. This is written by Gemini with some manual changes.
std::string expandPath(std::string_view path)
{
    std::string expanded_path(path);

    // Always expand ~ to HOME even in Windows.
    if(expanded_path.starts_with('~'))
    {
        const char* home_dir = std::getenv("HOME");
        if(home_dir)
        {
            expanded_path.replace(0, 1, home_dir);
        }
    }

#ifdef _WIN32
    // Windows-specific expansion for %VAR% environment variables
    size_t start_pos = 0;
    while((start_pos = expanded_path.find('%', start_pos)) != std::string::npos)
    {
        size_t end_pos = expanded_path.find('%', start_pos + 1);
        if(end_pos == std::string::npos)
        {
            // Unmatched '%' - treat as literal
            break;
        }

        std::string var_name = expanded_path.substr(
            start_pos + 1, end_pos - start_pos - 1);
        const char* var_value = std::getenv(var_name.c_str());

        if(var_value)
        {
            // Replace %VAR% with its value
            expanded_path.replace(start_pos, end_pos - start_pos + 1, var_value);
            // Continue search from the end of the newly inserted value
            start_pos += std::strlen(var_value);
        }
        else
        {
            // Environment variable not found, skip this %VAR% and continue
            start_pos = end_pos + 1;
        }
    }
#endif
    return expanded_path;
}
