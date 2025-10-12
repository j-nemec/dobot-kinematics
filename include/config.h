#ifndef CONFIG_H
#define CONFIG_H

/* Hodnoty získány z dokumentace: Dobot Magician User Guide v2.0 */

/* Geometrie */
#define L1 135.0   /* mm */
#define L2 147.0   /* mm */

/* Výška osy J1 nad podložkou (Z) */
#define BASE_Z 138.0  /* mm */
/* J1 - rotace základny 180° */
#define J1_MIN_DEG   (-90.0)
#define J1_MAX_DEG   (+90.0)
/* J2 - rameno L1 */
#define J2_MIN_DEG     (0.0)
#define J2_MAX_DEG    (+85.0)
/* J3 - rameno L2 */
#define J3_MIN_DEG   (-10.0)
#define J3_MAX_DEG   (+90.0)
/* J4 - rotace nástroje */
#define J4_MIN_DEG   (-90.0)
#define J4_MAX_DEG   (+90.0)

/* Tolerance a dosah */
#define EPS_POS_MM   (1e-2) // Poloha 0.01 mm
#define EPS_ANG_RAD  (1e-6) // Úhel cca 0.000057°

/* Experimentální test */
//#define REACH_MIN_MM ( (L1)>(L2) ? (L1)-(L2) : (L2)-(L1) )
//#define REACH_MAX_MM ( (L1)+(L2) )

#endif /* CONFIG_H */
