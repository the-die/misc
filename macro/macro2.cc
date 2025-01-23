#define BAR() 1 BAZ()
#define BAZ() BAR
#define FOO(x) BAR() - x()

FOO(BAR())
// 1 BAR - 1 BAR()

/*
 * FOO(BAR())
 * |
 * v
 * FOO(1 BAZ()) // FOO:{} BAZ:{BAR}
 * |
 * v
 * FOO(1 BAR) // FOO:{} BAR:{BAR, BAZ}
 * |
 * v
 * BAR() - 1 BAR() // BAR:{FOO} BAR:{BAR, BAZ, FOO}
 * |
 * v
 * 1 BAZ() - 1 BAR() // BAZ:{FOO, BAR} BAR:{BAR, BAZ, FOO}
 * |
 * v
 * 1 BAR - 1 BAR() // BAR:{FOO, BAR, BAZ} BAR:{BAR, BAZ, FOO}
 */

FOO(FOO(BAR()))
// 1 BAR - 1 BAR - 1 BAR()()

/*
 * FOO(FOO(BAR()))
 * |
 * v
 * FOO(FOO(1 BAZ())) // FOO:{} FOO:{} BAZ:{BAR}
 * |
 * v
 * FOO(FOO(1 BAR)) // FOO:{} FOO:{} BAR:{BAR, BAZ}
 * |
 * V
 * FOO(BAR() - 1 BAR()) // FOO:{} BAR:{FOO} BAR:{BAR, BAZ, FOO}
 * |
 * v
 * FOO(1 BAZ() - 1 BAR()) // FOO:{} BAZ:{FOO, BAR} BAR:{BAR, BAZ, FOO}
 * |
 * v
 * FOO(1 BAR - 1 BAR()) // FOO:{} BAR:{FOO, BAR, BAZ} BAR:{BAR, BAZ, FOO}
 * |
 * v
 * `BAR() - 1 BAR - 1 BAR()`() // BAR:{FOO} BAR:{FOO, BAR, BAZ} BAR:{BAR, BAZ, FOO}
 * |
 * V
 * 1 BAZ() - 1 BAR - 1 BAR()() // BAZ:{FOO, BAR} BAR:{FOO, BAR, BAZ} BAR:{BAR, BAZ, FOO}
 * |
 * v
 * 1 BAR - 1 BAR - 1 BAR()() // BAR:{FOO, BAR, BAZ} BAR:{FOO, BAR, BAZ} BAR:{BAR, BAZ, FOO}
 */
