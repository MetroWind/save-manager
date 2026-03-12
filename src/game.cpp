#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

#include <ryml.hpp>
#include <ryml_std.hpp>

#include "game.hpp"
#include "c4/yml/node.hpp"
#include "utils.hpp"
#include "glob.hpp"

namespace {

namespace fs = std::filesystem;

std::vector<char> readFile(const fs::path& path)
{
    std::ifstream f(path, std::ios::binary);
    std::vector<char> content;
    content.reserve(102400);
    content.assign(std::istreambuf_iterator<char>(f),
                   std::istreambuf_iterator<char>());
    if(f.bad() || f.fail())
    {
        throw std::runtime_error(
            std::string("Failed to read file " + path.string()));
    }
    return content;
}

template<class T>
bool getYamlValue(ryml::ConstNodeRef node, T& result)
{
    auto value = node.val();
    auto status = std::from_chars(value.begin(), value.end(), result);
    return status.ec == std::errc();
}

} // namespace

std::filesystem::path StaticDirFinder::getDir() const
{
    return {expandPath(dir)};
}

std::filesystem::path FirstDirFromGlob::getDir() const
{
    Glob g(pattern);
    return g.begin()->path();
}

GameWithSingleSave::GameWithSingleSave(
    std::string_view name_, std::string_view short_name_,
    std::string_view save_dir_, std::span<const std::string> files_)
        : game_name(name_), short_name(short_name_), save_dir(save_dir_),
          files(std::begin(files_), std::end(files_))
{
}

std::vector<ActiveSave> GameWithSingleSave::saves() const
{
    std::vector<ActiveSave> saves(1);
    saves[0].files = files;
    saves[0].time = timeCast(fs::last_write_time(save_dir / files[0]));
    return saves;
}

std::unique_ptr<GameInterface>
GameInterface::createFromDefinition(const GameDefinition& def)
{
    switch(def.type)
    {
    case GameType::SINGLE_SAVE:
        return std::make_unique<GameWithSingleSave>(
            def.name, def.short_name, def.save_dir, def.files);
    default:
        std::unreachable();
    }
}

std::vector<GameDefinition>
GameDefinition::loadFromYAML(const fs::path& filename)
{
    auto buffer = readFile(filename);
    ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(buffer));
    std::vector<GameDefinition> games;
    ryml::ConstNodeRef root = tree.rootref();

    for(ryml::ConstNodeRef child: root.children())
    {
        GameDefinition game;
        game.short_name = std::string_view(child.key().str, child.key().size());
        std::string type;
        if(!child["type"].readable())
        {
            throw std::runtime_error("Missing type in game definition");
        }
        child["type"] >> type;
        if(type == "single-save")
        {
            game.type = GameType::SINGLE_SAVE;
        }
        else
        {
            throw std::runtime_error(std::string("Invalid game type: ") + type);
        }
        if(!child["name"].readable())
        {
            throw std::runtime_error("Missing name in game definition");
        }
        child["name"] >> game.name;
        if(!child["save-dir"].readable())
        {
            throw std::runtime_error("Missing save-dir in game definition");
        }
        std::string save_dir;
        child["save-dir"] >> save_dir;
        game.save_dir = expandPath(save_dir);

        if(!child["save-files"].readable())
        {
            throw std::runtime_error("Missing save-files in game definition");
        }
        child["save-files"] >> game.files;
        games.push_back(std::move(game));
    }
    return games;
}
