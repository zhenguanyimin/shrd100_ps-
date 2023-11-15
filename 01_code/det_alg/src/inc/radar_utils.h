
#ifndef RADAR_UTILS_H
#define RADAR_UTILS_H

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#endif /* ARRAY_SIZE */

#ifndef UNUSED
#define UNUSED(var) (void)(var)
#endif /*UNUSED */

#ifndef STATIC
#define STATIC static
#endif /* STATIC */

#ifndef PRIVATE
#define PRIVATE static
#endif /* PRIVATE */

#define U64_H32(val) ((val >> 32) & 0xffffffff)
#define U64_L32(val) (val & 0xffffffff)

#endif /* RADAR_UTILS_H */
