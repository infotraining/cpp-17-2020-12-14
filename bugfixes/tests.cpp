#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "catch.hpp"

using namespace std;

TEST_CASE("order of evaluation of operators")
{
    std::map<int, int> dict;

    dict[0] = dict.size();

    for (const auto& [key, value] : dict)
    {
        std::cout << key << " - " << value << "\n";
    }
}

enum class Coffee : uint8_t {
    espresso, chemex, v60
};

TEST_CASE("init enums")
{
    Coffee c1 = Coffee::chemex;
    Coffee c2{1}; // since C++17

    REQUIRE(c2 == Coffee::chemex);
}

TEST_CASE("C++ & bytes")
{
    uint8_t legacy_byte = 65;

    legacy_byte += 42;

    std::cout << legacy_byte << "\n";

    SECTION("Since C++17 - std::byte")
    {
        std::byte b1{65};
        std::byte b2{0b01010101};

        auto result = (b1 & b2) << 2;

        std::cout << "result in bytes: " << std::to_integer<uint32_t>(result) << "\n";
    }
}