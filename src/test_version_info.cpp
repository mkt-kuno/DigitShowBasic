/*
 * DigitShowDST - Direct Shear Test Machine Control Software
 * Copyright (C) 2025 Takuto ISHII
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file test_version_info.cpp
 * @brief Test for version_info utility
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "version_info.hpp"
#include <regex>

TEST_SUITE("version_info")
{
    TEST_CASE("get_commit_hash returns a valid full hash")
    {
        const auto hash = version_info::get_commit_hash();

        // Should be exactly 40 hex characters (SHA-1)
        CHECK(hash.size() == 40);

        const std::regex hash_pattern("^[0-9a-f]{40}$");
        CHECK(std::regex_match(std::string(hash), hash_pattern));
    }

    TEST_CASE("get_commit_hash_short returns a valid short hash")
    {
        const auto hash = version_info::get_commit_hash_short();

        // Should be exactly 7 hex characters
        CHECK(hash.size() == 7);

        const std::regex hash_pattern("^[0-9a-f]{7}$");
        CHECK(std::regex_match(std::string(hash), hash_pattern));
    }

    TEST_CASE("get_version_string returns string with hash and optional dirty suffix")
    {
        const auto version = version_info::get_version_string();

        // Should always return a non-empty string
        CHECK(!version.empty());

        // Should contain the full hash (40 chars) and optionally "-dirty"
        if (version_info::is_dirty())
        {
            // Should be 40 + 6 = 46 characters
            CHECK(version.size() == 46);
            CHECK(version.ends_with("-dirty"));

            // First 40 chars should be hex
            const std::regex hash_pattern("^[0-9a-f]{40}-dirty$");
            CHECK(std::regex_match(version, hash_pattern));
        }
        else
        {
            // Should be exactly 40 characters
            CHECK(version.size() == 40);

            const std::regex hash_pattern("^[0-9a-f]{40}$");
            CHECK(std::regex_match(version, hash_pattern));
        }
    }

    TEST_CASE("get_version_string_short returns compact version")
    {
        const auto version = version_info::get_version_string_short();

        // Should always return a non-empty string
        CHECK(!version.empty());

        // Should contain short hash (7 chars) and optionally "-dirty"
        if (version_info::is_dirty())
        {
            // Should be 7 + 6 = 13 characters
            CHECK(version.size() == 13);
            CHECK(version.ends_with("-dirty"));

            const std::regex hash_pattern("^[0-9a-f]{7}-dirty$");
            CHECK(std::regex_match(version, hash_pattern));
        }
        else
        {
            // Should be exactly 7 characters
            CHECK(version.size() == 7);

            const std::regex hash_pattern("^[0-9a-f]{7}$");
            CHECK(std::regex_match(version, hash_pattern));
        }
    }

    TEST_CASE("version string consistency")
    {
        // Calling multiple times should return the same result (build-time constant)
        const auto v1 = version_info::get_version_string();
        const auto v2 = version_info::get_version_string();

        CHECK(v1 == v2);
    }

    TEST_CASE("is_dirty returns consistent value")
    {
        // Should return a consistent boolean value
        const bool dirty1 = version_info::is_dirty();
        const bool dirty2 = version_info::is_dirty();

        CHECK(dirty1 == dirty2);
    }

    TEST_CASE("short hash is prefix of full hash")
    {
        const auto full_hash = version_info::get_commit_hash();
        const auto short_hash = version_info::get_commit_hash_short();

        // Short hash should be first 7 characters of full hash
        CHECK(std::string(full_hash).starts_with(std::string(short_hash)));
    }
}
