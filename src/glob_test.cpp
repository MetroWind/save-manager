#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "glob.hpp"

// Helper function to collect glob results into a sorted vector of strings
std::vector<std::string> collect_glob_results(Glob&& glob) {
    std::vector<std::string> results;
    for (const auto& entry : glob) {
        results.push_back(entry.path().filename().string());
    }
    std::sort(results.begin(), results.end());
    return results;
}

class GlobTest : public ::testing::Test {
protected:
    // Create a predictable test environment before each test
    void SetUp() override {
        test_dir_ = "glob_unit_test_dir";
        std::filesystem::create_directory(test_dir_);

        // Create a set of files and directories for testing
        create_file(test_dir_ / "file1.txt");
        create_file(test_dir_ / "file2.log");
        create_file(test_dir_ / "another.txt");
        create_file(test_dir_ / "data.bin");
        create_file(test_dir_ / "config.json");
        std::filesystem::create_directory(test_dir_ / "subdir1");
        std::filesystem::create_directory(test_dir_ / "subdir2");
    }

    // Clean up the test environment after each test
    void TearDown() override {
        std::filesystem::remove_all(test_dir_);
    }

    void create_file(const std::filesystem::path& path) {
        std::ofstream outfile(path);
        outfile << "test";
    }

    std::filesystem::path test_dir_;
};

// Test the '*' wildcard to find all .txt files
TEST_F(GlobTest, FindsAllTxtFilesWithStar) {
    auto pattern = (test_dir_ / "*.txt").string();
    auto results = collect_glob_results(Glob(pattern));

    ASSERT_EQ(results.size(), 2);
    EXPECT_THAT(results, ::testing::ElementsAre("another.txt", "file1.txt"));
}

// Test the '?' wildcard for a single character match
TEST_F(GlobTest, FindsFileWithQuestionMark) {
    auto pattern = (test_dir_ / "file?.log").string();
    auto results = collect_glob_results(Glob(pattern));

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0], "file2.log");
}

// Test matching all files and directories in the test directory
TEST_F(GlobTest, FindsEverythingWithStar) {
    auto pattern = (test_dir_ / "*").string();
    auto results = collect_glob_results(Glob(pattern));

    ASSERT_EQ(results.size(), 7);
    EXPECT_THAT(results, ::testing::ElementsAre(
        "another.txt",
        "config.json",
        "data.bin",
        "file1.txt",
        "file2.log",
        "subdir1",
        "subdir2"
    ));
}

// Test matching directories specifically
TEST_F(GlobTest, FindsDirectories) {
    auto pattern = (test_dir_ / "sub*").string();
    auto results = collect_glob_results(Glob(pattern));

    ASSERT_EQ(results.size(), 2);
    EXPECT_THAT(results, ::testing::ElementsAre("subdir1", "subdir2"));
}

// Test a pattern that should yield no results
TEST_F(GlobTest, HandlesNoMatches) {
    auto pattern = (test_dir_ / "*.csv").string();
    auto results = collect_glob_results(Glob(pattern));

    EXPECT_TRUE(results.empty());
}

// Test a pattern in a directory that does not exist
TEST_F(GlobTest, HandlesNonExistentPath) {
    auto pattern = "non_existent_directory/*";
    auto results = collect_glob_results(Glob(pattern));

    EXPECT_TRUE(results.empty());
}

// Test a pattern with no wildcards, which should match an exact filename
TEST_F(GlobTest, FindsExactFilename) {
    auto pattern = (test_dir_ / "config.json").string();
    auto results = collect_glob_results(Glob(pattern));

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0], "config.json");
}

// Test providing just a directory path, which should be treated as "path/*"
TEST_F(GlobTest, HandlesDirectoryPathAsPattern) {
    auto pattern = test_dir_.string() + "/";
    auto results = collect_glob_results(Glob(pattern));

    // Should be the same as the "FindsEverythingWithStar" test
    ASSERT_EQ(results.size(), 7);
    EXPECT_THAT(results, ::testing::ElementsAre(
        "another.txt",
        "config.json",
        "data.bin",
        "file1.txt",
        "file2.log",
        "subdir1",
        "subdir2"
    ));
}

// Main function to run all the tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
