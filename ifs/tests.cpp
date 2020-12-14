#include <algorithm>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("if with initializers")
{
    vector vec = {1, 2, 665, 23, 43, 42, 545};

    if (auto pos = std::find(begin(vec), end(vec), 42); pos != end(vec))
    {
        std::cout << "Found " << *pos << "\n";
    }
    else
    {
        std::cout << "Item not found...\n";
        assert(pos == end(vec));
    }
}

TEST_CASE("if with initializer & structured bindings")
{
    std::map<int, std::string> dict = {{1, "one"}};

    if (auto [pos, was_inserted] = dict.emplace(2, "two"); was_inserted)
    {
        const auto& [key, value] = *pos;
        std::cout << "Item: " << key << " - " << value << " was inserted...\n";
    }
    else
    {
        const auto& [key, value] = *pos;
        std::cout << "Item: " << key << " - " << value << " was already in map...\n";
    }
}

TEST_CASE("if with mutex")
{
    std::queue<std::string> q_msg;
    std::mutex mtx_q_msg;

    SECTION("thread#1")
    {
        std::lock_guard lk {mtx_q_msg}; // CTAD
        q_msg.push("START");
    }

    SECTION("thread#2")
    {
        std::string msg;

        if (std::lock_guard lk {mtx_q_msg}; !q_msg.empty()) // mtx_q_msg.lock()
        {
            msg = q_msg.front();
            q_msg.pop();
        } // mtx_q_msg.unlock()

        //...
        std::cout << msg << "\n";
    }
}

namespace BeforeCpp17
{
    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_integral_v<T>, bool>
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point_v<T>, bool>
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == T(0.5);
    }
}

template <typename T>
bool is_power_of_2(T value)
{
    if constexpr (std::is_integral_v<T>)
    {
        return value > 0 && (value & (value - 1)) == 0;
    }
    else
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == T(0.5);
    }
}

TEST_CASE("if constexpr")
{
    REQUIRE(is_power_of_2(2));
    REQUIRE(is_power_of_2(4ll));
    REQUIRE(is_power_of_2(5) == false);
    REQUIRE(is_power_of_2(8u));

    REQUIRE(is_power_of_2(8.0));
    REQUIRE(is_power_of_2(64.0f));
}

namespace BeforeCpp17
{
    void print()
    {
        std::cout << "\n";
    }

    template <typename Head, typename... Tail>
    void print(const Head& head, const Tail&... tail)
    {
        std::cout << head << " ";
        print(tail...);
    }
}

template <typename Head, typename... Tail>
void print(const Head& head, const Tail&... tail)
{
    std::cout << head << " ";

    if constexpr(sizeof...(tail) > 0)
    {
        print(tail...);
    }
    else
    {
        std::cout << "\n";
    }
}

TEST_CASE("variadic templates")
{
    print(1, 2.13, "text"s);
}

///////////////////////////////////
// type traits

auto foo(int x) -> int
{
    auto multiplier = [x]() -> int { return 2 * x; };

    return 2 * x;
}

template <int v>
struct Foo
{
    static constexpr int value = 2 * v;
};

template <typename T>
struct IsPointer
{
    constexpr static bool value = false;
};

template <typename T>
struct IsPointer<T*>
{
    constexpr static bool value = true;
};

// variable template
template <typename T>
constexpr bool IsPointer_v = IsPointer<T>::value;

// template <typename T, typename = std::enable_if_t<!IsPointer_v<T>>>
// auto get_value(T value)
// {
//     return value;
// }

// template <typename T, typename = void, typename = std::enable_if_t<IsPointer_v<T>>>
// auto get_value(T value)
// {
//     return *value;
// }

template <typename T>
auto get_value(T value)
{
    if constexpr (IsPointer_v<T>)
    {
        return *value;
    }
    else
    {
        return value;
    }
}

TEST_CASE("type traits")
{
    REQUIRE(foo(2) == 4);

    static_assert(Foo<2>::value == 4);

    static_assert(IsPointer<int>::value == false);
    static_assert(IsPointer_v<int*> == true);

    int x = 10;
    int* ptr_x = &x;

    REQUIRE(get_value(x) == 10);
    REQUIRE(get_value(ptr_x) == 10);
}
