#pragma once

#include <utility>
#include <vector>

#include <cplib/template.hpp>

// x を素因数分解し、(素数, 指数) を素数の昇順で返す (x >= 1)
constexpr std::vector<std::pair<ll, ll>> p_factorize(ll x) {
    std::vector<std::pair<ll, ll>> res;
    for (ll i = 2; i * i <= x; i++) {
        ll ex = 0;
        while (x % i == 0) {
            ++ex;
            x /= i;
        }
        if (ex > 0) res.push_back({i, ex});
    }
    if (x != 1) res.push_back({x, 1});
    return res;
}

static_assert(p_factorize(360) == std::vector<std::pair<ll, ll>>{{2, 3}, {3, 2}, {5, 1}});
static_assert(p_factorize(1).empty());
static_assert(p_factorize(97) == std::vector<std::pair<ll, ll>>{{97, 1}});
static_assert(p_factorize(1009LL * 1009) == std::vector<std::pair<ll, ll>>{{1009, 2}});
