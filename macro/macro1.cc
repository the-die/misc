#define FOO(x, y) x + y
#define BAR(y) 13 + y
#define BAZ(z) FOO(BAZ(16), FOO(11, 0)) + z

BAZ(15)
// BAZ(16) + 11 + 0 + 15

/*
 * BAZ(15) // BAZ:{}
 * |
 * V
 * FOO(BAZ(16), FOO(11, 0)) + 15  // FOO:{BAZ} BAZ:{BAZ} FOO:{BAZ}
 * |
 * V
 * FOO(BAZ(16), 11 + 0) + 15 // FOO:{BAZ} BAZ:{BAZ}
 * |
 * V
 * BAZ(16) + 11 + 0 + 15 // BAZ:{BAZ, FOO}
 */
