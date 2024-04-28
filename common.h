
#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

#define Fi(n,a...) for(u32 i=0;i<n;i++){a;}
#define Fj(n,a...) for(u32 j=n;j>0;j--){a;}
#define Fx(n,a...) for(u32 x=0;x<n;x++){a;}
#define If(n,a...) if(n){a;}
#define Else(a...) else{a;}

#endif

