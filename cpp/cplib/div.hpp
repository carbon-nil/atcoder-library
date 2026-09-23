#pragma once

namespace cplib {

// 負数でも数学的な floor(a / b) / ceil(a / b) を返す (b != 0)
constexpr long long floor_div(long long a, long long b) {
    return a / b - (a % b != 0 && (a < 0) != (b < 0));
}
constexpr long long ceil_div(long long a, long long b) {
    return a / b + (a % b != 0 && (a < 0) == (b < 0));
}

static_assert(floor_div(-7, 2) == -4 && floor_div(7, -2) == -4 && floor_div(6, 3) == 2);
static_assert(ceil_div(-7, 2) == -3 && ceil_div(7, 2) == 4 && ceil_div(-7, -2) == 4);

}  // namespace cplib
