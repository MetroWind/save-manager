#pragma once

#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "utils.hpp"

struct ActiveSave
{
    // A set of files/dirs consist of a save. This should be the
    // logical set of paths which the save storage can zip into a
    // single file. The paths should be relative to
    // Game::getSaveDir().
    std::vector<std::filesystem::path> files;
    Time time;
};

class GameInterface
{
public:
    virtual ~GameInterface() = default;
    virtual std::string_view name() const = 0;
    // Return a short and filesystem-friendly name of the game.
    virtual const std::string& shortName() const = 0;

    // Get the save dir of the game. This joined with
    // ActiveSave::files must points to the actual save files.
    virtual std::filesystem::path getSaveDir() const = 0;
    // Get all active saves, ordered from old to new;
    virtual std::vector<ActiveSave> saves() const = 0;
    virtual size_t saveCount() const = 0;
};

class GameWithSingleSave : public GameInterface
{
public:
    GameWithSingleSave(std::string_view name, std::string_view short_name,
                       std::string_view save_dir,
                       std::span<std::string_view> files);
    ~GameWithSingleSave() override = default;

    std::string_view name() const override { return game_name; }
    const std::string& shortName() const override { return short_name; }
    std::filesystem::path getSaveDir() const override { return save_dir; }
    std::vector<ActiveSave> saves() const override;
    size_t saveCount() const override { return 1; }

private:
    std::string game_name;
    std::string short_name;
    std::filesystem::path save_dir;
    std::vector<std::filesystem::path> files;
};
