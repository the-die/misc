#define EMPTY()
#define DEFER(id) id EMPTY()
#define EXPAND(...) __VA_ARGS__
#define BAR_I() BAR
#define BAR() DEFER(BAR_I)()() 1

// BAR()
// BAR_I ()() 1

/*
 * BAR()
 * |
 * v
 * DEFER(BAR_I)()() 1 // DEFER:{BAR} BAR_I:{BAR}
 * |
 * v
 * BAR_I EMPTY()()() 1 // BAR_I:{BAR, DEFER} EMPTY:{DEFER)}
 * |
 * v
 * BAR_I ()() 1 // BAR_I:{BAR, DEFER}
 */

EXPAND(BAR())
// BAR_I ()() 1 1

/*
 * EXPAND(BAR())
 * |
 * v
 * EXPAND(DEFER(BAR_I)()() 1) // DEFER:{BAR} BAR_I:{BAR}
 * |
 * v
 * EXPAND(BAR_I EMPTY()()() 1) // BAR_I:{BAR, DEFER} EMPTY:{DEFER}
 * |
 * V
 * EXPAND(BAR_I ()() 1) // BAR_I:{BAR, DEFER}
 * |
 * V
 * BAR_I ()() 1 // BAR_I:{BAR, DEFER, EXPAND}
 * |
 * V
 * `BAR`() 1 // BAR:{}
 * |
 * v
 * ...
 * |
 * V
 * BAR_I ()() 1 1
 */

EXPAND(EXPAND(BAR()))
// BAR_I ()() 1 1 1

/*
 * EXPAND(EXPAND(BAR()))
 * |
 * v
 * ...
 * |
 * v
 * BAR_I ()() 1 1
 * |
 * v
 * BAR() 1 1
 * |
 * v
 * ...
 * |
 * v
 * BAR_I ()() 1 1 1
 */
