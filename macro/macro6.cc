#define EMPTY()
#define DEFER(id) id EMPTY()

#define FOO() macro

#define EXPAND(...) __VA_ARGS__

DEFER(FOO)()
// FOO ()

/*
 * DEFER(FOO)()
 * |
 * v
 * `FOO EMPTY()`() // FOO:{DEFER} EMPTY:{DEFER}
 * |
 * v
 * `FOO `()
 */

EXPAND(DEFER(FOO)())
// macro

/*
 * EXPAND(DEFER(FOO)())
 * |
 * v
 * EXPAND(`FOO EMPTY()`()) // FOO:{DEFER} EMPTY:{DEFER}
 * |
 * V
 * EXPAND(`FOO `()) // FOO:{DEFER}
 * |
 * v
 * FOO () // FOO:{DEFER}
 * |
 * v
 * macro
 */
