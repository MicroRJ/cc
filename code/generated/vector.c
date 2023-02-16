#define f64 double
#define f32 float
#ifdef _MSC_VER
# define i64 signed   __int64
# define u64 unsigned __int64
# define i32 signed   __int32
# define u32 unsigned __int32
# define i16 signed   __int16
# define u16 unsigned __int16
# define i8  signed   __int8
# define u8  unsigned __int8
#else
# define i64 signed   long long
# define u64 unsigned long long
# define i32 signed   int
# define u32 unsigned int
# define i16 signed   short
# define u16 unsigned short
# define i8  signed   char
# define u8  unsigned char
#endif
#define c8  char
#define c16 wchar_t
typedef struct f32x2
{
  f32 x;
  f32 y;
} f32x2;
static f32x2
f32x2m(f32 x,f32 y)
{ f32x2 r;
  r.x=x;
  r.y=y;
  return r;
}
static f32x2 operator * (f32x2 a,f32x2 b)
{ f32x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static f32x2 operator *= (f32x2 &a,f32x2 b)
{ f32x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static f32x2 operator / (f32x2 a,f32x2 b)
{ f32x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static f32x2 operator /= (f32x2 &a,f32x2 b)
{ f32x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static f32x2 operator + (f32x2 a,f32x2 b)
{ f32x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static f32x2 operator += (f32x2 &a,f32x2 b)
{ f32x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static f32x2 operator - (f32x2 a,f32x2 b)
{ f32x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
static f32x2 operator -= (f32x2 &a,f32x2 b)
{ f32x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
typedef struct f32x3
{
  f32 x;
  f32 y;
  f32 z;
} f32x3;
static f32x3
f32x3m(f32 x,f32 y,f32 z)
{ f32x3 r;
  r.x=x;
  r.y=y;
  r.z=z;
  return r;
}
static f32x3 operator * (f32x3 a,f32x3 b)
{ f32x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static f32x3 operator *= (f32x3 &a,f32x3 b)
{ f32x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static f32x3 operator / (f32x3 a,f32x3 b)
{ f32x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static f32x3 operator /= (f32x3 &a,f32x3 b)
{ f32x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static f32x3 operator + (f32x3 a,f32x3 b)
{ f32x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static f32x3 operator += (f32x3 &a,f32x3 b)
{ f32x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static f32x3 operator - (f32x3 a,f32x3 b)
{ f32x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
static f32x3 operator -= (f32x3 &a,f32x3 b)
{ f32x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
typedef struct f32x4
{
  f32 x;
  f32 y;
  f32 z;
  f32 w;
} f32x4;
static f32x4
f32x4m(f32 x,f32 y,f32 z,f32 w)
{ f32x4 r;
  r.x=x;
  r.y=y;
  r.z=z;
  r.w=w;
  return r;
}
static f32x4 operator * (f32x4 a,f32x4 b)
{ f32x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static f32x4 operator *= (f32x4 &a,f32x4 b)
{ f32x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static f32x4 operator / (f32x4 a,f32x4 b)
{ f32x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static f32x4 operator /= (f32x4 &a,f32x4 b)
{ f32x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static f32x4 operator + (f32x4 a,f32x4 b)
{ f32x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static f32x4 operator += (f32x4 &a,f32x4 b)
{ f32x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static f32x4 operator - (f32x4 a,f32x4 b)
{ f32x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
static f32x4 operator -= (f32x4 &a,f32x4 b)
{ f32x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
typedef struct f64x2
{
  f64 x;
  f64 y;
} f64x2;
static f64x2
f64x2m(f64 x,f64 y)
{ f64x2 r;
  r.x=x;
  r.y=y;
  return r;
}
static f64x2 operator * (f64x2 a,f64x2 b)
{ f64x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static f64x2 operator *= (f64x2 &a,f64x2 b)
{ f64x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static f64x2 operator / (f64x2 a,f64x2 b)
{ f64x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static f64x2 operator /= (f64x2 &a,f64x2 b)
{ f64x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static f64x2 operator + (f64x2 a,f64x2 b)
{ f64x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static f64x2 operator += (f64x2 &a,f64x2 b)
{ f64x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static f64x2 operator - (f64x2 a,f64x2 b)
{ f64x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
static f64x2 operator -= (f64x2 &a,f64x2 b)
{ f64x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
typedef struct f64x3
{
  f64 x;
  f64 y;
  f64 z;
} f64x3;
static f64x3
f64x3m(f64 x,f64 y,f64 z)
{ f64x3 r;
  r.x=x;
  r.y=y;
  r.z=z;
  return r;
}
static f64x3 operator * (f64x3 a,f64x3 b)
{ f64x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static f64x3 operator *= (f64x3 &a,f64x3 b)
{ f64x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static f64x3 operator / (f64x3 a,f64x3 b)
{ f64x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static f64x3 operator /= (f64x3 &a,f64x3 b)
{ f64x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static f64x3 operator + (f64x3 a,f64x3 b)
{ f64x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static f64x3 operator += (f64x3 &a,f64x3 b)
{ f64x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static f64x3 operator - (f64x3 a,f64x3 b)
{ f64x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
static f64x3 operator -= (f64x3 &a,f64x3 b)
{ f64x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
typedef struct f64x4
{
  f64 x;
  f64 y;
  f64 z;
  f64 w;
} f64x4;
static f64x4
f64x4m(f64 x,f64 y,f64 z,f64 w)
{ f64x4 r;
  r.x=x;
  r.y=y;
  r.z=z;
  r.w=w;
  return r;
}
static f64x4 operator * (f64x4 a,f64x4 b)
{ f64x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static f64x4 operator *= (f64x4 &a,f64x4 b)
{ f64x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static f64x4 operator / (f64x4 a,f64x4 b)
{ f64x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static f64x4 operator /= (f64x4 &a,f64x4 b)
{ f64x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static f64x4 operator + (f64x4 a,f64x4 b)
{ f64x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static f64x4 operator += (f64x4 &a,f64x4 b)
{ f64x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static f64x4 operator - (f64x4 a,f64x4 b)
{ f64x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
static f64x4 operator -= (f64x4 &a,f64x4 b)
{ f64x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
typedef struct i8x2
{
  i8 x;
  i8 y;
} i8x2;
static i8x2
i8x2m(i8 x,i8 y)
{ i8x2 r;
  r.x=x;
  r.y=y;
  return r;
}
static i8x2 operator * (i8x2 a,i8x2 b)
{ i8x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static i8x2 operator *= (i8x2 &a,i8x2 b)
{ i8x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static i8x2 operator / (i8x2 a,i8x2 b)
{ i8x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static i8x2 operator /= (i8x2 &a,i8x2 b)
{ i8x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static i8x2 operator + (i8x2 a,i8x2 b)
{ i8x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static i8x2 operator += (i8x2 &a,i8x2 b)
{ i8x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static i8x2 operator - (i8x2 a,i8x2 b)
{ i8x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
static i8x2 operator -= (i8x2 &a,i8x2 b)
{ i8x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
typedef struct i8x3
{
  i8 x;
  i8 y;
  i8 z;
} i8x3;
static i8x3
i8x3m(i8 x,i8 y,i8 z)
{ i8x3 r;
  r.x=x;
  r.y=y;
  r.z=z;
  return r;
}
static i8x3 operator * (i8x3 a,i8x3 b)
{ i8x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static i8x3 operator *= (i8x3 &a,i8x3 b)
{ i8x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static i8x3 operator / (i8x3 a,i8x3 b)
{ i8x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static i8x3 operator /= (i8x3 &a,i8x3 b)
{ i8x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static i8x3 operator + (i8x3 a,i8x3 b)
{ i8x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static i8x3 operator += (i8x3 &a,i8x3 b)
{ i8x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static i8x3 operator - (i8x3 a,i8x3 b)
{ i8x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
static i8x3 operator -= (i8x3 &a,i8x3 b)
{ i8x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
typedef struct i8x4
{
  i8 x;
  i8 y;
  i8 z;
  i8 w;
} i8x4;
static i8x4
i8x4m(i8 x,i8 y,i8 z,i8 w)
{ i8x4 r;
  r.x=x;
  r.y=y;
  r.z=z;
  r.w=w;
  return r;
}
static i8x4 operator * (i8x4 a,i8x4 b)
{ i8x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static i8x4 operator *= (i8x4 &a,i8x4 b)
{ i8x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static i8x4 operator / (i8x4 a,i8x4 b)
{ i8x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static i8x4 operator /= (i8x4 &a,i8x4 b)
{ i8x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static i8x4 operator + (i8x4 a,i8x4 b)
{ i8x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static i8x4 operator += (i8x4 &a,i8x4 b)
{ i8x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static i8x4 operator - (i8x4 a,i8x4 b)
{ i8x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
static i8x4 operator -= (i8x4 &a,i8x4 b)
{ i8x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
typedef struct i16x2
{
  i16 x;
  i16 y;
} i16x2;
static i16x2
i16x2m(i16 x,i16 y)
{ i16x2 r;
  r.x=x;
  r.y=y;
  return r;
}
static i16x2 operator * (i16x2 a,i16x2 b)
{ i16x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static i16x2 operator *= (i16x2 &a,i16x2 b)
{ i16x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static i16x2 operator / (i16x2 a,i16x2 b)
{ i16x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static i16x2 operator /= (i16x2 &a,i16x2 b)
{ i16x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static i16x2 operator + (i16x2 a,i16x2 b)
{ i16x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static i16x2 operator += (i16x2 &a,i16x2 b)
{ i16x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static i16x2 operator - (i16x2 a,i16x2 b)
{ i16x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
static i16x2 operator -= (i16x2 &a,i16x2 b)
{ i16x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
typedef struct i16x3
{
  i16 x;
  i16 y;
  i16 z;
} i16x3;
static i16x3
i16x3m(i16 x,i16 y,i16 z)
{ i16x3 r;
  r.x=x;
  r.y=y;
  r.z=z;
  return r;
}
static i16x3 operator * (i16x3 a,i16x3 b)
{ i16x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static i16x3 operator *= (i16x3 &a,i16x3 b)
{ i16x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static i16x3 operator / (i16x3 a,i16x3 b)
{ i16x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static i16x3 operator /= (i16x3 &a,i16x3 b)
{ i16x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static i16x3 operator + (i16x3 a,i16x3 b)
{ i16x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static i16x3 operator += (i16x3 &a,i16x3 b)
{ i16x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static i16x3 operator - (i16x3 a,i16x3 b)
{ i16x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
static i16x3 operator -= (i16x3 &a,i16x3 b)
{ i16x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
typedef struct i16x4
{
  i16 x;
  i16 y;
  i16 z;
  i16 w;
} i16x4;
static i16x4
i16x4m(i16 x,i16 y,i16 z,i16 w)
{ i16x4 r;
  r.x=x;
  r.y=y;
  r.z=z;
  r.w=w;
  return r;
}
static i16x4 operator * (i16x4 a,i16x4 b)
{ i16x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static i16x4 operator *= (i16x4 &a,i16x4 b)
{ i16x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static i16x4 operator / (i16x4 a,i16x4 b)
{ i16x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static i16x4 operator /= (i16x4 &a,i16x4 b)
{ i16x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static i16x4 operator + (i16x4 a,i16x4 b)
{ i16x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static i16x4 operator += (i16x4 &a,i16x4 b)
{ i16x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static i16x4 operator - (i16x4 a,i16x4 b)
{ i16x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
static i16x4 operator -= (i16x4 &a,i16x4 b)
{ i16x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
typedef struct i32x2
{
  i32 x;
  i32 y;
} i32x2;
static i32x2
i32x2m(i32 x,i32 y)
{ i32x2 r;
  r.x=x;
  r.y=y;
  return r;
}
static i32x2 operator * (i32x2 a,i32x2 b)
{ i32x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static i32x2 operator *= (i32x2 &a,i32x2 b)
{ i32x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static i32x2 operator / (i32x2 a,i32x2 b)
{ i32x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static i32x2 operator /= (i32x2 &a,i32x2 b)
{ i32x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static i32x2 operator + (i32x2 a,i32x2 b)
{ i32x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static i32x2 operator += (i32x2 &a,i32x2 b)
{ i32x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static i32x2 operator - (i32x2 a,i32x2 b)
{ i32x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
static i32x2 operator -= (i32x2 &a,i32x2 b)
{ i32x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
typedef struct i32x3
{
  i32 x;
  i32 y;
  i32 z;
} i32x3;
static i32x3
i32x3m(i32 x,i32 y,i32 z)
{ i32x3 r;
  r.x=x;
  r.y=y;
  r.z=z;
  return r;
}
static i32x3 operator * (i32x3 a,i32x3 b)
{ i32x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static i32x3 operator *= (i32x3 &a,i32x3 b)
{ i32x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static i32x3 operator / (i32x3 a,i32x3 b)
{ i32x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static i32x3 operator /= (i32x3 &a,i32x3 b)
{ i32x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static i32x3 operator + (i32x3 a,i32x3 b)
{ i32x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static i32x3 operator += (i32x3 &a,i32x3 b)
{ i32x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static i32x3 operator - (i32x3 a,i32x3 b)
{ i32x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
static i32x3 operator -= (i32x3 &a,i32x3 b)
{ i32x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
typedef struct i32x4
{
  i32 x;
  i32 y;
  i32 z;
  i32 w;
} i32x4;
static i32x4
i32x4m(i32 x,i32 y,i32 z,i32 w)
{ i32x4 r;
  r.x=x;
  r.y=y;
  r.z=z;
  r.w=w;
  return r;
}
static i32x4 operator * (i32x4 a,i32x4 b)
{ i32x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static i32x4 operator *= (i32x4 &a,i32x4 b)
{ i32x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static i32x4 operator / (i32x4 a,i32x4 b)
{ i32x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static i32x4 operator /= (i32x4 &a,i32x4 b)
{ i32x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static i32x4 operator + (i32x4 a,i32x4 b)
{ i32x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static i32x4 operator += (i32x4 &a,i32x4 b)
{ i32x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static i32x4 operator - (i32x4 a,i32x4 b)
{ i32x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
static i32x4 operator -= (i32x4 &a,i32x4 b)
{ i32x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
typedef struct i64x2
{
  i64 x;
  i64 y;
} i64x2;
static i64x2
i64x2m(i64 x,i64 y)
{ i64x2 r;
  r.x=x;
  r.y=y;
  return r;
}
static i64x2 operator * (i64x2 a,i64x2 b)
{ i64x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static i64x2 operator *= (i64x2 &a,i64x2 b)
{ i64x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static i64x2 operator / (i64x2 a,i64x2 b)
{ i64x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static i64x2 operator /= (i64x2 &a,i64x2 b)
{ i64x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static i64x2 operator + (i64x2 a,i64x2 b)
{ i64x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static i64x2 operator += (i64x2 &a,i64x2 b)
{ i64x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static i64x2 operator - (i64x2 a,i64x2 b)
{ i64x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
static i64x2 operator -= (i64x2 &a,i64x2 b)
{ i64x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
typedef struct i64x3
{
  i64 x;
  i64 y;
  i64 z;
} i64x3;
static i64x3
i64x3m(i64 x,i64 y,i64 z)
{ i64x3 r;
  r.x=x;
  r.y=y;
  r.z=z;
  return r;
}
static i64x3 operator * (i64x3 a,i64x3 b)
{ i64x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static i64x3 operator *= (i64x3 &a,i64x3 b)
{ i64x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static i64x3 operator / (i64x3 a,i64x3 b)
{ i64x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static i64x3 operator /= (i64x3 &a,i64x3 b)
{ i64x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static i64x3 operator + (i64x3 a,i64x3 b)
{ i64x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static i64x3 operator += (i64x3 &a,i64x3 b)
{ i64x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static i64x3 operator - (i64x3 a,i64x3 b)
{ i64x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
static i64x3 operator -= (i64x3 &a,i64x3 b)
{ i64x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
typedef struct i64x4
{
  i64 x;
  i64 y;
  i64 z;
  i64 w;
} i64x4;
static i64x4
i64x4m(i64 x,i64 y,i64 z,i64 w)
{ i64x4 r;
  r.x=x;
  r.y=y;
  r.z=z;
  r.w=w;
  return r;
}
static i64x4 operator * (i64x4 a,i64x4 b)
{ i64x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static i64x4 operator *= (i64x4 &a,i64x4 b)
{ i64x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static i64x4 operator / (i64x4 a,i64x4 b)
{ i64x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static i64x4 operator /= (i64x4 &a,i64x4 b)
{ i64x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static i64x4 operator + (i64x4 a,i64x4 b)
{ i64x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static i64x4 operator += (i64x4 &a,i64x4 b)
{ i64x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static i64x4 operator - (i64x4 a,i64x4 b)
{ i64x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
static i64x4 operator -= (i64x4 &a,i64x4 b)
{ i64x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
typedef struct u8x2
{
  u8 x;
  u8 y;
} u8x2;
static u8x2
u8x2m(u8 x,u8 y)
{ u8x2 r;
  r.x=x;
  r.y=y;
  return r;
}
static u8x2 operator * (u8x2 a,u8x2 b)
{ u8x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static u8x2 operator *= (u8x2 &a,u8x2 b)
{ u8x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static u8x2 operator / (u8x2 a,u8x2 b)
{ u8x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static u8x2 operator /= (u8x2 &a,u8x2 b)
{ u8x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static u8x2 operator + (u8x2 a,u8x2 b)
{ u8x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static u8x2 operator += (u8x2 &a,u8x2 b)
{ u8x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static u8x2 operator - (u8x2 a,u8x2 b)
{ u8x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
static u8x2 operator -= (u8x2 &a,u8x2 b)
{ u8x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
typedef struct u8x3
{
  u8 x;
  u8 y;
  u8 z;
} u8x3;
static u8x3
u8x3m(u8 x,u8 y,u8 z)
{ u8x3 r;
  r.x=x;
  r.y=y;
  r.z=z;
  return r;
}
static u8x3 operator * (u8x3 a,u8x3 b)
{ u8x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static u8x3 operator *= (u8x3 &a,u8x3 b)
{ u8x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static u8x3 operator / (u8x3 a,u8x3 b)
{ u8x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static u8x3 operator /= (u8x3 &a,u8x3 b)
{ u8x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static u8x3 operator + (u8x3 a,u8x3 b)
{ u8x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static u8x3 operator += (u8x3 &a,u8x3 b)
{ u8x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static u8x3 operator - (u8x3 a,u8x3 b)
{ u8x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
static u8x3 operator -= (u8x3 &a,u8x3 b)
{ u8x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
typedef struct u8x4
{
  u8 x;
  u8 y;
  u8 z;
  u8 w;
} u8x4;
static u8x4
u8x4m(u8 x,u8 y,u8 z,u8 w)
{ u8x4 r;
  r.x=x;
  r.y=y;
  r.z=z;
  r.w=w;
  return r;
}
static u8x4 operator * (u8x4 a,u8x4 b)
{ u8x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static u8x4 operator *= (u8x4 &a,u8x4 b)
{ u8x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static u8x4 operator / (u8x4 a,u8x4 b)
{ u8x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static u8x4 operator /= (u8x4 &a,u8x4 b)
{ u8x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static u8x4 operator + (u8x4 a,u8x4 b)
{ u8x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static u8x4 operator += (u8x4 &a,u8x4 b)
{ u8x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static u8x4 operator - (u8x4 a,u8x4 b)
{ u8x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
static u8x4 operator -= (u8x4 &a,u8x4 b)
{ u8x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
typedef struct u16x2
{
  u16 x;
  u16 y;
} u16x2;
static u16x2
u16x2m(u16 x,u16 y)
{ u16x2 r;
  r.x=x;
  r.y=y;
  return r;
}
static u16x2 operator * (u16x2 a,u16x2 b)
{ u16x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static u16x2 operator *= (u16x2 &a,u16x2 b)
{ u16x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static u16x2 operator / (u16x2 a,u16x2 b)
{ u16x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static u16x2 operator /= (u16x2 &a,u16x2 b)
{ u16x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static u16x2 operator + (u16x2 a,u16x2 b)
{ u16x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static u16x2 operator += (u16x2 &a,u16x2 b)
{ u16x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static u16x2 operator - (u16x2 a,u16x2 b)
{ u16x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
static u16x2 operator -= (u16x2 &a,u16x2 b)
{ u16x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
typedef struct u16x3
{
  u16 x;
  u16 y;
  u16 z;
} u16x3;
static u16x3
u16x3m(u16 x,u16 y,u16 z)
{ u16x3 r;
  r.x=x;
  r.y=y;
  r.z=z;
  return r;
}
static u16x3 operator * (u16x3 a,u16x3 b)
{ u16x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static u16x3 operator *= (u16x3 &a,u16x3 b)
{ u16x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static u16x3 operator / (u16x3 a,u16x3 b)
{ u16x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static u16x3 operator /= (u16x3 &a,u16x3 b)
{ u16x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static u16x3 operator + (u16x3 a,u16x3 b)
{ u16x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static u16x3 operator += (u16x3 &a,u16x3 b)
{ u16x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static u16x3 operator - (u16x3 a,u16x3 b)
{ u16x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
static u16x3 operator -= (u16x3 &a,u16x3 b)
{ u16x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
typedef struct u16x4
{
  u16 x;
  u16 y;
  u16 z;
  u16 w;
} u16x4;
static u16x4
u16x4m(u16 x,u16 y,u16 z,u16 w)
{ u16x4 r;
  r.x=x;
  r.y=y;
  r.z=z;
  r.w=w;
  return r;
}
static u16x4 operator * (u16x4 a,u16x4 b)
{ u16x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static u16x4 operator *= (u16x4 &a,u16x4 b)
{ u16x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static u16x4 operator / (u16x4 a,u16x4 b)
{ u16x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static u16x4 operator /= (u16x4 &a,u16x4 b)
{ u16x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static u16x4 operator + (u16x4 a,u16x4 b)
{ u16x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static u16x4 operator += (u16x4 &a,u16x4 b)
{ u16x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static u16x4 operator - (u16x4 a,u16x4 b)
{ u16x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
static u16x4 operator -= (u16x4 &a,u16x4 b)
{ u16x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
typedef struct u32x2
{
  u32 x;
  u32 y;
} u32x2;
static u32x2
u32x2m(u32 x,u32 y)
{ u32x2 r;
  r.x=x;
  r.y=y;
  return r;
}
static u32x2 operator * (u32x2 a,u32x2 b)
{ u32x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static u32x2 operator *= (u32x2 &a,u32x2 b)
{ u32x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static u32x2 operator / (u32x2 a,u32x2 b)
{ u32x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static u32x2 operator /= (u32x2 &a,u32x2 b)
{ u32x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static u32x2 operator + (u32x2 a,u32x2 b)
{ u32x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static u32x2 operator += (u32x2 &a,u32x2 b)
{ u32x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static u32x2 operator - (u32x2 a,u32x2 b)
{ u32x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
static u32x2 operator -= (u32x2 &a,u32x2 b)
{ u32x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
typedef struct u32x3
{
  u32 x;
  u32 y;
  u32 z;
} u32x3;
static u32x3
u32x3m(u32 x,u32 y,u32 z)
{ u32x3 r;
  r.x=x;
  r.y=y;
  r.z=z;
  return r;
}
static u32x3 operator * (u32x3 a,u32x3 b)
{ u32x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static u32x3 operator *= (u32x3 &a,u32x3 b)
{ u32x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static u32x3 operator / (u32x3 a,u32x3 b)
{ u32x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static u32x3 operator /= (u32x3 &a,u32x3 b)
{ u32x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static u32x3 operator + (u32x3 a,u32x3 b)
{ u32x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static u32x3 operator += (u32x3 &a,u32x3 b)
{ u32x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static u32x3 operator - (u32x3 a,u32x3 b)
{ u32x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
static u32x3 operator -= (u32x3 &a,u32x3 b)
{ u32x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
typedef struct u32x4
{
  u32 x;
  u32 y;
  u32 z;
  u32 w;
} u32x4;
static u32x4
u32x4m(u32 x,u32 y,u32 z,u32 w)
{ u32x4 r;
  r.x=x;
  r.y=y;
  r.z=z;
  r.w=w;
  return r;
}
static u32x4 operator * (u32x4 a,u32x4 b)
{ u32x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static u32x4 operator *= (u32x4 &a,u32x4 b)
{ u32x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static u32x4 operator / (u32x4 a,u32x4 b)
{ u32x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static u32x4 operator /= (u32x4 &a,u32x4 b)
{ u32x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static u32x4 operator + (u32x4 a,u32x4 b)
{ u32x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static u32x4 operator += (u32x4 &a,u32x4 b)
{ u32x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static u32x4 operator - (u32x4 a,u32x4 b)
{ u32x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
static u32x4 operator -= (u32x4 &a,u32x4 b)
{ u32x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
typedef struct u64x2
{
  u64 x;
  u64 y;
} u64x2;
static u64x2
u64x2m(u64 x,u64 y)
{ u64x2 r;
  r.x=x;
  r.y=y;
  return r;
}
static u64x2 operator * (u64x2 a,u64x2 b)
{ u64x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static u64x2 operator *= (u64x2 &a,u64x2 b)
{ u64x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static u64x2 operator / (u64x2 a,u64x2 b)
{ u64x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static u64x2 operator /= (u64x2 &a,u64x2 b)
{ u64x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static u64x2 operator + (u64x2 a,u64x2 b)
{ u64x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static u64x2 operator += (u64x2 &a,u64x2 b)
{ u64x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static u64x2 operator - (u64x2 a,u64x2 b)
{ u64x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
static u64x2 operator -= (u64x2 &a,u64x2 b)
{ u64x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
typedef struct u64x3
{
  u64 x;
  u64 y;
  u64 z;
} u64x3;
static u64x3
u64x3m(u64 x,u64 y,u64 z)
{ u64x3 r;
  r.x=x;
  r.y=y;
  r.z=z;
  return r;
}
static u64x3 operator * (u64x3 a,u64x3 b)
{ u64x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static u64x3 operator *= (u64x3 &a,u64x3 b)
{ u64x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static u64x3 operator / (u64x3 a,u64x3 b)
{ u64x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static u64x3 operator /= (u64x3 &a,u64x3 b)
{ u64x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static u64x3 operator + (u64x3 a,u64x3 b)
{ u64x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static u64x3 operator += (u64x3 &a,u64x3 b)
{ u64x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static u64x3 operator - (u64x3 a,u64x3 b)
{ u64x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
static u64x3 operator -= (u64x3 &a,u64x3 b)
{ u64x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
typedef struct u64x4
{
  u64 x;
  u64 y;
  u64 z;
  u64 w;
} u64x4;
static u64x4
u64x4m(u64 x,u64 y,u64 z,u64 w)
{ u64x4 r;
  r.x=x;
  r.y=y;
  r.z=z;
  r.w=w;
  return r;
}
static u64x4 operator * (u64x4 a,u64x4 b)
{ u64x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static u64x4 operator *= (u64x4 &a,u64x4 b)
{ u64x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static u64x4 operator / (u64x4 a,u64x4 b)
{ u64x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static u64x4 operator /= (u64x4 &a,u64x4 b)
{ u64x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static u64x4 operator + (u64x4 a,u64x4 b)
{ u64x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static u64x4 operator += (u64x4 &a,u64x4 b)
{ u64x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static u64x4 operator - (u64x4 a,u64x4 b)
{ u64x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
static u64x4 operator -= (u64x4 &a,u64x4 b)
{ u64x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
typedef struct c8x2
{
  c8 x;
  c8 y;
} c8x2;
static c8x2
c8x2m(c8 x,c8 y)
{ c8x2 r;
  r.x=x;
  r.y=y;
  return r;
}
static c8x2 operator * (c8x2 a,c8x2 b)
{ c8x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static c8x2 operator *= (c8x2 &a,c8x2 b)
{ c8x2 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  return r;
}
static c8x2 operator / (c8x2 a,c8x2 b)
{ c8x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static c8x2 operator /= (c8x2 &a,c8x2 b)
{ c8x2 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  return r;
}
static c8x2 operator + (c8x2 a,c8x2 b)
{ c8x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static c8x2 operator += (c8x2 &a,c8x2 b)
{ c8x2 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  return r;
}
static c8x2 operator - (c8x2 a,c8x2 b)
{ c8x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
static c8x2 operator -= (c8x2 &a,c8x2 b)
{ c8x2 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  return r;
}
typedef struct c8x3
{
  c8 x;
  c8 y;
  c8 z;
} c8x3;
static c8x3
c8x3m(c8 x,c8 y,c8 z)
{ c8x3 r;
  r.x=x;
  r.y=y;
  r.z=z;
  return r;
}
static c8x3 operator * (c8x3 a,c8x3 b)
{ c8x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static c8x3 operator *= (c8x3 &a,c8x3 b)
{ c8x3 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  return r;
}
static c8x3 operator / (c8x3 a,c8x3 b)
{ c8x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static c8x3 operator /= (c8x3 &a,c8x3 b)
{ c8x3 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  return r;
}
static c8x3 operator + (c8x3 a,c8x3 b)
{ c8x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static c8x3 operator += (c8x3 &a,c8x3 b)
{ c8x3 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  return r;
}
static c8x3 operator - (c8x3 a,c8x3 b)
{ c8x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
static c8x3 operator -= (c8x3 &a,c8x3 b)
{ c8x3 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  return r;
}
typedef struct c8x4
{
  c8 x;
  c8 y;
  c8 z;
  c8 w;
} c8x4;
static c8x4
c8x4m(c8 x,c8 y,c8 z,c8 w)
{ c8x4 r;
  r.x=x;
  r.y=y;
  r.z=z;
  r.w=w;
  return r;
}
static c8x4 operator * (c8x4 a,c8x4 b)
{ c8x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static c8x4 operator *= (c8x4 &a,c8x4 b)
{ c8x4 r;
  r.x=a.x*b.x;
  r.y=a.y*b.y;
  r.z=a.z*b.z;
  r.w=a.w*b.w;
  return r;
}
static c8x4 operator / (c8x4 a,c8x4 b)
{ c8x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static c8x4 operator /= (c8x4 &a,c8x4 b)
{ c8x4 r;
  r.x=a.x/b.x;
  r.y=a.y/b.y;
  r.z=a.z/b.z;
  r.w=a.w/b.w;
  return r;
}
static c8x4 operator + (c8x4 a,c8x4 b)
{ c8x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static c8x4 operator += (c8x4 &a,c8x4 b)
{ c8x4 r;
  r.x=a.x+b.x;
  r.y=a.y+b.y;
  r.z=a.z+b.z;
  r.w=a.w+b.w;
  return r;
}
static c8x4 operator - (c8x4 a,c8x4 b)
{ c8x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
static c8x4 operator -= (c8x4 &a,c8x4 b)
{ c8x4 r;
  r.x=a.x-b.x;
  r.y=a.y-b.y;
  r.z=a.z-b.z;
  r.w=a.w-b.w;
  return r;
}
