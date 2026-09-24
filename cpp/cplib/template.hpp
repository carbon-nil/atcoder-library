#pragma once

// all などのマクロが標準ヘッダの views::all(...) を書き換えないよう、先に標準ヘッダをすべて読む
// (gnu++23 では <queue> なども <ranges> を読む)
#include <bits/stdc++.h>

// 修飾なしで使う共通定義なのでグローバル名前空間に置く
#define rep(i,a,b) for(int i=a;i<b;i++)
#define rrep(i,a,b) for(int i=a;i>=b;i--)
#define fora(x,a) for(auto &x:a)
#define all(x) x.begin(),x.end()

typedef long long ll;
const int inf = INT_MAX / 2;
const ll infl = 1LL << 60;

// a を b で最大化し、更新したかを返す
template<class T> constexpr bool chmax(T &a, const T &b) {
    if (a<b) { a = b; return 1; }
    return 0;
}

// a を b で最小化し、更新したかを返す
template<class T> constexpr bool chmin(T &a, const T &b) {
    if (b<a) { a = b; return 1; }
    return 0;
}

static_assert([] {
    int a = 2;
    return chmax(a, 3) && a == 3 && !chmax(a, 3) && !chmax(a, 1) && a == 3;
}());
static_assert([] {
    int a = 2;
    return chmin(a, 1) && a == 1 && !chmin(a, 1) && !chmin(a, 3) && a == 1;
}());
