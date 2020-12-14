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

        if (std::lock_guard lk{mtx_q_msg}; !q_msg.empty()) // mtx_q_msg.lock()
        {
            msg = q_msg.front();
            q_msg.pop();
        } // mtx_q_msg.unlock()

        //...
        std::cout << msg << "\n";
    }
}
