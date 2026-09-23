/// 負数でも数学的な floor(a / b) / ceil(a / b) を返す (b != 0)
pub fn floor_div(a: i64, b: i64) -> i64 {
    a / b - (a % b != 0 && (a < 0) != (b < 0)) as i64
}

pub fn ceil_div(a: i64, b: i64) -> i64 {
    a / b + (a % b != 0 && (a < 0) == (b < 0)) as i64
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn signs() {
        assert!(floor_div(-7, 2) == -4 && floor_div(7, -2) == -4 && floor_div(6, 3) == 2);
        assert!(ceil_div(-7, 2) == -3 && ceil_div(7, 2) == 4 && ceil_div(-7, -2) == 4);
    }
}
