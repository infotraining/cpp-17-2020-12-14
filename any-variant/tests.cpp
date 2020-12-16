#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <any>
#include <variant>

#include "catch.hpp"

using namespace std;

TEST_CASE("any")
{
    std::any anything;

    anything = 1;
    anything = 3.14;
    anything = "text"s;
    anything = vector{1, 2, 3};

    REQUIRE(anything.has_value());

    SECTION("any_cast")
    {
        auto vec = std::any_cast<std::vector<int>>(anything);

        REQUIRE(vec == vector{1, 2, 3});

        REQUIRE_THROWS_AS(std::any_cast<std::string>(anything), std::bad_any_cast);
    }

    SECTION("fast any_cast")
    {
        vector<int>* vec = std::any_cast<std::vector<int>>(&anything);

        if (vec)
        {
            REQUIRE(*vec == vector{1, 2, 3});
        }
    }

    SECTION("any + RTTI")
    {
        const type_info& type_desc = anything.type();

        std::cout << type_desc.name() << "\n";
    }
}

struct NoDefaultConstructible
{
    int value;

    NoDefaultConstructible(int v)
        : value {v}
    {
    }
};

TEST_CASE("std::variant")
{
    std::variant<int, double, std::string, std::vector<int>> v1;

    REQUIRE(std::holds_alternative<int>(v1));
    REQUIRE(std::get<int>(v1) == 0);

    v1 = 3.14;
    v1 = "text"s;
    v1 = std::vector{1, 2, 3};

    REQUIRE(std::get<std::vector<int>>(v1) == vector{1, 2, 3});
    REQUIRE_THROWS_AS(std::get<int>(v1), std::bad_variant_access);

    REQUIRE(std::get_if<int>(&v1) == nullptr);
    REQUIRE(*std::get_if<std::vector<int>>(&v1) == vector{1, 2, 3});

    REQUIRE(v1.index() == 3);

    std::variant<std::monostate, NoDefaultConstructible, double> v2;
    REQUIRE(std::holds_alternative<std::monostate>(v2));
}

struct Printer
{
    void operator()(int x) { std::cout << "int: " << x << "\n"; }
    void operator()(double x) { std::cout << "double: " << x << "\n"; }
    void operator()(const std::string& s) { std::cout << "string: " << s << "\n"; }
};

template <typename... Closures>
struct overload : Closures...
{
    using Closures::operator()...; // since C++17
};

template<typename... Closures>
overload(Closures...) -> overload<Closures...>;

TEST_CASE("visiting variants")
{
    std::variant<int, double, string> v1 = 3.14;

    std::visit(Printer{}, v1);

    auto printer = overload {
        [](int x) { std::cout << "int: " << x << "\n"; },
        [](double x) { std::cout << "double: " << x << "\n"; },
        [](const std::string& s) { std::cout << "string: " << s << "\n"; }
    };

    std::visit(printer, v1);
}

[[nodiscard]] std::variant<std::string, std::errc> load_content(const std::string& filename)
{
    if (filename == "")
        return std::errc::bad_file_descriptor;
    return "content"s;
}

TEST_CASE("using variant")
{
    auto result = load_content("data");

    std::visit(overload{ 
        [](std::errc ec) { std::cout << "Error!!! " <<  static_cast<int>(ec) << std::endl; },
        [](const std::string& s) { std::cout << "Content: " << s << "\n"; }
    }, result);
}