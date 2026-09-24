// 試し割りとの比較と大きな整数の素因数分解を実行して確かめる (要 ACL)
#include <cassert>

#include <cplib/prime.hpp>
#include <cplib/fast_factorize.hpp>

int main() {
    assert(factorize(1).empty());
    for (ll n = 1; n <= 20000; ++n) {
        std::vector<ll> expected;
        for (auto [prime, exponent] : p_factorize(n)) {
            for (ll i = 0; i < exponent; ++i) expected.push_back(prime);
        }
        assert(factorize(n) == expected);
    }

    assert(is_prime(998244353));
    assert(is_prime(1000000007));
    assert(is_prime((1LL << 61) - 1));
    assert(!is_prime(1000000007LL * 998244353));
    assert((factorize(1000000007LL * 998244353) == std::vector<ll>{998244353, 1000000007}));

    std::vector<ll> expected(18, 2);
    expected.insert(expected.end(), 18, 5);
    assert(factorize(1000000000000000000LL) == expected);
    assert((factor_count(12) == std::map<ll, ll>{{2, 2}, {3, 1}}));
    assert((divisors(12) == std::vector<ll>{1, 2, 3, 4, 6, 12}));
}
