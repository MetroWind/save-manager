#include <algorithm>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "game.hpp"
#include "utils.hpp"

GameWithSingleSave::GameWithSingleSave(
    std::string_view name_, std::string_view short_name_,
    std::string_view save_dir_, std::span<std::string_view> files_)
        : game_name(name_), short_name(short_name_), save_dir(save_dir_),
          files(std::begin(files_), std::end(files_))
{
}

std::vector<ActiveSave> GameWithSingleSave::saves() const
{
    std::vector<ActiveSave> saves(1);
    saves[0].files = files;
    saves[0].time = secondsToTime(timeToSeconds(
        std::filesystem::last_write_time(save_dir / files[0])));
    return saves;
}
