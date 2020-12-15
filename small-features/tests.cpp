#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

class Person
{
    inline static constexpr auto class_id = "Person";
    inline static uint64_t gen_id = 0;

public:
    uint64_t id = ++gen_id;
};

TEST_CASE("inline static members")
{
    Person p1;
    Person p2;

    REQUIRE(p1.id == 1);
    REQUIRE(p2.id == 2);
}

template <typename TContainer>
void print(string_view prefix, const TContainer& container)
{
    std::cout << prefix << ": [ ";
    for (const auto& item : container)
        std::cout << item << " ";
    std::cout << "]\n";
}

struct Aggregate1
{
    int a;
    double b;
    int coord[3];
    std::string name;

    void print() const
    {
        std::cout << "Aggregate1{" << a << ", " << b;
        std::cout << ", [ ";
        for (const auto& item : coord)
            std::cout << item << " ";
        std::cout << "], '" << name << "'}\n";
    }
};

struct Aggregate2 : Aggregate1, std::string
{
    std::vector<int> data;
};

TEST_CASE("aggregates")
{
    SECTION("native arrays are aggregates")
    {
        int tab_int1[5];
        print("tab_int1", tab_int1);

        std::string tab_str[5];
        print("tab_str", tab_str);

        int tab_int2[5] = {1, 2, 3, 4, 5};
        print("tab_int2", tab_int2);

        int tab_int3[5] = {1, 2, 3};
        print("tab_int3", tab_int3);

        int tab_int4[5] = {};
        print("tab_int4", tab_int4);
    }

    SECTION("structs")
    {
        Aggregate1 agg1;
        agg1.print();

        Aggregate1 agg2 {};
        agg2.print();

        Aggregate1 agg3 {1, 3.14, {1, 2}, "abc"};
        agg3.print();

        Aggregate1 agg4 {1};
        agg4.print();

        std::array<int, 5> arr1 = {1};
        print("arr1", arr1);

        static_assert(std::is_aggregate_v<Aggregate2>);

        Aggregate2 super_agg1 {{1, 3.14, {1, 2}, "base"}, {"text"}, {42, 43, 44}};
        super_agg1.print();
        REQUIRE(super_agg1.size() == 4);
        print("super_agg1.data", super_agg1.data);

        Aggregate2 super_tab[2] = {{{1, 3.14, {1, 2}, "base"}, {"text"}, {42, 43, 44}}, {{1}}};
    }
}

/////////////////////////////////////
// attributes

struct ErrorCode
{
    int err_code;
    const char* msg;
};

[[nodiscard]] ErrorCode open_file(const std::string& filename)
{
    [[maybe_unused]] auto flag = 42;
    return {13, "Error#13"};
}

void step1() { }
void step2() { }
void step3() { }

enum Coffee
{
    espresso,
    americano [[deprecated]] = espresso
};

namespace [[deprecated]] LegacyCode
{
    namespace Lib::ver_1_0
    {
        void f(int n)
        {
            switch (n)
            {
            case 1:
            case 2:
                step1();
                [[fallthrough]];
            case 3: // no warning on fallthrough
                step2();
                break;
            case 4:
                step3();
            }
        }
    }
}

TEST_CASE("attributes")
{
    ErrorCode ec = open_file("not-found");

    LegacyCode::Lib::ver_1_0::f(42);

    Coffee c = americano;
    REQUIRE(c == espresso);
}