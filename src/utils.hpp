#pragma once

#include <chrono>
#include <exception>
#include <string>
#include <string_view>

#include <QMessageBox>

inline void msgBox(QMessageBox::Icon icon, const char* msg)
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
    }
    msgBox.setDetailedText(msg);
    msgBox.exec();
}

class Warning : public std::exception
{
public:
    Warning(std::string_view text) : msg(text) {}
    const char* what() const noexcept override { return msg.c_str(); }

private:
    std::string msg;
};

using Clock = std::chrono::system_clock;
using Time = std::chrono::time_point<Clock>;

template <typename C>
inline int64_t timeToSeconds(const std::chrono::time_point<C>& t)
{
    return std::chrono::duration_cast<std::chrono::seconds>(
        t.time_since_epoch()).count();
}

inline Time secondsToTime(const int64_t t)
{
    return Time(std::chrono::seconds(t));
}

inline std::string timeToStr(const Time& t)
{
    return std::format("{:%F %R}", t);
}

std::string expandPath(std::string_view p);
