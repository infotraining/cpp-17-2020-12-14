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

TEST_CASE("auto + {}")
{
    int x1 = 10;
    int x2{10};

    auto a1 = 10; // int
    auto a2{10};  // int
    //auto a3{1, 2, 3}; // ERROR - ill-formed

    auto lst1 = {1, 2, 3}; // std::initializer_list<int>
    auto lst2 = {1};       // std::initializer_list<int>
}

void foo1()
{
}

void foo2() noexcept
{
}

template <typename F1, typename F2>
void call(F1 f1, F2 f2)
{
    f1();
    f2();
}

TEST_CASE("noexcept")
{
    static_assert(!std::is_same_v<decltype(foo1), decltype(foo2)>);

    void (*ptr_fun)() noexcept = nullptr;

    ptr_fun = foo2;

    call(foo1, foo2);
}

std::vector<int> create_vector_rvo(size_t n)
{
    return std::vector<int>(n, -1);  // prvalue
}

std::vector<int> create_vector_nrvo(size_t n)
{
    vector<int> vec(n);
    std::iota(begin(vec), end(vec), 0);
    
    return vec; // lvalue
}

TEST_CASE("rvo vs. nrvo")
{
    std::vector<int> data = create_vector_rvo(1'000'000);
}

struct CopyMoveDisabled
{
    vector<int> data;

    CopyMoveDisabled(std::initializer_list<int> lst)
        : data {lst}
    {
    }

    CopyMoveDisabled(const CopyMoveDisabled&) = delete;
    CopyMoveDisabled& operator=(const CopyMoveDisabled&) = delete;
    CopyMoveDisabled(CopyMoveDisabled&&) = delete;
    CopyMoveDisabled& operator=(CopyMoveDisabled&&) = delete;
    ~CopyMoveDisabled() = default;
};

CopyMoveDisabled create_impossible()
{
    return CopyMoveDisabled{1, 2, 3};
}

// CopyMoveDisabled create_cmd_nrvo()
// {
//     CopyMoveDisabled result{1, 2, 3};
//     return result; // illegal - CopyMoveDisabled must be copyable or moveable
// }

void use_impossible(CopyMoveDisabled arg)
{
    std::cout << "Using object: ";
    for(const auto& item : arg.data)
        std::cout << item << " ";
    std::cout << "\n";
}

TEST_CASE("returning & passing CopyMoveDisabled")
{
    CopyMoveDisabled cmd = create_impossible();

    REQUIRE(cmd.data == vector{1, 2, 3});

    use_impossible(create_impossible());
}