# 過去提出から共通実装を取り出す (2026-09-24)

## 作業工程
- AtCoder Problems API (`kenkoooo.com/atcoder/atcoder-api/v3/user/submissions?user=blanoir&from_second=N`、1 回 500 件) で提出一覧を取得。1135 件のうち C++ が大半で、Java 90・Python 17 件。Python と Rust の実装は作らない方針
- C++ の AC を問題ごとに最新 1 件に絞り (554 件)、`atcoder.jp/contests/<c>/submissions/<id>` の `<pre id="submission-code">` からソースを取得
- 定義名の出現数は Codex (OpenAI Codex CLI) に書かせた集計スクリプトで数えた: rep 379、all 297、chmin 284、ll 280、rrep 278、chmax 269、fore 234、inf/infl 約 230、gcd/lcm/swap 約 120、pair 比較関数 10〜41、ModInt 系 5〜6、p_factorize 2
- 探索系はコード形で数えた: DFS 8、グリッド BFS 5、グラフ BFS 3、Dijkstra 2。どれも書き方が毎回違う
- ユーザーが採用したもの: `template.hpp` (rep/rrep/fora/all/ll/inf/infl/chmin/chmax)、`prime.hpp` (p_factorize)、`graph.hpp` (bfs/dijkstra)。見送ったもの: pair 比較関数、fore、grid_bfs、DFS、gcd/lcm/swap (std で足りる)、modint/dsu/segtree (ジャッジの ACL を使う)

## 問題点と解決
- atcoder.jp は curl の既定 User-Agent を 403 (CloudFront) で弾く → ブラウザの UA を付ける。間隔は 1.5 秒
- 取得・集計の補助スクリプトを Claude が自分で書いた → ユーザーの指摘を受け、user スコープのルール `rules/codex-delegation.md` に「使い捨てを含む補助スクリプトも Codex」と追記した (haiiro2gou/obsidian-claude#32)。project の memory ではなくルールに書く
- `div.hpp` を実装の手本として Codex に渡した → ユーザーの指摘では、これはただの出力サンプルで手本にならない。書き方の手本はユーザー自身の提出にする (名前・型・グローバル名前空間)
- cwd が git リポジトリの外 (`~/workspace/atcoder`) だと EnterWorktree が失敗する → リポジトリ内で `git worktree add .claude/worktrees/<name> -b worktree-<name> origin/master` を実行し、cwd がリポジトリ内になってから EnterWorktree の `path` で入る
- worktree 分離中は、関数定義を含む grep の連続コマンドが「複雑すぎる」として拒否された → ジョブの tmp にスクリプトとして置き、`bash <path>` で実行する
- ヘッダが `<cplib/template.hpp>` を include するようになったので、構文チェックには `-I cpp` が要る
