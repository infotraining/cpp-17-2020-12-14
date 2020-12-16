#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <charconv>
#include <array>

#include "catch.hpp"

using namespace std;

TEST_CASE("optional")
{
    std::optional<int> o1 = 42;
    REQUIRE(o1.has_value());

    o1 = std::nullopt;
    REQUIRE(o1.has_value() == false);

    std::optional<int> o2;
    REQUIRE(o2.has_value() == false);

    std::optional<int> o3 = std::nullopt;

    std::optional middle_name = "Maria"s; // std::optional<std::string>

    std::optional<std::string> nick(std::in_place, 3, 'a');

    if (nick)
    {
        REQUIRE(*nick == "aaa"s);
    }

    SECTION("access to value")
    {
        optional opt = 42;

        SECTION("unsafe")
        {
            REQUIRE(*opt == 42);
        }

        SECTION("safe")
        {
            REQUIRE(opt.value() == 42);

            opt.reset();

            REQUIRE_THROWS_AS(opt.value(), std::bad_optional_access);
        }
    }
}

std::optional<const char*> maybe_getenv(const char* n)
{
    if (const char* x = std::getenv(n))
        return x;
    else
        return std::nullopt;
}

TEST_CASE("maybe_getenv")
{
    std::cout << maybe_getenv("BADPATH").value_or("(not found)") << "\n";
}

TEST_CASE("strange cases")
{
    SECTION("move semantics")
    {
        optional data = vector{1, 2, 3};

        optional target = std::move(data);
        data.reset();

        REQUIRE(data.has_value() == false);
        REQUIRE(target.has_value());
        REQUIRE(*target == vector{1, 2, 3});
    }

    SECTION("optional<bool>")
    {
        optional<bool> ob{false};

        if (ob)
        {
            std::cout << "ob has value..." << *ob << "\n";
        }

        REQUIRE(ob == false);
    }

    SECTION("optional<int*>")
    {
        std::optional<int*> optr{nullptr};

        if (optr)
        {
            std::cout << "optr has value..." << *optr << "\n";
        }

        REQUIRE(optr == nullptr);
    }
}

namespace Explain
{
    template <typename T>
    class Optional
    {
        alignas(T) char object_[sizeof(T)];
        bool has_state_;
    };
}

std::optional<int> to_int(const string_view str)
{
    int value{};

    auto start = str.data();
    auto end = str.data() + str.size();

    if (const auto [pos_end, error_code] = std::from_chars(start, end, value);
        error_code != std::errc{} || pos_end != end)
    {
        return nullopt;
    }

    return value;
}

TEST_CASE("to_int")
{
    optional<int> number = to_int("42");

    REQUIRE(number == 42);

    number = to_int("44as");

    REQUIRE(number.has_value() == false);
}

template <typename TContainer>
constexpr std::optional<std::string_view> find_id(const TContainer& container, std::string_view id)
{
    for(const auto& item : container)
        if (item == id)
            return item;

    return nullopt;
}

TEST_CASE("string_view + optional + constexr")
{
    constexpr std::array ids = { "one"sv, "two"sv, "three"sv };

    static_assert(ids[1] == "two"sv);

    constexpr optional opt_id = find_id(ids, "two"sv);

    static_assert(opt_id.has_value());
}