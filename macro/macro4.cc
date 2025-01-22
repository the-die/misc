#define BAR_I() BAR
#define BAR() 1 BAR_I()

BAR() ()
// 1 BAR ()

/*
 * BAR() ()
 * |
 * v
 * `1 BAR_I()` () // BAR_I:{BAR}
 * |
 * v
 * `1 BAR` () // BAR:{BAR, BAR_I}
 */
