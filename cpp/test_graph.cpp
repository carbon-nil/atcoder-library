// graph.hpp は queue が constexpr にならず static_assert で検査できないので、実行して確かめる
#include <cassert>

#include <cplib/graph.hpp>

int main() {
    std::vector<std::vector<int>> g{{1, 3}, {0, 2}, {1}, {0}, {}};
    assert((bfs(g, 0) == std::vector<int>{0, 1, 2, 1, -1}));

    std::vector<std::vector<std::pair<int, ll>>> w{{{1, 4}, {2, 1}}, {{3, 5}}, {{1, 2}}, {}, {}};
    assert((dijkstra(w, 0) == std::vector<ll>{0, 3, 1, 8, -1}));
}
