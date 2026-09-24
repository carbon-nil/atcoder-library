# fast_factorize.hpp の出典

- upstream: https://nyaannyaan.github.io/library/prime/fast-factorize.hpp.html (Nyaan's Library)
- ライセンス: CC0
- ユーザーの AtCoder 提出 45717366 (abc112_d) に同梱されたコードからコピー。
- upstream commit SHA: 不明。
- 32-bit Montgomery modint を ACL の `dynamic_modint<45717366>` に、gcd を `std::gcd` に、乱数を `std::mt19937_64` に置換。未使用の逆元・累乗・乱数補助関数を削除。
