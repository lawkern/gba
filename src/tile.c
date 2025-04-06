/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

typedef struct {
   u32 data[8];
} tile4;

typedef struct {
   u32 data[16];
} tile8;

typedef tile4 charblock4[512];
typedef tile8 charblock8[256];

#define TILE4_MEMORY ((charblock4 *)0x06000000)
#define TILE8_MEMORY ((charblock8 *)0x06000000)

enum
{
   ATTR0_REGULAR      = 0x0,
   ATTR0_AFFINE       = 0x1,
   ATTR0_HIDE         = 0x2,
   ATTR0_AFFINEDOUBLE = 0x3,
};

typedef struct align(4) {
   u16 attribute0;
   u16 attribute1;
   u16 attribute2;
   s16 padding;
} object_attributes;

typedef struct align(4) {
   u16 padding0[3];
   s16 pa;

   u16 padding1[3];
   s16 pb;

   u16 padding2[3];
   s16 pc;

   u16 padding3[3];
   s16 pd;
} object_affine;
