#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

#include "catch.hpp"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////

// TODO

template<typename T, typename... Args>
uintmax_t matches(const T& cont, Args&&... args)
{
    return (... + std::count(std::begin(cont), std::end(cont), args));
}

TEST_CASE("matches - returns how many items is stored in a container")
{
    vector<int> v{1, 2, 3, 4, 5};

    REQUIRE(matches(v, 2, 5) == 2);
    REQUIRE(matches(v, 100, 200) == 0);
    REQUIRE(matches("abccdef", 'x', 'y', 'z') == 0);
    REQUIRE(matches("abccdef", 'a', 'c', 'f') == 4);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

class Gadget
{
public:
    virtual std::string id() const { return "a"; }
    virtual ~Gadget() = default;
};

class SuperGadget : public Gadget
{
public:
    std::string id() const override
    {
        return "b";
    }
};

template <typename... TArgs>
auto make_vector(TArgs&&... args)
{
  using TCom = std::common_type_t<TArgs...>;

  std::vector<TCom> outvec;
  outvec.reserve(sizeof...(args));
  
  (..., outvec.push_back(std::forward<TArgs>(args)));
  
  return outvec;
}

TEST_CASE("make_vector - create vector from a list of arguments")
{
    // Tip: use std::common_type_t<Ts...> trait

    using namespace Catch::Matchers;

    SECTION("ints")
    {
        std::vector<int> v = make_vector(1, 2, 3);

        REQUIRE_THAT(v, Equals(vector{1, 2, 3}));
    }

    SECTION("string")
    {
        string str = "abc";

        std::vector v = make_vector("aaa"s, str);

        REQUIRE(v == vector{"aaa"s, "abc"s});
    }

    SECTION("unique_ptrs")
    {
        const auto ptrs = make_vector(make_unique<int>(5), make_unique<int>(6));

        REQUIRE(ptrs.size() == 2);
    }

    SECTION("unique_ptrs with polymorphic hierarchy")
    {
        auto gadgets = make_vector(make_unique<Gadget>(), make_unique<SuperGadget>(), make_unique<Gadget>());

        static_assert(is_same_v<decltype(gadgets)::value_type, unique_ptr<Gadget>>);

        vector<string> ids;
        transform(begin(gadgets), end(gadgets), back_inserter(ids), [](auto& ptr) { return ptr->id(); });

        REQUIRE_THAT(ids, Equals(vector<string>{"a", "b", "a"}));
    }
}

// /////////////////////////////////////////////////////////////////////////////////////////////////

// TODO

template <typename T>
struct Range
{
    T low, high;
};

template <typename T1, typename T2>
Range(T1, T2) -> Range<std::common_type_t<T1, T2>>;

template <typename T, typename... TArgs>
bool within(const Range<T>& range, const TArgs&... args)
{
    auto in_range = [&range](const auto& value) { return value >= range.low && value <= range.high; };

    return (... && in_range(args));
}

TEST_CASE("within - checks if all values fit in range [low, high]")
{
    REQUIRE(within(Range{10, 20.1}, 1, 15, 30) == false);
    REQUIRE(within(Range{10, 20}, 10, 12, 13) == true);
    REQUIRE(within(Range{5.0, 5.5}, 5.1, 5.2, 5.3) == true);
}