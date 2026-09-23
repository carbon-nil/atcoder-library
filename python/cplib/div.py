def floor_div(a: int, b: int) -> int:
    return a // b


def ceil_div(a: int, b: int) -> int:
    return -(-a // b)


if __name__ == "__main__":
    assert floor_div(-7, 2) == -4 and floor_div(7, -2) == -4 and floor_div(6, 3) == 2
    assert ceil_div(-7, 2) == -3 and ceil_div(7, 2) == 4 and ceil_div(-7, -2) == 4
