#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <regex>
#include <iterator>

/**
 * @brief Converts a glob pattern string into a std::regex pattern.
 *
 * This helper function translates common glob wildcards (* and ?) into their
 * regular expression equivalents. It also escapes special regex characters.
 *
 * @param glob The input glob pattern (e.g., "*.txt").
 * @return A string containing the equivalent regular expression.
 */
inline std::string glob_to_regex(std::string_view glob) {
    std::string regex_str = "^";
    for (char c : glob) {
        switch (c) {
            case '*':
                regex_str += ".*";
                break;
            case '?':
                regex_str += ".";
                break;
            // Escape special regex characters
            case '.': case '+': case '(': case ')':
            case '{': case '}': case '[': case ']':
            case '\\': case '|': case '^': case '$':
                regex_str += '\\';
                regex_str += c;
                break;
            default:
                regex_str += c;
        }
    }
    regex_str += "$";
    return regex_str;
}

/**
 * @class Glob
 * @brief A C++20 range-based class for iterating over files/directories
 * that match a glob pattern.
 *
 * This class allows you to easily find all filesystem entries in a directory
 * that match a given glob pattern. It is designed to be used with a
 * range-based for loop.
 *
 * Example:
 * for (const auto& entry : Glob("src/*.cpp")) {
 * std::cout << entry.path() << std::endl;
 * }
 */
class Glob {
public:
    // --- Iterator sub-class ---
    class Iterator {
    public:
        // C++ iterator traits
        using iterator_category = std::input_iterator_tag;
        using value_type = std::filesystem::directory_entry;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        // Default constructor creates the "end" iterator
        Iterator() = default;

        /**
         * @brief Constructs an iterator starting at a given path.
         * @param path The directory to start searching in.
         * @param rgx The compiled regex to match filenames against.
         */
        explicit Iterator(const std::filesystem::path& path, const std::regex& rgx)
            : regex(&rgx) {
            try {
                dir_iterator_ = std::filesystem::directory_iterator{path};
                find_next_match(); // Find the first match
            } catch (const std::filesystem::filesystem_error&) {
                // If path doesn't exist or isn't a directory, treat as an empty range.
                // The default-constructed directory_iterator is the end sentinel.
            }
        }

        reference operator*() const { return *dir_iterator_; }
        pointer operator->() const { return &(*dir_iterator_); }

        Iterator& operator++() {
            if (dir_iterator_ != std::filesystem::directory_iterator{}) {
                ++dir_iterator_;
                find_next_match();
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return dir_iterator_ == other.dir_iterator_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

    private:
        /**
         * @brief Advances the internal directory iterator until a match is found
         * or the end of the directory is reached.
         */
        void find_next_match() {
            while (dir_iterator_ != std::filesystem::directory_iterator{}) {
                const std::string filename = dir_iterator_->path().filename().string();
                if (std::regex_match(filename, *regex)) {
                    break; // Found a match
                }
                ++dir_iterator_;
            }
        }

        std::filesystem::directory_iterator dir_iterator_{};
        const std::regex* regex = nullptr;
    };
    // --- End of Iterator sub-class ---

    /**
     * @brief Constructs a Glob object.
     * @param pattern The glob pattern (e.g., "data/*.csv", "*.log").
     */
    explicit Glob(std::string_view pattern) {
        std::filesystem::path p(pattern);

        // Separate the directory path from the filename pattern
        search_path = p.has_parent_path() ? p.parent_path() : ".";
        std::string filename_pattern = p.filename().string();

        // If the pattern is something like "data/", treat it as "data/*"
        if (filename_pattern.empty()) {
            filename_pattern = "*";
        }

        regex = std::regex(glob_to_regex(filename_pattern));
    }

    /**
     * @brief Returns an iterator to the first matching file.
     */
    Iterator begin() {
        return Iterator(search_path, regex);
    }

    /**
     * @brief Returns the end-of-sequence iterator.
     */
    Iterator end() {
        return Iterator();
    }

private:
    std::filesystem::path search_path;
    std::regex regex;
};
