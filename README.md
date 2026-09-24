# atcoder-library

競技プログラミング用の自作ライブラリ (C++ / Python / Rust)。解法リポジトリ [atcoder-solutions](https://github.com/carbon-nil/atcoder-solutions) から submodule (`lib/`) として参照する。

| 言語 | 置き場所 | 使い方 |
|---|---|---|
| C++ | `cpp/cplib/*.hpp` | `CPLUS_INCLUDE_PATH` に `cpp/` を足し、`#include <cplib/div.hpp>` |
| Python | `python/cplib/*.py` | `PYTHONPATH` に `python/` を足し、`from cplib.div import ceil_div` |
| Rust | `rust/` (crate `cplib`) | `Cargo.toml` に `cplib = { path = ".../rust" }`、`use cplib::div::ceil_div;` |

C++ ヘッダ:

- `div.hpp`: `cplib::floor_div` / `cplib::ceil_div`
- `template.hpp`: `rep` / `rrep` / `fora` / `all` / `ll` / `inf` / `infl` / `chmin` / `chmax`
- `prime.hpp`: `p_factorize`
- `graph.hpp`: `bfs` / `dijkstra` (未到達は -1)

`template.hpp` 以降は過去の提出から取り出したもので、提出と同じくグローバル名前空間に置く。

## 提出用の展開

```sh
python3 tools/bundle.py main.cpp > submit.cpp   # .py / .rs も同じ
```

解法が使っている部分 (間接的な依存を含む) だけを 1 ファイルに展開する。`<atcoder/...>` などライブラリ外の include はそのまま残す。制約: Python のライブラリ内は `cplib.` からの絶対 import、Rust のモジュール間参照は `crate::` で書く。

## 検査

```sh
g++-14 -std=gnu++20 -I cpp -fsyntax-only cpp/cplib/*.hpp   # static_assert (#pragma once の警告は無視してよい)
g++-14 -std=gnu++20 -I cpp cpp/test_graph.cpp -o /tmp/test_graph && /tmp/test_graph
python3 python/cplib/div.py
cargo test --manifest-path rust/Cargo.toml
python3 tools/test_bundle.py
```

atcoder-docker (`ghcr.io/carbon-nil/atcoder-docker:3.1.1-light`) 上で動く。
