#define FOO_(x) FOO_1(x)
#define FOO_1(x) FOO_2(x) + 1
#define FOO_2(x) FOO_1(x) - 1

#define BAR(x) FOO_ ## x (12) FOO_2

BAR(1) (5)
// FOO_1(12) - 1 + 1 FOO_2(5) + 1 - 1

/*
 * BAR(1) (5)
 * |
 * v
 * `FOO_ ## 1 (12) FOO_2` (5)
 * |
 * V
 * `FOO_1 (12) FOO_2` (5) // FOO_1:{BAR} FOO_2:{}
 * |
 * v
 * `FOO_2(12) + 1 FOO_2` (5) // FOO2:{BAR, FOO_1} FOO_2:{}
 * |
 * v
 * `F00_1(12) - 1 + 1 FOO_2` (5) // FOO_1:{BAR, FOO_1, FOO_2} FOO_2:{}
 * |
 * v
 * F00_1(12) - 1 + 1 FOO_1(5) - 1 // FOO_1:{BAR, FOO_1, FOO_2} FOO_1:{FOO_2}
 * |
 * v
 * F00_1(12) - 1 + 1 FOO_2(5) + 1 - 1 // FOO_1:{BAR, FOO_1, FOO_2} FOO_2:{FOO_2, FOO_1}
 */
