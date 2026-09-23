# atcoder-library

競技プログラミング用の自作ライブラリ (C++ / Python / Rust)。解法リポジトリ [atcoder-solutions](https://github.com/carbon-nil/atcoder-solutions) から submodule (`lib/`) として参照する。

| 言語 | 置き場所 | 使い方 |
|---|---|---|
| C++ | `cpp/cplib/*.hpp` | `CPLUS_INCLUDE_PATH` に `cpp/` を足し、`#include <cplib/div.hpp>` |
| Python | `python/cplib/*.py` | `PYTHONPATH` に `python/` を足し、`from cplib.div import ceil_div` |
| Rust | `rust/` (crate `cplib`) | `Cargo.toml` に `cplib = { path = ".../rust" }`、`use cplib::div::ceil_div;` |

## 検査

```sh
g++-14 -std=gnu++20 -fsyntax-only cpp/cplib/*.hpp   # static_assert (#pragma once の警告は無視してよい)
python3 python/cplib/div.py
cargo test --manifest-path rust/Cargo.toml
```

atcoder-docker (`ghcr.io/carbon-nil/atcoder-docker:3.1.1-light`) 上で動く。
