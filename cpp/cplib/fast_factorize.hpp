#pragma once

// 出典: https://nyaannyaan.github.io/library/prime/fast-factorize.hpp.html
// ライセンス: CC0
// ユーザーの AtCoder 提出 45717366 (abc112_d) からコピー。upstream commit は不明。
// 置換: 32-bit modint は ACL、gcd・乱数は標準ライブラリ。未使用の補助関数は削除。

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <istream>
#include <iterator>
#include <map>
#include <numeric>
#include <ostream>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

#include <atcoder/modint>

struct montgomery64 {
  using mint = montgomery64;
  using i64 = std::int64_t;
  using u64 = std::uint64_t;
  using u128 = __uint128_t;

  static u64 mod;
  static u64 r;
  static u64 n2;

  static u64 get_r() {
    u64 ret = mod;
    for (i64 i = 0; i < 5; ++i) ret *= 2 - mod * ret;
    return ret;
  }

  static void set_mod(u64 m) {
    assert(m < (1LL << 62));
    assert((m & 1) == 1);
    mod = m;
    n2 = -u128(m) % m;
    r = get_r();
    assert(r * mod == 1);
  }

  u64 a;

  montgomery64() : a(0) {}
  montgomery64(const std::int64_t &b) : a(reduce((u128(b) + mod) * n2)){};

  static u64 reduce(const u128 &b) {
    return (b + u128(u64(b) * u64(-r)) * mod) >> 64;
  }

  mint &operator+=(const mint &b) {
    if (i64(a += b.a - 2 * mod) < 0) a += 2 * mod;
    return *this;
  }

  mint &operator-=(const mint &b) {
    if (i64(a -= b.a) < 0) a += 2 * mod;
    return *this;
  }

  mint &operator*=(const mint &b) {
    a = reduce(u128(a) * b.a);
    return *this;
  }

  mint &operator/=(const mint &b) {
    *this *= b.inverse();
    return *this;
  }

  mint operator+(const mint &b) const { return mint(*this) += b; }
  mint operator-(const mint &b) const { return mint(*this) -= b; }
  mint operator*(const mint &b) const { return mint(*this) *= b; }
  mint operator/(const mint &b) const { return mint(*this) /= b; }
  bool operator==(const mint &b) const {
    return (a >= mod ? a - mod : a) == (b.a >= mod ? b.a - mod : b.a);
  }
  bool operator!=(const mint &b) const {
    return (a >= mod ? a - mod : a) != (b.a >= mod ? b.a - mod : b.a);
  }
  mint operator-() const { return mint() - mint(*this); }

  mint pow(u128 n) const {
    mint ret(1), mul(*this);
    while (n > 0) {
      if (n & 1) ret *= mul;
      mul *= mul;
      n >>= 1;
    }
    return ret;
  }

  friend std::ostream &operator<<(std::ostream &os, const mint &b) {
    return os << b.get();
  }

  friend std::istream &operator>>(std::istream &is, mint &b) {
    std::int64_t t;
    is >> t;
    b = montgomery64(t);
    return (is);
  }

  mint inverse() const { return pow(mod - 2); }

  u64 get() const {
    u64 ret = reduce(a);
    return ret >= mod ? ret - mod : ret;
  }

  static u64 get_mod() { return mod; }
};
inline typename montgomery64::u64 montgomery64::mod, montgomery64::r, montgomery64::n2;

namespace fast_factorize {
using u64 = std::uint64_t;

template <typename mint>
bool miller_rabin(u64 n, std::vector<u64> as) {
  mint::set_mod(n);
  u64 d = n - 1;
  while (~d & 1) d >>= 1;
  mint e{1}, rev{std::int64_t(n - 1)};
  for (u64 a : as) {
    if (n <= a) break;
    u64 t = d;
    mint y = mint(a).pow(t);
    while (t != n - 1 && y != e && y != rev) {
      y *= y;
      t *= 2;
    }
    if (y != rev && t % 2 == 0) return false;
  }
  return true;
}

inline bool is_prime(u64 n) {
  if (~n & 1) return n == 2;
  if (n <= 1) return false;
  if (n < (1LL << 30))
    return miller_rabin<atcoder::dynamic_modint<45717366>>(n, {2, 7, 61});
  else
    return miller_rabin<montgomery64>(
        n, {2, 325, 9375, 28178, 450775, 9780504, 1795265022});
}

template <typename mint, typename T>
T pollard_rho(T n) {
  if (~n & 1) return 2;
  if (is_prime(n)) return n;
  mint::set_mod(n);
  mint R, one = 1;
  auto f = [&](mint x) { return x * x + R; };
  static std::mt19937_64 rng(std::random_device{}());
  auto rnd_ = [&]() { return rng() % (n - 2) + 2; };
  auto value = [](mint x) {
    if constexpr (std::is_same_v<mint, montgomery64>) return x.get();
    else return x.val();
  };
  while (1) {
    mint x, y, ys, q = one;
    R = rnd_(), y = rnd_();
    T g = 1;
    constexpr int m = 128;
    for (int r = 1; g == 1; r <<= 1) {
      x = y;
      for (int i = 0; i < r; ++i) y = f(y);
      for (int k = 0; g == 1 && k < r; k += m) {
        ys = y;
        for (int i = 0; i < m && i < r - k; ++i) q *= x - (y = f(y));
        g = std::gcd<T>(value(q), n);
      }
    }
    if (g == n) do
        g = std::gcd<T>(value(x - (ys = f(ys))), n);
      while (g == 1);
    if (g != n) return g;
  }
  std::exit(1);
}

using i64 = long long;

inline std::vector<i64> inner_factorize(u64 n) {
  if (n <= 1) return {};
  u64 p;
  if (n <= (1LL << 30))
    p = pollard_rho<atcoder::dynamic_modint<45717366>, std::uint32_t>(n);
  else
    p = pollard_rho<montgomery64, std::uint64_t>(n);
  if (p == n) return {i64(p)};
  auto l = inner_factorize(p);
  auto r = inner_factorize(n / p);
  std::copy(std::begin(r), std::end(r), std::back_inserter(l));
  return l;
}

inline std::vector<i64> factorize(u64 n) {
  auto ret = inner_factorize(n);
  std::sort(std::begin(ret), std::end(ret));
  return ret;
}

inline std::map<i64, i64> factor_count(u64 n) {
  std::map<i64, i64> mp;
  for (auto &x : factorize(n)) mp[x]++;
  return mp;
}

inline std::vector<i64> divisors(u64 n) {
  if (n == 0) return {};
  std::vector<std::pair<i64, i64>> v;
  for (auto &p : factorize(n)) {
    if (v.empty() || v.back().first != p) {
      v.emplace_back(p, 1);
    } else {
      v.back().second++;
    }
  }
  std::vector<i64> ret;
  auto f = [&](auto rc, int i, i64 x) -> void {
    if (i == (int)v.size()) {
      ret.push_back(x);
      return;
    }
    for (int j = v[i].second;; --j) {
      rc(rc, i + 1, x);
      if (j == 0) break;
      x *= v[i].first;
    }
  };
  f(f, 0, 1);
  std::sort(std::begin(ret), std::end(ret));
  return ret;
}

}  // namespace fast_factorize

using fast_factorize::divisors;
using fast_factorize::factor_count;
using fast_factorize::factorize;
using fast_factorize::is_prime;
