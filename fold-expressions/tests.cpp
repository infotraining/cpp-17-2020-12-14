#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

namespace Cpp98
{
    void fold_98()
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};

        auto sum = std::accumulate(std::begin(vec), std::end(vec), 0);
        std::cout << "sum: " << sum << "\n";

        auto result = std::accumulate(std::begin(vec), std::end(vec), "0"s,
            [](const std::string& reduced, int item) {
                return "("s + reduced + " + "s + std::to_string(item) + ")"s;
            });

        std::cout << result << "\n";
    }
}

TEST_CASE("fold expressions")
{
    Cpp98::fold_98();
}

namespace BeforeCpp17
{
    template <typename T>
    auto sum(const T& last)
    {
        return last;
    }

    template <typename THead, typename... TTail>
    auto sum(const THead& head, const TTail&... tail)
    {
        return head + sum(tail...);
    }
}

template <typename... TArgs>
auto sum(const TArgs&... args)   // arg1, arg2, arg3, arg4
{
    return (... + args); // left unary fold +  (((arg1 + arg2) + arg3) + arg4)
}

template <typename... TArgs>
auto sum_r(const TArgs&... args) // arg1, arg2, arg3, arg4
{
    return (args + ...); // right unary fold + (arg1 + (arg2 + (arg3 + arg4)))
}

template <typename... TArgs>
void print(const TArgs&... args)
{
    bool is_first = true;

    auto with_space = [&is_first](const auto& arg) {
        if (!is_first)
            std::cout << " ";
        is_first = false;
        return arg;
    };

    (std::cout << ... << with_space(args)) << "\n"; // binary left fold <<
}

template <typename... TArgs>
void print_lines(const TArgs&... args)
{
    (..., (std::cout << args << "\n")); // left unary fold ,
}

void f(int x)
{
    std::cout << "f(" << x << ")\n";
}

template <typename F, typename... TArgs>
void call(F func, TArgs&&... args)
{
    (..., func(std::forward<TArgs>(args)));
}

TEST_CASE("fold + variadic templates")
{
    REQUIRE(sum(1, 2, 3, 4) == 10);

    print(1, 3.14, "text"s, "abc");
    print_lines(1, 3.14, "text"s, "abc");

    call(f, 1, 2, 3, 4);
}