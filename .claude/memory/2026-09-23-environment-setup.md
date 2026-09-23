# AtCoder 開発環境の立ち上げ (2026-09-23)

## 作業工程
- `claude-init-category atcoder --account carbon-nil ... --skip-ssh --skip-gh` でカテゴリを作成 (SSH 鍵と gh 認証は kra カテゴリと共有)
- carbon-nil/atcoder-library (public) と carbon-nil/atcoder-solutions (private) を `gh repo create --add-readme` で作成。既定ブランチは master
- library: `cpp/cplib` / `python/cplib` / `rust` (crate cplib) の 3 言語構成 (PR #1)
- solutions: atcoder-docker の ghcr イメージ (3.1.1-light / -full) を使う devcontainer、`lib/` submodule、acc テンプレート (PR #1)
- 提出用の単一ファイル展開 `tools/bundle.py` を Codex に実装させた (使った部分だけを含める)

## 問題点と解決
- git-guard が `git -C`・`git --version`・`2>` 付きの push を拒否 → 素の `git <subcommand>` をリポジトリ内で個別に実行する
- `git clone` は cwd がリポジトリ内だと拒否 → 先に別コマンドでカテゴリディレクトリへ `cd` してから clone
- worktree 分離中は、heredoc や `bash -c` に git 以外の文字列を渡すだけでも拒否されることがある → ファイルは Write ツールで書き、検証スクリプトはジョブの tmp に置いて `bash /verify.sh` の形で渡す。PR 本文は `--body-file`
- 動作確認はホストに rustc がないため、`ghcr.io/carbon-nil/atcoder-docker:3.1.1-light` を `docker run` し、remoteEnv と post-create を再現して `ojt` で 3 言語 AC を確認
- AtCoder は 2025-04 から Cloudflare Turnstile を導入しており、oj / acc からの提出は通らない前提。提出コマンドは作らず、展開したファイルをブラウザに貼る
- AtCoder の C++ は GCC 15.2.0 `-std=gnu++23` (2025-10 新ジャッジ)。atcoder-docker の `ojt` は `g++` (13) で gnu++17 になるので、イメージ側の更新が未解決の課題
