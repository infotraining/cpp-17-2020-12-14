#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <string_view>

#include "catch.hpp"

using namespace std;

TEST_CASE("string_view")
{
    SECTION("creation")
    {
        const char* cstr = "text";

        std::string_view sv1 = cstr;
        REQUIRE(sv1.size() == 4);

        std::string str = "text";

        std::string_view sv2 = str;
        REQUIRE(sv2.size() == 4);

        const char* text = "abc def ghi";
        std::string_view token(text, 3);
        REQUIRE(token == "abc"s);
    }

}

template <typename Container>
void print_all(const Container& container, std::string_view prefix)
{
    cout << prefix << ": [ ";
    for (const auto& item : container)
        cout << item << " ";
    cout << "]\n";
}

TEST_CASE("using string_view")
{
    print_all(vector{1, 2, 3}, "vec"s);
}

string_view get_prefix(std::string&& text, size_t length) = delete;

string_view get_prefix(string_view text, size_t length)
{
    return {text.data(), length};
}

TEST_CASE("beware")
{
    SECTION("safe")
    {
        const char* text = "abcdef";
        string_view sv1(text, 3);

        REQUIRE(sv1 == "abc"sv);
    }

    SECTION("string")
    {
        string_view sv_evil = "text"s;

        std::cout << sv_evil << "\n";

        SECTION("only valid way")
        {
            const std::string input_text = "abc def";
            std::string_view prefix = get_prefix(input_text, 3);
            REQUIRE(prefix == "abc"sv);
        }

        //string_view sv_dangling = get_prefix("abc_def"s, 3);
        //std::cout << sv_dangling << "\n";
    }
}

TEST_CASE("string_view differences")
{
    std::string_view sv;
    REQUIRE(sv.data() == nullptr);

    std::string str;
    REQUIRE(str.data() != nullptr);

    char tab[] = { 'a', 'b', 'c'};

    std::string_view sv_without_null(tab, 3);
    REQUIRE(sv_without_null == "abc"sv);
}

TEST_CASE("string_view to string conversion")
{
    std::string_view sv = "text";
    std::string s(sv); // explicit conversion
}



// void print(const std::string& str)
// {
//     std::cout << str << "\n";
// }

void print(std::string_view str)
{
    std::cout << str << "\n";
}

TEST_CASE("string_view as constexpr")
{
    constexpr array ids = { "ericsson"sv, "nokia"sv, "china"sv };

    print(ids[0]);
    print("text");
}

