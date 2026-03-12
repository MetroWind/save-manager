#pragma once

#include <filesystem>
#include <memory>
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

enum class GameType
{
    SINGLE_SAVE,
};

enum class SaveDirType
{
    STATIC,
    FIRST_FROM_GLOB,
};

struct GameDefinition
{
    std::string name;
    std::string short_name;
    std::string save_dir;
    std::vector<std::string> files;
    GameType type;
    SaveDirType save_dir_type = SaveDirType::STATIC;

    static std::vector<GameDefinition> loadFromYAML(
        const std::filesystem::path& filename);
};

class DirFinderInterface
{
public:
    // TODO: this should return an optional.
    virtual std::filesystem::path getDir() const = 0;
};

class StaticDirFinder : public DirFinderInterface
{
public:
    explicit StaticDirFinder(const std::string& d): dir(d) {}
    std::filesystem::path getDir() const override;

private:
    std::string dir;
};

class FirstDirFromGlob : public DirFinderInterface
{
public:
    explicit FirstDirFromGlob(const std::string& p): pattern(p) {}
    std::filesystem::path getDir() const override;

private:
    std::string pattern;
};

class GameInterface
{
public:
    virtual ~GameInterface() = default;
    virtual const std::string& name() const = 0;
    // Return a short and filesystem-friendly name of the game.
    virtual const std::string& shortName() const = 0;

    // Get the save dir of the game. This joined with
    // ActiveSave::files must points to the actual save files.
    virtual std::filesystem::path getSaveDir() const = 0;
    // Get all active saves, ordered from old to new;
    virtual std::vector<ActiveSave> saves() const = 0;
    virtual size_t saveCount() const = 0;

    static std::unique_ptr<GameInterface>
    createFromDefinition(const GameDefinition& def);

};

class GameWithSingleSave : public GameInterface
{
public:
    GameWithSingleSave(std::string_view name, std::string_view short_name,
                       std::unique_ptr<DirFinderInterface> dir_finder,
                       std::span<const std::string> files);
    ~GameWithSingleSave() override = default;

    const std::string& name() const override { return game_name; }
    const std::string& shortName() const override { return short_name; }
    std::filesystem::path getSaveDir() const override;
    std::vector<ActiveSave> saves() const override;
    size_t saveCount() const override { return 1; }

private:
    std::string game_name;
    std::string short_name;
    std::unique_ptr<DirFinderInterface> dir_finder;
    std::vector<std::filesystem::path> files;
};
