#pragma once

#include <cplib/template.hpp>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

// 重みなしグラフの始点 s からの最短距離を返す (未到達は -1)
inline std::vector<int> bfs(const std::vector<std::vector<int>>& g, int s) {
    std::vector<int> dist(g.size(), -1);
    std::queue<int> q;
    dist[s] = 0;
    q.push(s);
    while (!q.empty()) {
        int pos = q.front();
        q.pop();
        fora(nex, g[pos]) {
            if (dist[nex] == -1) {
                dist[nex] = dist[pos] + 1;
                q.push(nex);
            }
        }
    }
    return dist;
}

// 非負重みグラフの始点 s からの最短距離を返す (未到達は -1)
inline std::vector<ll> dijkstra(const std::vector<std::vector<std::pair<int, ll>>>& g, int s) {
    std::vector<ll> dist(g.size(), infl);
    std::priority_queue<std::pair<ll, int>, std::vector<std::pair<ll, int>>, std::greater<>> pq;
    dist[s] = 0;
    pq.push({0, s});
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        fora(p, g[u]) {
            int v = p.first;
            ll nd = d + p.second;
            if (chmin(dist[v], nd)) pq.push({nd, v});
        }
    }
    fora(d, dist) {
        if (d == infl) d = -1;
    }
    return dist;
}
