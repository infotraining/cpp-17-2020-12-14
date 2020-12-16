#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <optional>
#include <string>
#include <tuple>
#include <vector>
#include <array>

#include "catch.hpp"

using namespace std;

void foo()
{
}

template <typename T>
void deduce1(T arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce2(T& arg)
{
    puts(__PRETTY_FUNCTION__);
}

TEST_CASE("Template Argument Deduction - case 1")
{
    int x = 10;
    const int cx = 20;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];

    [[maybe_unused]] auto a1 = x; // int
    deduce1(x);

    [[maybe_unused]] auto a2 = cx; // int
    deduce1(cx);

    [[maybe_unused]] auto a3 = ref_x; // int
    deduce1(ref_x);

    [[maybe_unused]] auto a4 = cref_x; // int
    deduce1(cref_x);

    [[maybe_unused]] auto a5 = tab; // int*
    deduce1(tab);

    [[maybe_unused]] auto a6 = foo; // void(*)()
    deduce1(foo);
}

TEST_CASE("--")
{
    cout << "\n----------\n\n";
}

TEST_CASE("Template Argument Deduction - case 2")
{
    int x = 10;
    const int cx = 20;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];

    [[maybe_unused]] auto& a1 = x; // int&
    deduce2(x);

    [[maybe_unused]] auto& a2 = cx; // const int&
    deduce2(cx);

    [[maybe_unused]] auto& a3 = ref_x; // int&
    deduce2(ref_x);
    deduce2<int&>(ref_x); // reference collapsing

    [[maybe_unused]] auto& a4 = cref_x; // const int&
    deduce2(cref_x);

    [[maybe_unused]] auto& a5 = tab; // int(&)[10]
    deduce2(tab);

    [[maybe_unused]] auto& a6 = foo; // void(&)()
    deduce2(foo);
}

TEST_CASE("**")
{
    cout << "\n----------\n\n";
}

template <typename T>
void deduce3(T&& arg)
{
    [[maybe_unused]] auto target = std::forward<T>(arg);
    puts(__PRETTY_FUNCTION__);
}

namespace Explain
{
    template <typename T>
    class vector
    {
        std::list<T> items_;

    public:
        void push_back(const T& item) // lvalue ref to const
        {
            puts(__PRETTY_FUNCTION__);
            items_.push_back(item); // copy of item
        }

        void push_back(T&& item) // rvalue reference
        {
            puts(__PRETTY_FUNCTION__);
            items_.push_back(std::move(item));
        }

        template <typename... TArgs>
        void emplace_back(TArgs&&... args)
        {
            items_.emplace_back(std::forward<TArgs>(args)...);
        }
    };
}

TEST_CASE("Template Argument Deduction - case 3")
{
    int x = 10;

    [[maybe_unused]] auto&& a1 = 10; // int&&
    deduce3(10);

    [[maybe_unused]] auto&& a2 = x; // int&
    deduce3(x);

    Explain::vector<std::string> vec;

    std::string str = "abc";
    std::string& ref_str = str;

    vec.push_back("abc"s);
    vec.push_back(ref_str);
    vec.emplace_back(10, 'c');
}

struct Gadget
{
    string name;
};

void have_fun(Gadget& g)
{
    puts(__PRETTY_FUNCTION__);
}

void have_fun(const Gadget& g)
{
    puts(__PRETTY_FUNCTION__);
}

void have_fun(Gadget&& g)
{
    puts(__PRETTY_FUNCTION__);
}

// void use(Gadget& g)
// {
//     have_fun(g); // forward
// }

// void use(const Gadget& g)
// {
//     have_fun(g); // forward
// }

// void use(Gadget&& g)
// {
//     have_fun(std::move(g)); // forward
// }

template <typename TGadget>
void use(TGadget&& g)
{
    have_fun(std::forward<TGadget>(g));
}

TEST_CASE("using gadgets")
{
    Gadget g {"ipad"};
    const Gadget cg {"const ipad"};

    // have_fun(g);
    // have_fun(cg);
    // have_fun(Gadget{"temp ipad"});

    use(g);
    use(cg);
    use(Gadget {"temp ipad"});
}

template <typename T1, typename T2>
struct ValuePair
{
    T1 fst;
    T2 snd;

