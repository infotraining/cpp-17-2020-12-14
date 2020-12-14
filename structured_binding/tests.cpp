#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <array>

#include "catch.hpp"

using namespace std;

namespace BeforeCpp17
{
    tuple<int, int, double> calc_stats(const vector<int>& data)
    {
        vector<int>::const_iterator min_pos, max_pos;
        tie(min_pos, max_pos) = minmax_element(data.begin(), data.end());

        double avg = accumulate(data.begin(), data.end(), 0.0) / data.size();

        return make_tuple(*min_pos, *max_pos, avg);
    }
}

template <typename TContainer>
tuple<int, int, double> calc_stats(const TContainer& data)
{
    const auto [min_pos, max_pos] = minmax_element(std::begin(data), std::end(data));

    double avg = accumulate(std::begin(data), std::end(data), 0.0) / std::size(data);

    return std::tuple(*min_pos, *max_pos, avg);
}

TEST_CASE("Before C++17")
{
    int data[] = {4, 42, 665, 1, 123, 13};

    auto result = calc_stats(data);
    REQUIRE(std::get<0>(result) == 1); // min
    REQUIRE(std::get<1>(result) == 665);

    int min, max;
    double avg;

    tie(min, max, avg) = calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == 665);
    REQUIRE(avg == Approx(141.333));
}

std::array<int, 3> get_coord()
{
    return array{1, 2, 3};
}

struct ErrorCode
{
    int ec;
    const char* m;
};

ErrorCode open_file(const char* filename)
{
    return ErrorCode {13, "Error#13"};
}

TEST_CASE("Structured bindings")
{
    vector<int> data = {4, 42, 665, 1, 123, 13};

    SECTION("std::tuple")
    {
        auto [min, max, avg] = calc_stats(data); // structured bindings

        REQUIRE(min == 1);
        REQUIRE(max == 665);
        REQUIRE(avg == Approx(141.333));
    }

    SECTION("native arrays")
    {
        int tab[2] = {1, 2};

        auto [x, y] = tab;

        REQUIRE(x == 1);
        REQUIRE(y == 2);
    }

    SECTION("std::array")
    {
        auto [x, y, z] = get_coord();
        REQUIRE(x == 1);
        REQUIRE(z == 3);
    }

    SECTION("std::pair")
    {
        std::map<int, std::string> dict = { {1, "one"}, {2, "two"} };

        auto [pos, was_inserted] = dict.insert(std::pair(3, "three"));       
        
        REQUIRE(was_inserted);
    }

    SECTION("struct/class")
    {
        auto [error_code, message] = open_file("not-exist");
        
        REQUIRE(error_code == 13);
        REQUIRE(message == "Error#13"s);
    }
}

struct Timestamp
{
    int h, m, s;
};

struct StructBindingCompatible : Timestamp
{
};

TEST_CASE("structured bindings - how it works")
{
    Timestamp t1{11, 25, 0};

    auto& [hours, minutes, seconds] = t1;

    hours = 15;

    REQUIRE(t1.h == 15);

    SECTION("is interpreted")
    {
        auto& entity = t1;
     
        auto& hours = entity.h;
        auto& minutes = entity.m;
        auto& seconds = entity.s;
    }

    SECTION("alignas - as qualifier")
    {
        int counters[2] = {};

        alignas(128) auto [counter1, counter2] = counters;
    }
}

struct Person
{
    std::string f;
    std::string s;
};

TEST_CASE("structured binding - move semantics")
{
    Person p{"Jan", "Kowalski"};

   auto [first_name, second_name] = std::move(p);

    REQUIRE(first_name == "Jan"s);
    REQUIRE(second_name == "Kowalski"s);
}

TEST_CASE("use cases")
{
    SECTION("iteration over map")
    {
        std::map<int, std::string> dict = { {1, "one"}, {2, "two"} };

        auto [pos, was_inserted] = dict.insert(std::pair(3, "three"));      

        for(const auto& [key, value] : dict)
        {
            std::cout << key << " - " << value << "\n";
        }
    }

    SECTION("multiple declaration + init")
    {
        std::list lst = {"one", "two", "three" };       

        for(auto [index, it] = std::tuple(0, lst.begin()); it != lst.end(); ++it, ++index)
        {
            std::cout << index << " - " << *it << "\n";
        }
    }
}

///////////////////////////////////////////////
// tuple-like protocol

enum Something
{
    some, 
    thing
};

const std::map<Something, std::string_view> something_desc = {
    { some, "some"sv}, {thing, "thing"sv}
};

// step 1 - std::tuple_size<Something>
template <>
struct std::tuple_size<Something>
{
    static constexpr size_t value = 2;
};

// step 2 - std::tuple_element<Index, Something>
template <>
struct std::tuple_element<0, Something>
{
    using type = int;
};

template <>
struct std::tuple_element<1, Something>
{
    using type = std::string_view;
};

// step 3 - get<Index>
template <size_t Index>
decltype(auto) get(const Something& sth)
{
    if constexpr(Index == 0)
    {
        return static_cast<int>(sth);
    }
    else if constexpr(Index == 1)
    {
        return something_desc.at(sth);
    }
}

// template <>
// decltype(auto) get<0>(const Something& sth)
// {
//     return static_cast<int>(sth);
// }

// template <>
// decltype(auto) get<1>(const Something& sth)
// {
//     return something_desc.at(sth);
// }

TEST_CASE("tuple-like protocol")
{
    Something sth = some;

    const auto [value, description] = sth;

    REQUIRE(value == 0);
    REQUIRE(description == "some"sv);
}