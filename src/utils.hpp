#pragma once

#include <chrono>
#include <exception>
#include <string>
#include <string_view>

#include <QMessageBox>

void msgBox(QMessageBox::Icon icon, const char* msg);

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

template <typename T>
inline Time timeCast(const T& t)
{
    return std::chrono::clock_cast<Clock, typename T::clock, Clock::duration>(t);
}

inline std::string timeToStr(const Time& t)
{
    return std::format("{:%F %R}", t);
}

std::string expandPath(std::string_view p);