    template <typename TFirst, typename TSecond>
    ValuePair(TFirst&& f, TSecond&& s)
        : fst {std::forward<TFirst>(f)}
        , snd {std::forward<TSecond>(s)}
    {
    }
};

// deduction guides
template <typename T1, typename T2>
ValuePair(T1, T2) -> ValuePair<T1, T2>;

ValuePair(const char*, const char*)->ValuePair<std::string, std::string>;

TEST_CASE("CTAD")
{
    ValuePair<int, double> vp1 {1, 3.14};

    ValuePair vp2 {1, 3.14}; // ValuePair<int, double>

    ValuePair vp3 {3.14f, "text"}; // ValuePair<float, const char*>

    ValuePair vp4 {"text", "text"s}; // ValuePair<const char*, std::string>

    ValuePair vp5 {"abc", "def"}; // ValuePair<std::string, std::string>
}

TEST_CASE("CTAD - special case")
{
    std::vector vec {1, 2, 3}; // std::vector<int>

    std::vector data {vec}; // std::vector<int> - because copy syntax
    static_assert(std::is_same_v<decltype(data), std::vector<int>>);

    std::vector big_data {vec, vec}; // std::vector<std::vector<int>>

    std::tuple t1 {1, 3.14}; // std::tuple<int, double>
    std::tuple t2 {t1}; // std::tuple<int, double> - because of copy syntax
}

/////////////////////////////////////////
// CTAD + Aggregates

template <typename T1, typename T2>
struct Aggregate
{
    T1 v1;
    T2 v2;
};

// explicit deduction guide for an aggregate
template <typename T1, typename T2>
Aggregate(T1, T2) -> Aggregate<T1, T2>;

TEST_CASE("Aggregates + CTAD")
{
    Aggregate agg1 {1, 3.14};
}

TEST_CASE("auto - strange things")
{
    auto ptr1 = new int(13); // int*
    const auto ptr2 = new int(13); // int* const
    const auto* ptr3 = new int(13); // const int*
}

TEST_CASE("CTAD + std library")
{
    int tab[10];
    const int x = 10;

    SECTION("std::pair")
    {
        std::pair p1 {tab, x}; // std::pair<int*, int>

        SECTION("make_pair can do partial deduction")
        {
            auto p2 = std::make_pair<int>('a', 3.14);
        }

        std::map<int, std::string> dict;
        dict.insert(std::pair(1, "one"));
    }

    SECTION("std::tuple")
    {
        std::tuple t1 {tab, x, 3.14, "text"s}; // std::tuple<int*, int, double, std::string>

        std::tuple t2 = t1; // std::tuple<int*, int, double, std::string>
        std::tuple t3 {t1}; // std::tuple<int*, int, double, std::string>

        std::tuple t4 = std::pair {1, 3.14}; // std::tuple<int, double>

        std::tuple<std::tuple<int, double>> t5 {t4};
    }

    SECTION("std::optional")
    {
        std::optional o1 = 42; // std::optional<int>

        std::optional o2 {o1}; // std::optional<int>
    }

    SECTION("smart ptrs")
    {
        SECTION("deduction disabled for raw-pointers")
        {
            std::unique_ptr<int> uptr(new int(13));
            std::shared_ptr<int> sptr(new int(42));
        }

        SECTION("deduction is enabled for conversions")
        {
            std::unique_ptr<int> uptr(new int(13));

            std::shared_ptr sptr = std::move(uptr);
            std::weak_ptr wptr = sptr;
        }
    }

    SECTION("std::function")
    {
        std::function<void()> f = []{};

        std::function f1 = [](int x){ std::cout << x << "\n"; };
        f1(42);
    }

    SECTION("std::containers")
    {
        std::vector vec = {"one"s, "two"s, "three"s};

        std::list lst(vec.begin(), vec.end());

        std::array arr1 = {1, 2, 3, 4}; // std::array<int, 4>
    }

    std::map m1 = {std::pair{"foo"s, 2}, std::pair{"bar"s, 3}};
}

template <typename T>
struct DisableCTAD
{
    using type = T;
};

template <typename T>
class UniquePtr
{
    T* ptr_;
public:
    using T_ = typename DisableCTAD<T>::type; // disables CTAD

    explicit UniquePtr(T_* ptr) : ptr_{ptr}
    {}

    //...
};

TEST_CASE("how to disable CTAD")
{
    UniquePtr<int> uptr(new int(13));
}