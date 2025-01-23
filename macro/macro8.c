#define PRIMITIVE_CONCAT(x, y) x##y
#define CONCAT(x, y) PRIMITIVE_CONCAT(x, y)
#define EMPTY()
#define DEFER(x) x EMPTY()
#define EXPAND(...) __VA_ARGS__

#define GET_SEC(x, n, ...) n
#define CHECK(...) GET_SEC(__VA_ARGS__, 0)
#define PROBE(x) x, 1

#define IS_EMPTY(x, ...) CHECK(CONCAT(PRIMITIVE_CONCAT(IS_EMPTY_, x), EMPTY())())
#define IS_EMPTY_(x) PROBE(x)

#define FOR_EACH(macro, x, ...) \
  CONCAT(FOR_EACH_, IS_EMPTY(__VA_ARGS__))(macro, x __VA_OPT__(, ) __VA_ARGS__)

#define FOR_EACH_0(macro, x, ...) macro(x) DEFER(FOR_EACH_I)()(macro, __VA_ARGS__)
#define FOR_EACH_1(macro, x) macro(x)
#define FOR_EACH_I() FOR_EACH

#define FUNC_PROTO(x) void x(void*, void*, unsigned long);

IS_EMPTY()
IS_EMPTY(1)
IS_EMPTY(0)

EXPAND(EXPAND(EXPAND(FOR_EACH(FUNC_PROTO, x, y, z))))

#define EVAL(...)  EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL4(...) EVAL5(EVAL5(EVAL5(__VA_ARGS__)))
#define EVAL5(...) __VA_ARGS__

EVAL(FOR_EACH(FUNC_PROTO, a, b, c, d, e, f, g, h, i, j, k, l))
