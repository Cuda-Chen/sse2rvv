#ifndef SSE2RVV_H
#define SSE2RVV_H

// This header file provides a simple API translation layer
// between SSE intrinsics to their corresponding RVV versions

/*
 * sse2rvv is freely redistributable under the MIT License.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* compiler specific definitions */
#if defined(__GNUC__) || defined(__clang__)
#pragma push_macro("FORCE_INLINE")
#pragma push_macro("ALIGN_STRUCT")
#define FORCE_INLINE static inline __attribute__((always_inline))
#define ALIGN_STRUCT(x) __attribute__((aligned(x)))
#define _sse2rvv_likely(x) __builtin_expect(!!(x), 1)
#define _sse2rvv_unlikely(x) __builtin_expect(!!(x), 0)
#else
#pragma message("Macro name collisions may happen with unsupported compilers.")
#endif

/* C language does not allow initializing a variable with a function call. */
#ifdef __cplusplus
#define _sse2rvv_const static const
#else
#define _sse2rvv_const const
#endif

#include <riscv_vector.h>
#include <stdint.h>
#include <stdlib.h>

/* A few intrinsics accept traditional data types like ints or floats, but
 * most operate on data types that are specific to SSE.
 * If a vector type ends in d, it contains doubles, and if it does not have
 * a suffix, it contains floats. An integer vector type can contain any type
 * of integer, from chars to shorts to unsigned long longs.
 */
typedef vint32m1_t __m64;     /* 64-bit vector containing integers */
typedef vfloat32m1_t __m128;  /* 128-bit vector containing 4 floats */
typedef vfloat64m1_t __m128d; /* 128-bit vector containing 2 doubles */
typedef vint32m1_t __m128i;   /* 128-bit vector containing integers */

// A struct is defined in this header file called 'SIMDVec' which can be used
// by applications which attempt to access the contents of an __m128 struct
// directly.  It is important to note that accessing the __m128 struct directly
// is bad coding practice by Microsoft: @see:
// https://learn.microsoft.com/en-us/cpp/cpp/m128
//
// However, some legacy source code may try to access the contents of an __m128
// struct directly so the developer can use the SIMDVec as an alias for it.  Any
// casting must be done manually by the developer, as you cannot cast or
// otherwise alias the base NEON data type for intrinsic operations.
//
// union intended to allow direct access to an __m128 variable using the names
// that the MSVC compiler provides.  This union should really only be used when
// trying to access the members of the vector as integer values.  GCC/clang
// allow native access to the float members through a simple array access
// operator (in C since 4.6, in C++ since 4.8).
//
// Ideally direct accesses to SIMD vectors should not be used since it can cause
// a performance hit.  If it really is needed however, the original __m128
// variable can be aliased with a pointer to this union and used to access
// individual components.  The use of this union should be hidden behind a macro
// that is used throughout the codebase to access the members instead of always
// declaring this type of variable.
typedef union ALIGN_STRUCT(16) SIMDVec {
  float m128_f32[4];    // as floats - DON'T USE. Added for convenience.
  int8_t m128_i8[16];   // as signed 8-bit integers.
  int16_t m128_i16[8];  // as signed 16-bit integers.
  int32_t m128_i32[4];  // as signed 32-bit integers.
  int64_t m128_i64[2];  // as signed 64-bit integers.
  uint8_t m128_u8[16];  // as unsigned 8-bit integers.
  uint16_t m128_u16[8]; // as unsigned 16-bit integers.
  uint32_t m128_u32[4]; // as unsigned 32-bit integers.
  uint64_t m128_u64[2]; // as unsigned 64-bit integers.
} SIMDVec;

#define vreinterpretq_m128_u8(x)                                               \
  __riscv_vreinterpret_v_u32m1_u8m1(__riscv_vreinterpret_v_f32m1_u32m1(x))
#define vreinterpretq_m128_u16(x)                                              \
  __riscv_vreinterpret_v_u32m1_u16m1(__riscv_vreinterpret_v_f32m1_u32m1(x))
#define vreinterpretq_m128_u32(x) __riscv_vreinterpret_v_f32m1_u32m1(x)
#define vreinterpretq_m128_u64(x)                                              \
  __riscv_vreinterpret_v_f64m1_u64m1(__riscv_vreinterpret_v_f32m1_f64m1(x))
#define vreinterpretq_m128_i8(x)                                               \
  __riscv_vreinterpret_v_i32m1_i8m1(__riscv_vreinterpret_v_f32m1_i32m1(x))
#define vreinterpretq_m128_i16(x)                                              \
  __riscv_vreinterpret_v_i32m1_i16m1(__riscv_vreinterpret_v_f32m1_i32m1(x))
#define vreinterpretq_m128_i32(x) __riscv_vreinterpret_v_f32m1_i32m1(x)
#define vreinterpretq_m128_i64(x)                                              \
  __riscv_vreinterpret_v_i32m1_i64m1(__riscv_vreinterpret_v_f32m1_i32m1(x))
#define vreinterpretq_m128_f32(x) (x)
#define vreinterpretq_m128_f64(x) __riscv_vreinterpret_v_f32m1_f64m1(x)

#define vreinterpretq_u8_m128(x)                                               \
  __riscv_vreinterpret_v_u32m1_f32m1(__riscv_vreinterpret_v_u8m1_u32m1(x))
#define vreinterpretq_u16_m128(x)                                              \
  __riscv_vreinterpret_v_u32m1_f32m1(__riscv_vreinterpret_v_u16m1_u32m1(x))
#define vreinterpretq_u32_m128(x) __riscv_vreinterpret_v_u32m1_f32m1(x)
#define vreinterpretq_u64_m128(x)                                              \
  __riscv_vreinterpret_v_f64m1_f32m1(__riscv_vreinterpret_v_u64m1_f64m1(x))
#define vreinterpretq_i8_m128(x)                                               \
  __riscv_vreinterpret_v_i32m1_f32m1(__riscv_vreinterpret_v_i8m1_i32m1(x))
#define vreinterpretq_i16_m128(x)                                              \
  __riscv_vreinterpret_v_i32m1_f32m1(__riscv_vreinterpret_v_i16m1_i32m1(x))
#define vreinterpretq_i32_m128(x) __riscv_vreinterpret_v_i32m1_f32m1(x)
#define vreinterpretq_i64_m128(x)                                              \
  __riscv_vreinterpret_v_f64m1_f32m1(__riscv_vreinterpret_v_i64m1_f64m1(x))
#define vreinterpretq_f32_m128(x) (x)
#define vreinterpretq_f64_m128(x) __riscv_vreinterpret_v_f64m1_f32m1(x)

#define vreinterpretq_m128d_u8(x)                                              \
  __riscv_vreinterpret_v_u64m1_u8m1(__riscv_vreinterpret_v_f64m1_u64m1(x))
#define vreinterpretq_m128d_u16(x)                                             \
  __riscv_vreinterpret_v_u64m1_u16m1(__riscv_vreinterpret_v_f64m1_u64m1(x))
#define vreinterpretq_m128d_u32(x)                                             \
  __riscv_vreinterpret_v_u64m1_u32m1(__riscv_vreinterpret_v_f64m1_u64m1(x))
#define vreinterpretq_m128d_u64(x) __riscv_vreinterpret_v_f64m1_u64m1(x)
#define vreinterpretq_m128d_i8(x)                                              \
  __riscv_vreinterpret_v_i64m1_i8m1(__riscv_vreinterpret_v_f64m1_i64m1(x))
#define vreinterpretq_m128d_i16(x)                                             \
  __riscv_vreinterpret_v_i64m1_i16m1(__riscv_vreinterpret_v_f64m1_i64m1(x))
#define vreinterpretq_m128d_i32(x)                                             \
  __riscv_vreinterpret_v_i64m1_i32m1(__riscv_vreinterpret_v_f64m1_i64m1(x))
#define vreinterpretq_m128d_i64(x) __riscv_vreinterpret_v_f64m1_i64m1(x)
#define vreinterpretq_m128d_f32(x)                                             \
  __riscv_vreinterpret_v_i32m1_f32m1(__riscv_vreinterpret_v_i64m1_i32m1(       \
      __riscv_vreinterpret_v_f64m1_i64m1(x)))
#define vreinterpretq_m128d_f64(x) (x)

#define vreinterpretq_u8_m128d(x)                                              \
  __riscv_vreinterpret_v_u64m1_f64m1(__riscv_vreinterpret_v_u8m1_u64m1(x))
#define vreinterpretq_u16_m128d(x)                                             \
  __riscv_vreinterpret_v_u64m1_f64m1(__riscv_vreinterpret_v_u16m1_u64m1(x))
#define vreinterpretq_u32_m128d(x)                                             \
  __riscv_vreinterpret_v_u64m1_f64m1(__riscv_vreinterpret_v_u32m1_u64m1(x))
#define vreinterpretq_u64_m128d(x) __riscv_vreinterpret_v_u64m1_f64m1(x)
#define vreinterpretq_i8_m128d(x)                                              \
  __riscv_vreinterpret_v_i64m1_f64m1(__riscv_vreinterpret_v_i8m1_i64m1(x))
#define vreinterpretq_i16_m128d(x)                                             \
  __riscv_vreinterpret_v_i64m1_f64m1(__riscv_vreinterpret_v_i16m1_i64m1(x))
#define vreinterpretq_i32_m128d(x)                                             \
  __riscv_vreinterpret_v_i64m1_f64m1(__riscv_vreinterpret_v_i32m1_i64m1(x))
#define vreinterpretq_i64_m128d(x) __riscv_vreinterpret_v_i64m1_f64m1(x)
#define vreinterpretq_f32_m128d(x)                                             \
  __riscv_vreinterpret_v_i64m1_f64m1(__riscv_vreinterpret_v_i32m1_i64m1(       \
      __riscv_vreinterpret_v_f32m1_i32m1(x)))
#define vreinterpretq_f64_m128d(x) (x)

#define vreinterpretq_m128i_u8(x)                                              \
  __riscv_vreinterpret_v_u32m1_u8m1(__riscv_vreinterpret_v_i32m1_u32m1(x))
#define vreinterpretq_m128i_u16(x)                                             \
  __riscv_vreinterpret_v_u32m1_u16m1(__riscv_vreinterpret_v_i32m1_u32m1(x))
#define vreinterpretq_m128i_u32(x) __riscv_vreinterpret_v_i32m1_u32m1(x)
#define vreinterpretq_m128i_u64(x)                                             \
  __riscv_vreinterpret_v_u32m1_u64m1(__riscv_vreinterpret_v_i32m1_u64m1(x))
#define vreinterpretq_m128i_i8(x) __riscv_vreinterpret_v_i32m1_i8m1(x)
#define vreinterpretq_m128i_i16(x) __riscv_vreinterpret_v_i32m1_i16m1(x)
#define vreinterpretq_m128i_i32(x) (x)
#define vreinterpretq_m128i_i64(x) __riscv_vreinterpret_v_i32m1_i64m1(x)
#define vreinterpretq_m128i_f32(x) __riscv_vreinterpret_v_i32m1_f32m1(x)
#define vreinterpretq_m128i_f64(x)                                             \
  __riscv_vreinterpret_v_f32m1_f64m1(__riscv_vreinterpret_v_i32m1_f32m1(x))

#define vreinterpretq_u8_m128i(x)                                              \
  __riscv_vreinterpret_v_u32m1_i32m1(__riscv_vreinterpret_v_u8m1_u32m1(x))
#define vreinterpretq_u16_m128i(x)                                             \
  __riscv_vreinterpret_v_u32m1_i32m1(__riscv_vreinterpret_v_u16m1_u32m1(x))
#define vreinterpretq_u32_m128i(x) __riscv_vreinterpret_v_u32m1_i32m1(x)
#define vreinterpretq_u64_m128i(x)                                             \
  __riscv_vreinterpret_v_u64m1_i32m1(__riscv_vreinterpret_v_u64m1_u32m1(x))
#define vreinterpretq_i8_m128i(x) __riscv_vreinterpret_v_i8m1_i32m1(x)
#define vreinterpretq_i16_m128i(x) __riscv_vreinterpret_v_i16m1_i32m1(x)
#define vreinterpretq_i32_m128i(x) (x)
#define vreinterpretq_i64_m128i(x) __riscv_vreinterpret_v_i64m1_i32m1(x)
#define vreinterpretq_f32_m128i(x) __riscv_vreinterpret_v_f32m1_i32m1(x)
#define vreinterpretq_f64_m128i(x)                                             \
  __riscv_vreinterpret_v_i64m1_i32m1(__riscv_vreinterpret_v_f64m1_i64m1(x))

#define vreinterpretq_m64_u8(x)                                                \
  __riscv_vreinterpret_v_u32m1_u8m1(__riscv_vreinterpret_v_i32m1_u32m1(x))
#define vreinterpretq_m64_u16(x)                                               \
  __riscv_vreinterpret_v_u32m1_u16m1(__riscv_vreinterpret_v_i32m1_u32m1(x))
#define vreinterpretq_m64_u32(x) __riscv_vreinterpret_v_f32m1_u32m1(x)
#define vreinterpretq_m64_u64(x)                                               \
  __riscv_vreinterpret_v_f64m1_u64m1(__riscv_vreinterpret_v_f32m1_f64m1(x))
#define vreinterpretq_m64_i8(x) __riscv_vreinterpret_v_i32m1_i8m1(x)
#define vreinterpretq_m64_i16(x) __riscv_vreinterpret_v_i32m1_i16m1(x)
#define vreinterpretq_m64_i32(x) (x)
#define vreinterpretq_m64_i64(x) __riscv_vreinterpret_v_i32m1_i64m1(x)
#define vreinterpretq_m64_f32(x) __riscv_vreinterpret_v_i32m1_f32m1(x)
#define vreinterpretq_m64_f64(x)                                               \
  __riscv_vreinterpret_v_i64m1_f64m1(__riscv_vreinterpret_v_i32m1_i64m1(x))

#define vreinterpretq_u8_m64(x)                                                \
  __riscv_vreinterpret_v_u32m1_i32m1(__riscv_vreinterpret_v_u8m1_u32m1(x))
#define vreinterpretq_u16_m64(x)                                               \
  __riscv_vreinterpret_v_u32m1_i32m1(__riscv_vreinterpret_v_u16m1_u32m1(x))
#define vreinterpretq_u32_m64(x) __riscv_vreinterpret_v_u32m1_f32m1(x)
#define vreinterpretq_u64_m64(x)                                               \
  __riscv_vreinterpret_v_f64m1_f32m1(__riscv_vreinterpret_v_u64m1_f64m1(x))
#define vreinterpretq_i8_m64(x) __riscv_vreinterpret_v_i8m1_i32m1(x)
#define vreinterpretq_i16_m64(x) __riscv_vreinterpret_v_i16m1_i32m1(x)
#define vreinterpretq_i32_m64(x) (x)
#define vreinterpretq_i64_m64(x) __riscv_vreinterpret_v_i64m1_i32m1(x)
#define vreinterpretq_f32_m64(x) __riscv_vreinterpret_v_f32m1_i32m1(x)
#define vreinterpretq_f64_m64(x)                                               \
  __riscv_vreinterpret_v_i64m1_i32m1(__riscv_vreinterpret_v_f64m1_i64m1(x))

// __int64 is defined in the Intrinsics Guide which maps to different datatype
// in different data model
#if !(defined(_WIN32) || defined(_WIN64) || defined(__int64))
#if (defined(__x86_64__) || defined(__i386__))
#define __int64 long long
#else
#define __int64 int64_t
#endif
#endif

/* SSE, SSE2, SSE3, SSSE3, SSE4.1, SSE4.2 */

FORCE_INLINE __m128i _mm_abs_epi16(__m128i a) {
  vint16m1_t _a = vreinterpretq_m128i_i16(a);
  vint16m1_t mask = __riscv_vsra_vx_i16m1(_a, 15, 8);
  vint16m1_t a_xor = __riscv_vxor_vv_i16m1(_a, mask, 8);
  return vreinterpretq_i16_m128i(__riscv_vsub_vv_i16m1(a_xor, mask, 8));
}

FORCE_INLINE __m128i _mm_abs_epi32(__m128i a) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t mask = __riscv_vsra_vx_i32m1(_a, 31, 4);
  vint32m1_t a_xor = __riscv_vxor_vv_i32m1(_a, mask, 4);
  return vreinterpretq_i32_m128i(__riscv_vsub_vv_i32m1(a_xor, mask, 4));
}

FORCE_INLINE __m128i _mm_abs_epi8(__m128i a) {
  vint8m1_t _a = vreinterpretq_m128i_i8(a);
  vint8m1_t mask = __riscv_vsra_vx_i8m1(_a, 7, 16);
  vint8m1_t a_xor = __riscv_vxor_vv_i8m1(_a, mask, 16);
  return vreinterpretq_i8_m128i(__riscv_vsub_vv_i8m1(a_xor, mask, 16));
}

FORCE_INLINE __m64 _mm_abs_pi16(__m64 a) {
  vint16m1_t _a = vreinterpretq_m128i_i16(a);
  vint16m1_t mask = __riscv_vsra_vx_i16m1(_a, 15, 4);
  vint16m1_t a_xor = __riscv_vxor_vv_i16m1(_a, mask, 4);
  return vreinterpretq_i16_m128i(__riscv_vsub_vv_i16m1(a_xor, mask, 4));
}

FORCE_INLINE __m64 _mm_abs_pi32(__m64 a) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t mask = __riscv_vsra_vx_i32m1(_a, 31, 2);
  vint32m1_t a_xor = __riscv_vxor_vv_i32m1(_a, mask, 2);
  return vreinterpretq_i32_m128i(__riscv_vsub_vv_i32m1(a_xor, mask, 2));
}

FORCE_INLINE __m64 _mm_abs_pi8(__m64 a) {
  vint8m1_t _a = vreinterpretq_m128i_i8(a);
  vint8m1_t mask = __riscv_vsra_vx_i8m1(_a, 7, 8);
  vint8m1_t a_xor = __riscv_vxor_vv_i8m1(_a, mask, 8);
  return vreinterpretq_i8_m128i(__riscv_vsub_vv_i8m1(a_xor, mask, 8));
}

FORCE_INLINE __m128i _mm_add_epi16(__m128i a, __m128i b) {
  vint16m1_t _a = vreinterpretq_m128i_i16(a);
  vint16m1_t _b = vreinterpretq_m128i_i16(b);
  return vreinterpretq_i16_m128i(__riscv_vadd_vv_i16m1(_a, _b, 8));
}

FORCE_INLINE __m128i _mm_add_epi32(__m128i a, __m128i b) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _b = vreinterpretq_m128i_i32(b);
  return __riscv_vadd_vv_i32m1(_a, _b, 4);
}

FORCE_INLINE __m128i _mm_add_epi64(__m128i a, __m128i b) {
  vint64m1_t _a = vreinterpretq_m128i_i64(a);
  vint64m1_t _b = vreinterpretq_m128i_i64(b);
  return vreinterpretq_i64_m128i(__riscv_vadd_vv_i64m1(_a, _b, 2));
}

FORCE_INLINE __m128i _mm_add_epi8(__m128i a, __m128i b) {
  vint8m1_t _a = vreinterpretq_m128i_i8(a);
  vint8m1_t _b = vreinterpretq_m128i_i8(b);
  return vreinterpretq_i8_m128i(__riscv_vadd_vv_i8m1(_a, _b, 16));
}

FORCE_INLINE __m128d _mm_add_pd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  return vreinterpretq_f64_m128d(__riscv_vfadd_vv_f64m1(_a, _b, 2));
}

FORCE_INLINE __m128 _mm_add_ps(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  return vreinterpretq_f32_m128(__riscv_vfadd_vv_f32m1(_a, _b, 4));
}

FORCE_INLINE __m128d _mm_add_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vfloat64m1_t add = __riscv_vfadd_vv_f64m1(_a, _b, 2);
  return vreinterpretq_f64_m128d(__riscv_vslideup_vx_f64m1(_a, add, 0, 1));
}

FORCE_INLINE __m64 _mm_add_si64(__m64 a, __m64 b) {
  vint64m1_t _a = vreinterpretq_m64_i64(a);
  vint64m1_t _b = vreinterpretq_m64_i64(b);
  return vreinterpretq_i64_m64(__riscv_vadd_vv_i64m1(_a, _b, 1));
}

FORCE_INLINE __m128 _mm_add_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vfloat32m1_t add = __riscv_vfadd_vv_f32m1(_a, _b, 4);
  return vreinterpretq_f32_m128(__riscv_vslideup_vx_f32m1(_a, add, 0, 1));
}

FORCE_INLINE __m128i _mm_adds_epi16(__m128i a, __m128i b) {
  vint16m1_t _a = vreinterpretq_m128i_i16(a);
  vint16m1_t _b = vreinterpretq_m128i_i16(b);
  return vreinterpretq_i16_m128i(__riscv_vsadd_vv_i16m1(_a, _b, 8));
}

FORCE_INLINE __m128i _mm_adds_epi8(__m128i a, __m128i b) {
  vint8m1_t _a = vreinterpretq_m128i_i8(a);
  vint8m1_t _b = vreinterpretq_m128i_i8(b);
  return vreinterpretq_i8_m128i(__riscv_vsadd_vv_i8m1(_a, _b, 16));
}

FORCE_INLINE __m128i _mm_adds_epu16(__m128i a, __m128i b) {
  vuint16m1_t _a = vreinterpretq_m128i_u16(a);
  vuint16m1_t _b = vreinterpretq_m128i_u16(b);
  return vreinterpretq_u16_m128i(__riscv_vsaddu_vv_u16m1(_a, _b, 8));
}

FORCE_INLINE __m128i _mm_adds_epu8(__m128i a, __m128i b) {
  vuint8m1_t _a = vreinterpretq_m128i_u8(a);
  vuint8m1_t _b = vreinterpretq_m128i_u8(b);
  return vreinterpretq_u8_m128i(__riscv_vsaddu_vv_u8m1(_a, _b, 16));
}

FORCE_INLINE __m128d _mm_addsub_pd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vfloat64m1_t add = __riscv_vfadd_vv_f64m1(_a, _b, 2);
  vfloat64m1_t sub = __riscv_vfsub_vv_f64m1(_a, _b, 2);
  return vreinterpretq_f64_m128d(__riscv_vslideup_vx_f64m1(add, sub, 0, 1));
}

FORCE_INLINE __m128 _mm_addsub_ps(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vfloat32m1_t add = __riscv_vfadd_vv_f32m1(_a, _b, 4);
  vfloat32m1_t sub = __riscv_vfsub_vv_f32m1(_a, _b, 4);
  vbool32_t mask =
      __riscv_vreinterpret_v_i32m1_b32(__riscv_vmv_s_x_i32m1(0xa, 4));
  return vreinterpretq_f32_m128(__riscv_vmerge_vvm_f32m1(sub, add, mask, 4));
}

// FORCE_INLINE __m128i _mm_alignr_epi8 (__m128i a, __m128i b, int imm8) {}

// FORCE_INLINE __m64 _mm_alignr_pi8 (__m64 a, __m64 b, int imm8) {}

FORCE_INLINE __m128d _mm_and_pd(__m128d a, __m128d b) {
  vint64m1_t _a = vreinterpretq_m128d_i64(a);
  vint64m1_t _b = vreinterpretq_m128d_i64(b);
  return vreinterpretq_i64_m128d(__riscv_vand_vv_i64m1(_a, _b, 2));
}

FORCE_INLINE __m128 _mm_and_ps(__m128 a, __m128 b) {
  vint32m1_t _a = vreinterpretq_m128_i32(a);
  vint32m1_t _b = vreinterpretq_m128_i32(b);
  return vreinterpretq_i32_m128(__riscv_vand_vv_i32m1(_a, _b, 4));
}

FORCE_INLINE __m128i _mm_and_si128(__m128i a, __m128i b) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _b = vreinterpretq_m128i_i32(b);
  return vreinterpretq_i32_m128i(__riscv_vand_vv_i32m1(_a, _b, 4));
}

FORCE_INLINE __m128d _mm_andnot_pd(__m128d a, __m128d b) {
  vint64m1_t _a = vreinterpretq_m128d_i64(a);
  vint64m1_t _b = vreinterpretq_m128d_i64(b);
  return vreinterpretq_i64_m128d(
      __riscv_vand_vv_i64m1(__riscv_vnot_v_i64m1(_a, 2), _b, 2));
}

FORCE_INLINE __m128 _mm_andnot_ps(__m128 a, __m128 b) {
  vint32m1_t _a = vreinterpretq_m128_i32(a);
  vint32m1_t _b = vreinterpretq_m128_i32(b);
  return vreinterpretq_i32_m128(
      __riscv_vand_vv_i32m1(__riscv_vnot_v_i32m1(_a, 4), _b, 4));
}

FORCE_INLINE __m128i _mm_andnot_si128(__m128i a, __m128i b) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _b = vreinterpretq_m128i_i32(b);
  return vreinterpretq_i32_m128i(
      __riscv_vand_vv_i32m1(__riscv_vnot_v_i32m1(_a, 4), _b, 4));
}

FORCE_INLINE __m128i _mm_avg_epu16(__m128i a, __m128i b) {
  vuint16m1_t _a = vreinterpretq_m128i_u16(a);
  vuint16m1_t _b = vreinterpretq_m128i_u16(b);
  return vreinterpretq_u16_m128i(
      __riscv_vaaddu_vv_u16m1(_a, _b, __RISCV_VXRM_RNU, 8));
}

FORCE_INLINE __m128i _mm_avg_epu8(__m128i a, __m128i b) {
  vuint8m1_t _a = vreinterpretq_m128i_u8(a);
  vuint8m1_t _b = vreinterpretq_m128i_u8(b);
  return vreinterpretq_u8_m128i(
      __riscv_vaaddu_vv_u8m1(_a, _b, __RISCV_VXRM_RNU, 16));
}

FORCE_INLINE __m64 _mm_avg_pu16(__m64 a, __m64 b) {
  // FIXME vreinterpretq_m64_u16 would trigger memory error
  vint16m1_t __a = __riscv_vreinterpret_v_i32m1_i16m1(a);
  vuint16m1_t _a = __riscv_vreinterpret_v_i16m1_u16m1(__a);
  vint16m1_t __b = __riscv_vreinterpret_v_i32m1_i16m1(b);
  vuint16m1_t _b = __riscv_vreinterpret_v_i16m1_u16m1(__b);
  return vreinterpretq_u16_m64(
      __riscv_vaaddu_vv_u16m1(_a, _b, __RISCV_VXRM_RNU, 4));
}

FORCE_INLINE __m64 _mm_avg_pu8(__m64 a, __m64 b) {
  vuint8m1_t _a = vreinterpretq_m64_u8(a);
  vuint8m1_t _b = vreinterpretq_m64_u8(b);
  return vreinterpretq_u8_m64(
      __riscv_vaaddu_vv_u8m1(_a, _b, __RISCV_VXRM_RNU, 8));
}

FORCE_INLINE __m128i _mm_blend_epi16(__m128i a, __m128i b, const int imm8) {
  vint16m1_t _a = vreinterpretq_m128i_i16(a);
  vint16m1_t _b = vreinterpretq_m128i_i16(b);
  vbool16_t _imm8 =
      __riscv_vreinterpret_v_i8m1_b16(__riscv_vmv_s_x_i8m1(imm8, 8));
  return vreinterpretq_i16_m128i(__riscv_vmerge_vvm_i16m1(_a, _b, _imm8, 8));
}

FORCE_INLINE __m128d _mm_blend_pd(__m128d a, __m128d b, const int imm8) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t _imm8 =
      __riscv_vreinterpret_v_i8m1_b64(__riscv_vmv_s_x_i8m1(imm8, 2));
  return vreinterpretq_f64_m128d(__riscv_vmerge_vvm_f64m1(_a, _b, _imm8, 2));
}

FORCE_INLINE __m128 _mm_blend_ps(__m128 a, __m128 b, const int imm8) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t _imm8 =
      __riscv_vreinterpret_v_i8m1_b32(__riscv_vmv_s_x_i8m1(imm8, 4));
  return vreinterpretq_f32_m128(__riscv_vmerge_vvm_f32m1(_a, _b, _imm8, 4));
}

FORCE_INLINE __m128i _mm_blendv_epi8(__m128i a, __m128i b, __m128i mask) {
  vint8m1_t _a = vreinterpretq_m128i_i8(a);
  vint8m1_t _b = vreinterpretq_m128i_i8(b);
  vint8m1_t _mask = vreinterpretq_m128i_i8(mask);
  vint8m1_t mask_sra = __riscv_vsra_vx_i8m1(_mask, 7, 16);
  vbool8_t mask_b8 = __riscv_vmsne_vx_i8m1_b8(mask_sra, 0, 16);
  return vreinterpretq_i8_m128i(__riscv_vmerge_vvm_i8m1(_a, _b, mask_b8, 16));
}

FORCE_INLINE __m128d _mm_blendv_pd(__m128d a, __m128d b, __m128d mask) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vint64m1_t _mask = vreinterpretq_m128d_i64(mask);
  vint64m1_t mask_sra = __riscv_vsra_vx_i64m1(_mask, 63, 2);
  vbool64_t mask_b64 = __riscv_vmsne_vx_i64m1_b64(mask_sra, 0, 2);
  return vreinterpretq_f64_m128d(__riscv_vmerge_vvm_f64m1(_a, _b, mask_b64, 2));
}

FORCE_INLINE __m128 _mm_blendv_ps(__m128 a, __m128 b, __m128 mask) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vint32m1_t _mask = vreinterpretq_m128_i32(mask);
  vint32m1_t mask_sra = __riscv_vsra_vx_i32m1(_mask, 31, 4);
  vbool32_t mask_b32 = __riscv_vmsne_vx_i32m1_b32(mask_sra, 0, 4);
  return vreinterpretq_f32_m128(__riscv_vmerge_vvm_f32m1(_a, _b, mask_b32, 4));
}

// FORCE_INLINE __m128i _mm_bslli_si128 (__m128i a, int imm8) {}

// FORCE_INLINE __m128i _mm_bsrli_si128 (__m128i a, int imm8) {}

FORCE_INLINE __m128 _mm_castpd_ps(__m128d a) {
  return __riscv_vreinterpret_v_u32m1_f32m1(__riscv_vreinterpret_v_u64m1_u32m1(
      __riscv_vreinterpret_v_f64m1_u64m1(a)));
}

FORCE_INLINE __m128i _mm_castpd_si128(__m128d a) {
  return __riscv_vreinterpret_v_i64m1_i32m1(
      __riscv_vreinterpret_v_f64m1_i64m1(a));
}

FORCE_INLINE __m128d _mm_castps_pd(__m128 a) {
  return __riscv_vreinterpret_v_i64m1_f64m1(__riscv_vreinterpret_v_i32m1_i64m1(
      __riscv_vreinterpret_v_f32m1_i32m1(a)));
}

FORCE_INLINE __m128i _mm_castps_si128(__m128 a) {
  return __riscv_vreinterpret_v_f32m1_i32m1(a);
}

FORCE_INLINE __m128d _mm_castsi128_pd(__m128i a) {
  return __riscv_vreinterpret_v_i64m1_f64m1(
      __riscv_vreinterpret_v_i32m1_i64m1(a));
}

FORCE_INLINE __m128 _mm_castsi128_ps(__m128i a) {
  return __riscv_vreinterpret_v_i32m1_f32m1(a);
}

// FORCE_INLINE __m128d _mm_ceil_pd (__m128d a) {}

// FORCE_INLINE __m128 _mm_ceil_ps (__m128 a) {}

// FORCE_INLINE __m128d _mm_ceil_sd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_ceil_ss (__m128 a, __m128 b) {}

// FORCE_INLINE void _mm_clflush (void const* p) {}

FORCE_INLINE __m128i _mm_cmpeq_epi16(__m128i a, __m128i b) {
  vint16m1_t _a = vreinterpretq_m128i_i16(a);
  vint16m1_t _b = vreinterpretq_m128i_i16(b);
  vbool16_t cmp_res = __riscv_vmseq_vv_i16m1_b16(_a, _b, 8);
  return vreinterpretq_i16_m128i(__riscv_vmerge_vvm_i16m1(
      __riscv_vmv_v_x_i16m1(0x0, 8), __riscv_vmv_v_x_i16m1(UINT16_MAX, 8),
      cmp_res, 8));
}

FORCE_INLINE __m128i _mm_cmpeq_epi32(__m128i a, __m128i b) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _b = vreinterpretq_m128i_i32(b);
  vbool32_t cmp_res = __riscv_vmseq_vv_i32m1_b32(_a, _b, 4);
  return vreinterpretq_i32_m128i(__riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4));
}

FORCE_INLINE __m128i _mm_cmpeq_epi64(__m128i a, __m128i b) {
  vint64m1_t _a = vreinterpretq_m128i_i64(a);
  vint64m1_t _b = vreinterpretq_m128i_i64(b);
  vbool64_t cmp_res = __riscv_vmseq_vv_i64m1_b64(_a, _b, 2);
  return vreinterpretq_i64_m128i(__riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2));
}

FORCE_INLINE __m128i _mm_cmpeq_epi8(__m128i a, __m128i b) {
  vint8m1_t _a = vreinterpretq_m128i_i8(a);
  vint8m1_t _b = vreinterpretq_m128i_i8(b);
  vbool8_t cmp_res = __riscv_vmseq_vv_i8m1_b8(_a, _b, 16);
  return vreinterpretq_i8_m128i(__riscv_vmerge_vvm_i8m1(
      __riscv_vmv_v_x_i8m1(0x0, 16), __riscv_vmv_v_x_i8m1(UINT8_MAX, 16),
      cmp_res, 16));
}

FORCE_INLINE __m128d _mm_cmpeq_pd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t cmp_res = __riscv_vmfeq_vv_f64m1_b64(_a, _b, 2);
  return vreinterpretq_i64_m128d(__riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2));
}

FORCE_INLINE __m128 _mm_cmpeq_ps(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t cmp_res = __riscv_vmfeq_vv_f32m1_b32(_a, _b, 4);
  return vreinterpretq_i32_m128(__riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4));
}

FORCE_INLINE __m128d _mm_cmpeq_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t cmp_res = __riscv_vmfeq_vv_f64m1_b64(_a, _b, 2);
  vint64m1_t cmp_res_i64 = __riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2);
  return vreinterpretq_i64_m128d(__riscv_vslideup_vx_i64m1(
      __riscv_vreinterpret_v_f64m1_i64m1(_a), cmp_res_i64, 0, 1));
}

FORCE_INLINE __m128 _mm_cmpeq_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t cmp_res = __riscv_vmfeq_vv_f32m1_b32(_a, _b, 4);
  vint32m1_t cmp_res_i32 = __riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4);
  return vreinterpretq_i32_m128(__riscv_vslideup_vx_i32m1(
      __riscv_vreinterpret_v_f32m1_i32m1(_a), cmp_res_i32, 0, 1));
}

// FORCE_INLINE int _mm_cmpestra (__m128i a, int la, __m128i b, int lb, const
// int imm8) {}

// FORCE_INLINE int _mm_cmpestrc (__m128i a, int la, __m128i b, int lb, const
// int imm8) {}

// FORCE_INLINE int _mm_cmpestri (__m128i a, int la, __m128i b, int lb, const
// int imm8) {}

// FORCE_INLINE __m128i _mm_cmpestrm (__m128i a, int la, __m128i b, int lb,
// const int imm8) {}

// FORCE_INLINE int _mm_cmpestro (__m128i a, int la, __m128i b, int lb, const
// int imm8) {}

// FORCE_INLINE int _mm_cmpestrs (__m128i a, int la, __m128i b, int lb, const
// int imm8) {}

// FORCE_INLINE int _mm_cmpestrz (__m128i a, int la, __m128i b, int lb, const
// int imm8) {}

FORCE_INLINE __m128d _mm_cmpge_pd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t cmp_res = __riscv_vmfge_vv_f64m1_b64(_a, _b, 2);
  return vreinterpretq_i64_m128d(__riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2));
}

FORCE_INLINE __m128 _mm_cmpge_ps(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t cmp_res = __riscv_vmfge_vv_f32m1_b32(_a, _b, 4);
  return vreinterpretq_i32_m128(__riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4));
}

FORCE_INLINE __m128d _mm_cmpge_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t cmp_res = __riscv_vmfge_vv_f64m1_b64(_a, _b, 2);
  vint64m1_t cmp_res_i64 = __riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2);
  return vreinterpretq_i64_m128d(__riscv_vslideup_vx_i64m1(
      __riscv_vreinterpret_v_f64m1_i64m1(_a), cmp_res_i64, 0, 1));
}

FORCE_INLINE __m128 _mm_cmpge_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t cmp_res = __riscv_vmfge_vv_f32m1_b32(_a, _b, 4);
  vint32m1_t cmp_res_i32 = __riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4);
  return vreinterpretq_i32_m128(__riscv_vslideup_vx_i32m1(
      __riscv_vreinterpret_v_f32m1_i32m1(_a), cmp_res_i32, 0, 1));
}

FORCE_INLINE __m128i _mm_cmpgt_epi16(__m128i a, __m128i b) {
  vint16m1_t _a = vreinterpretq_m128i_i16(a);
  vint16m1_t _b = vreinterpretq_m128i_i16(b);
  vbool16_t cmp_res = __riscv_vmsgt_vv_i16m1_b16(_a, _b, 8);
  return vreinterpretq_i16_m128i(__riscv_vmerge_vvm_i16m1(
      __riscv_vmv_v_x_i16m1(0x0, 8), __riscv_vmv_v_x_i16m1(UINT16_MAX, 8),
      cmp_res, 8));
}

FORCE_INLINE __m128i _mm_cmpgt_epi32(__m128i a, __m128i b) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _b = vreinterpretq_m128i_i32(b);
  vbool32_t cmp_res = __riscv_vmsgt_vv_i32m1_b32(_a, _b, 4);
  return vreinterpretq_i32_m128i(__riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4));
}

FORCE_INLINE __m128i _mm_cmpgt_epi64(__m128i a, __m128i b) {
  vint64m1_t _a = vreinterpretq_m128i_i64(a);
  vint64m1_t _b = vreinterpretq_m128i_i64(b);
  vbool64_t cmp_res = __riscv_vmsgt_vv_i64m1_b64(_a, _b, 2);
  return vreinterpretq_i64_m128i(__riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2));
}

FORCE_INLINE __m128i _mm_cmpgt_epi8(__m128i a, __m128i b) {
  vint8m1_t _a = vreinterpretq_m128i_i8(a);
  vint8m1_t _b = vreinterpretq_m128i_i8(b);
  vbool8_t cmp_res = __riscv_vmsgt_vv_i8m1_b8(_a, _b, 16);
  return vreinterpretq_i8_m128i(__riscv_vmerge_vvm_i8m1(
      __riscv_vmv_v_x_i8m1(0x0, 16), __riscv_vmv_v_x_i8m1(UINT8_MAX, 16),
      cmp_res, 16));
}

FORCE_INLINE __m128d _mm_cmpgt_pd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t cmp_res = __riscv_vmfgt_vv_f64m1_b64(_a, _b, 2);
  return vreinterpretq_i64_m128d(__riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2));
}

FORCE_INLINE __m128 _mm_cmpgt_ps(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t cmp_res = __riscv_vmfgt_vv_f32m1_b32(_a, _b, 4);
  return vreinterpretq_i32_m128(__riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4));
}

FORCE_INLINE __m128d _mm_cmpgt_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t cmp_res = __riscv_vmfgt_vv_f64m1_b64(_a, _b, 2);
  vint64m1_t cmp_res_i64 = __riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2);
  return vreinterpretq_i64_m128d(__riscv_vslideup_vx_i64m1(
      __riscv_vreinterpret_v_f64m1_i64m1(_a), cmp_res_i64, 0, 1));
}

FORCE_INLINE __m128 _mm_cmpgt_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t cmp_res = __riscv_vmfgt_vv_f32m1_b32(_a, _b, 4);
  vint32m1_t cmp_res_i32 = __riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4);
  return vreinterpretq_i32_m128(__riscv_vslideup_vx_i32m1(
      __riscv_vreinterpret_v_f32m1_i32m1(_a), cmp_res_i32, 0, 1));
}

// FORCE_INLINE int _mm_cmpistra (__m128i a, __m128i b, const int imm8) {}

// FORCE_INLINE int _mm_cmpistrc (__m128i a, __m128i b, const int imm8) {}

// FORCE_INLINE int _mm_cmpistri (__m128i a, __m128i b, const int imm8) {}

// FORCE_INLINE __m128i _mm_cmpistrm (__m128i a, __m128i b, const int imm8) {}

// FORCE_INLINE int _mm_cmpistro (__m128i a, __m128i b, const int imm8) {}

// FORCE_INLINE int _mm_cmpistrs (__m128i a, __m128i b, const int imm8) {}

// FORCE_INLINE int _mm_cmpistrz (__m128i a, __m128i b, const int imm8) {}

FORCE_INLINE __m128d _mm_cmple_pd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t cmp_res = __riscv_vmfle_vv_f64m1_b64(_a, _b, 2);
  return vreinterpretq_i64_m128d(__riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2));
}

FORCE_INLINE __m128 _mm_cmple_ps(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t cmp_res = __riscv_vmfle_vv_f32m1_b32(_a, _b, 4);
  return vreinterpretq_i32_m128(__riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4));
}

FORCE_INLINE __m128d _mm_cmple_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t cmp_res = __riscv_vmfle_vv_f64m1_b64(_a, _b, 2);
  vint64m1_t cmp_res_i64 = __riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2);
  return vreinterpretq_i64_m128d(__riscv_vslideup_vx_i64m1(
      __riscv_vreinterpret_v_f64m1_i64m1(_a), cmp_res_i64, 0, 1));
}

FORCE_INLINE __m128 _mm_cmple_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t cmp_res = __riscv_vmfle_vv_f32m1_b32(_a, _b, 4);
  vint32m1_t cmp_res_i32 = __riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4);
  return vreinterpretq_i32_m128(__riscv_vslideup_vx_i32m1(
      __riscv_vreinterpret_v_f32m1_i32m1(_a), cmp_res_i32, 0, 1));
}

FORCE_INLINE __m128i _mm_cmplt_epi16(__m128i a, __m128i b) {
  vint16m1_t _a = vreinterpretq_m128i_i16(a);
  vint16m1_t _b = vreinterpretq_m128i_i16(b);
  vbool16_t cmp_res = __riscv_vmslt_vv_i16m1_b16(_a, _b, 8);
  return vreinterpretq_i16_m128i(__riscv_vmerge_vvm_i16m1(
      __riscv_vmv_v_x_i16m1(0x0, 8), __riscv_vmv_v_x_i16m1(UINT16_MAX, 8),
      cmp_res, 8));
}

FORCE_INLINE __m128i _mm_cmplt_epi32(__m128i a, __m128i b) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _b = vreinterpretq_m128i_i32(b);
  vbool32_t cmp_res = __riscv_vmslt_vv_i32m1_b32(_a, _b, 4);
  return vreinterpretq_i32_m128i(__riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4));
}

FORCE_INLINE __m128i _mm_cmplt_epi8(__m128i a, __m128i b) {
  vint8m1_t _a = vreinterpretq_m128i_i8(a);
  vint8m1_t _b = vreinterpretq_m128i_i8(b);
  vbool8_t cmp_res = __riscv_vmslt_vv_i8m1_b8(_a, _b, 16);
  return vreinterpretq_i8_m128i(__riscv_vmerge_vvm_i8m1(
      __riscv_vmv_v_x_i8m1(0x0, 16), __riscv_vmv_v_x_i8m1(UINT8_MAX, 16),
      cmp_res, 16));
}

FORCE_INLINE __m128d _mm_cmplt_pd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t cmp_res = __riscv_vmflt_vv_f64m1_b64(_a, _b, 2);
  return vreinterpretq_i64_m128d(__riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2));
}

FORCE_INLINE __m128 _mm_cmplt_ps(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t cmp_res = __riscv_vmflt_vv_f32m1_b32(_a, _b, 4);
  return vreinterpretq_i32_m128(__riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4));
}

FORCE_INLINE __m128d _mm_cmplt_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t cmp_res = __riscv_vmflt_vv_f64m1_b64(_a, _b, 2);
  vint64m1_t cmp_res_i64 = __riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2);
  return vreinterpretq_i64_m128d(__riscv_vslideup_vx_i64m1(
      __riscv_vreinterpret_v_f64m1_i64m1(_a), cmp_res_i64, 0, 1));
}

FORCE_INLINE __m128 _mm_cmplt_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t cmp_res = __riscv_vmflt_vv_f32m1_b32(_a, _b, 4);
  vint32m1_t cmp_res_i32 = __riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4);
  return vreinterpretq_i32_m128(__riscv_vslideup_vx_i32m1(
      __riscv_vreinterpret_v_f32m1_i32m1(_a), cmp_res_i32, 0, 1));
}

FORCE_INLINE __m128d _mm_cmpneq_pd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t cmp_res = __riscv_vmfne_vv_f64m1_b64(_a, _b, 2);
  return vreinterpretq_i64_m128d(__riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2));
}

FORCE_INLINE __m128 _mm_cmpneq_ps(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t cmp_res = __riscv_vmfne_vv_f32m1_b32(_a, _b, 4);
  return vreinterpretq_i32_m128(__riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4));
}

FORCE_INLINE __m128d _mm_cmpneq_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t cmp_res = __riscv_vmfne_vv_f64m1_b64(_a, _b, 2);
  vint64m1_t cmp_res_i64 = __riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      cmp_res, 2);
  return vreinterpretq_i64_m128d(__riscv_vslideup_vx_i64m1(
      __riscv_vreinterpret_v_f64m1_i64m1(_a), cmp_res_i64, 0, 1));
}

FORCE_INLINE __m128 _mm_cmpneq_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t cmp_res = __riscv_vmfne_vv_f32m1_b32(_a, _b, 4);
  vint32m1_t cmp_res_i32 = __riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      cmp_res, 4);
  return vreinterpretq_i32_m128(__riscv_vslideup_vx_i32m1(
      __riscv_vreinterpret_v_f32m1_i32m1(_a), cmp_res_i32, 0, 1));
}

// FORCE_INLINE __m128d _mm_cmpnge_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_cmpnge_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128d _mm_cmpnge_sd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_cmpnge_ss (__m128 a, __m128 b) {}

// FORCE_INLINE __m128d _mm_cmpngt_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_cmpngt_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128d _mm_cmpngt_sd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_cmpngt_ss (__m128 a, __m128 b) {}

// FORCE_INLINE __m128d _mm_cmpnle_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_cmpnle_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128d _mm_cmpnle_sd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_cmpnle_ss (__m128 a, __m128 b) {}

// FORCE_INLINE __m128d _mm_cmpnlt_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_cmpnlt_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128d _mm_cmpnlt_sd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_cmpnlt_ss (__m128 a, __m128 b) {}

FORCE_INLINE __m128d _mm_cmpord_pd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t not_nan_a = __riscv_vmfeq_vv_f64m1_b64(_a, _a, 2);
  vbool64_t not_nan_b = __riscv_vmfeq_vv_f64m1_b64(_b, _b, 2);
  vbool64_t non_nan = __riscv_vmand_mm_b64(not_nan_a, not_nan_b, 2);
  return vreinterpretq_i64_m128d(__riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      non_nan, 2));
}

FORCE_INLINE __m128 _mm_cmpord_ps(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t not_nan_a = __riscv_vmfeq_vv_f32m1_b32(_a, _a, 4);
  vbool32_t not_nan_b = __riscv_vmfeq_vv_f32m1_b32(_b, _b, 4);
  vbool32_t non_nan = __riscv_vmand_mm_b32(not_nan_a, not_nan_b, 4);
  return vreinterpretq_i32_m128(__riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      non_nan, 4));
}

FORCE_INLINE __m128d _mm_cmpord_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t not_nan_a = __riscv_vmfeq_vv_f64m1_b64(_a, _a, 2);
  vbool64_t not_nan_b = __riscv_vmfeq_vv_f64m1_b64(_b, _b, 2);
  vbool64_t non_nan = __riscv_vmand_mm_b64(not_nan_a, not_nan_b, 2);
  vint64m1_t cmp_res_i64 = __riscv_vmerge_vvm_i64m1(
      __riscv_vmv_v_x_i64m1(0x0, 2), __riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
      non_nan, 2);
  return vreinterpretq_i64_m128d(__riscv_vslideup_vx_i64m1(
      __riscv_vreinterpret_v_f64m1_i64m1(_a), cmp_res_i64, 0, 1));
}

FORCE_INLINE __m128 _mm_cmpord_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t not_nan_a = __riscv_vmfeq_vv_f32m1_b32(_a, _a, 4);
  vbool32_t not_nan_b = __riscv_vmfeq_vv_f32m1_b32(_b, _b, 4);
  vbool32_t non_nan = __riscv_vmand_mm_b32(not_nan_a, not_nan_b, 4);
  vint32m1_t cmp_res_i32 = __riscv_vmerge_vvm_i32m1(
      __riscv_vmv_v_x_i32m1(0x0, 4), __riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
      non_nan, 4);
  return vreinterpretq_i32_m128(__riscv_vslideup_vx_i32m1(
      __riscv_vreinterpret_v_f32m1_i32m1(_a), cmp_res_i32, 0, 1));
}

FORCE_INLINE __m128d _mm_cmpunord_pd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t not_nan_a = __riscv_vmfeq_vv_f64m1_b64(_a, _a, 2);
  vbool64_t not_nan_b = __riscv_vmfeq_vv_f64m1_b64(_b, _b, 2);
  vbool64_t non_nan = __riscv_vmand_mm_b64(not_nan_a, not_nan_b, 2);
  return vreinterpretq_i64_m128d(
      __riscv_vmerge_vvm_i64m1(__riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
                               __riscv_vmv_v_x_i64m1(0x0, 2), non_nan, 2));
}

FORCE_INLINE __m128 _mm_cmpunord_ps(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t not_nan_a = __riscv_vmfeq_vv_f32m1_b32(_a, _a, 4);
  vbool32_t not_nan_b = __riscv_vmfeq_vv_f32m1_b32(_b, _b, 4);
  vbool32_t non_nan = __riscv_vmand_mm_b32(not_nan_a, not_nan_b, 4);
  return vreinterpretq_i32_m128(
      __riscv_vmerge_vvm_i32m1(__riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
                               __riscv_vmv_v_x_i32m1(0x0, 4), non_nan, 4));
}

FORCE_INLINE __m128d _mm_cmpunord_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vbool64_t not_nan_a = __riscv_vmfeq_vv_f64m1_b64(_a, _a, 2);
  vbool64_t not_nan_b = __riscv_vmfeq_vv_f64m1_b64(_b, _b, 2);
  vbool64_t non_nan = __riscv_vmand_mm_b64(not_nan_a, not_nan_b, 2);
  vint64m1_t cmp_res_i64 =
      __riscv_vmerge_vvm_i64m1(__riscv_vmv_v_x_i64m1(UINT64_MAX, 2),
                               __riscv_vmv_v_x_i64m1(0x0, 2), non_nan, 2);
  return vreinterpretq_i64_m128d(__riscv_vslideup_vx_i64m1(
      __riscv_vreinterpret_v_f64m1_i64m1(_a), cmp_res_i64, 0, 1));
}

FORCE_INLINE __m128 _mm_cmpunord_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vbool32_t not_nan_a = __riscv_vmfeq_vv_f32m1_b32(_a, _a, 4);
  vbool32_t not_nan_b = __riscv_vmfeq_vv_f32m1_b32(_b, _b, 4);
  vbool32_t non_nan = __riscv_vmand_mm_b32(not_nan_a, not_nan_b, 4);
  vint32m1_t cmp_res_i32 =
      __riscv_vmerge_vvm_i32m1(__riscv_vmv_v_x_i32m1(UINT32_MAX, 4),
                               __riscv_vmv_v_x_i32m1(0x0, 4), non_nan, 4);
  return vreinterpretq_i32_m128(__riscv_vslideup_vx_i32m1(
      __riscv_vreinterpret_v_f32m1_i32m1(_a), cmp_res_i32, 0, 1));
}

FORCE_INLINE int _mm_comieq_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  return __riscv_vmv_x_s_i64m1_i64(
             __riscv_vreinterpret_v_f64m1_i64m1(_mm_cmpeq_sd(_a, _b))) &
         0x1;
}

FORCE_INLINE int _mm_comieq_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  return __riscv_vmv_x_s_i32m1_i32(
             __riscv_vreinterpret_v_f32m1_i32m1(_mm_cmpeq_ss(_a, _b))) &
         0x1;
}

FORCE_INLINE int _mm_comige_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  return __riscv_vmv_x_s_i64m1_i64(
             __riscv_vreinterpret_v_f64m1_i64m1(_mm_cmpge_sd(_a, _b))) &
         0x1;
}

FORCE_INLINE int _mm_comige_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  return __riscv_vmv_x_s_i32m1_i32(
             __riscv_vreinterpret_v_f32m1_i32m1(_mm_cmpge_ss(_a, _b))) &
         0x1;
}

FORCE_INLINE int _mm_comigt_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  return __riscv_vmv_x_s_i64m1_i64(
             __riscv_vreinterpret_v_f64m1_i64m1(_mm_cmpgt_sd(_a, _b))) &
         0x1;
}

FORCE_INLINE int _mm_comigt_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  return __riscv_vmv_x_s_i32m1_i32(
             __riscv_vreinterpret_v_f32m1_i32m1(_mm_cmpgt_ss(_a, _b))) &
         0x1;
}

FORCE_INLINE int _mm_comile_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  return __riscv_vmv_x_s_i64m1_i64(
             __riscv_vreinterpret_v_f64m1_i64m1(_mm_cmple_sd(_a, _b))) &
         0x1;
}

FORCE_INLINE int _mm_comile_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  return __riscv_vmv_x_s_i32m1_i32(
             __riscv_vreinterpret_v_f32m1_i32m1(_mm_cmple_ss(_a, _b))) &
         0x1;
}

FORCE_INLINE int _mm_comilt_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  return __riscv_vmv_x_s_i64m1_i64(
             __riscv_vreinterpret_v_f64m1_i64m1(_mm_cmplt_sd(_a, _b))) &
         0x1;
}

FORCE_INLINE int _mm_comilt_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  return __riscv_vmv_x_s_i32m1_i32(
             __riscv_vreinterpret_v_f32m1_i32m1(_mm_cmplt_ss(_a, _b))) &
         0x1;
}

FORCE_INLINE int _mm_comineq_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  return __riscv_vmv_x_s_i64m1_i64(
             __riscv_vreinterpret_v_f64m1_i64m1(_mm_cmpneq_sd(_a, _b))) &
         0x1;
}

FORCE_INLINE int _mm_comineq_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  return __riscv_vmv_x_s_i32m1_i32(
             __riscv_vreinterpret_v_f32m1_i32m1(_mm_cmpneq_ss(_a, _b))) &
         0x1;
}

// FORCE_INLINE unsigned int _mm_crc32_u16 (unsigned int crc, unsigned short v)
// {}

// FORCE_INLINE unsigned int _mm_crc32_u32 (unsigned int crc, unsigned int v) {}

// FORCE_INLINE unsigned __int64 _mm_crc32_u64 (unsigned __int64 crc, unsigned
// __int64 v) {}

// FORCE_INLINE unsigned int _mm_crc32_u8 (unsigned int crc, unsigned char v) {}

// FORCE_INLINE __m128 _mm_cvt_pi2ps (__m128 a, __m64 b) {}

// FORCE_INLINE __m64 _mm_cvt_ps2pi (__m128 a) {}

// FORCE_INLINE __m128 _mm_cvt_si2ss (__m128 a, int b) {}

// FORCE_INLINE int _mm_cvt_ss2si (__m128 a) {}

// FORCE_INLINE __m128i _mm_cvtepi16_epi32 (__m128i a) {}

// FORCE_INLINE __m128i _mm_cvtepi16_epi64 (__m128i a) {}

// FORCE_INLINE __m128i _mm_cvtepi32_epi64 (__m128i a) {}

// FORCE_INLINE __m128d _mm_cvtepi32_pd (__m128i a) {}

// FORCE_INLINE __m128 _mm_cvtepi32_ps (__m128i a) {}

// FORCE_INLINE __m128i _mm_cvtepi8_epi16 (__m128i a) {}

// FORCE_INLINE __m128i _mm_cvtepi8_epi32 (__m128i a) {}

// FORCE_INLINE __m128i _mm_cvtepi8_epi64 (__m128i a) {}

// FORCE_INLINE __m128i _mm_cvtepu16_epi32 (__m128i a) {}

// FORCE_INLINE __m128i _mm_cvtepu16_epi64 (__m128i a) {}

// FORCE_INLINE __m128i _mm_cvtepu32_epi64 (__m128i a) {}

// FORCE_INLINE __m128i _mm_cvtepu8_epi16 (__m128i a) {}

// FORCE_INLINE __m128i _mm_cvtepu8_epi32 (__m128i a) {}

// FORCE_INLINE __m128i _mm_cvtepu8_epi64 (__m128i a) {}

// FORCE_INLINE __m128i _mm_cvtpd_epi32 (__m128d a) {}

// FORCE_INLINE __m64 _mm_cvtpd_pi32 (__m128d a) {}

// FORCE_INLINE __m128 _mm_cvtpd_ps (__m128d a) {}

// FORCE_INLINE __m128 _mm_cvtpi16_ps (__m64 a) {}

// FORCE_INLINE __m128d _mm_cvtpi32_pd (__m64 a) {}

// FORCE_INLINE __m128 _mm_cvtpi32_ps (__m128 a, __m64 b) {}

// FORCE_INLINE __m128 _mm_cvtpi32x2_ps (__m64 a, __m64 b) {}

// FORCE_INLINE __m128 _mm_cvtpi8_ps (__m64 a) {}

// FORCE_INLINE __m128i _mm_cvtps_epi32 (__m128 a) {}

// FORCE_INLINE __m128d _mm_cvtps_pd (__m128 a) {}

// FORCE_INLINE __m64 _mm_cvtps_pi16 (__m128 a) {}

// FORCE_INLINE __m64 _mm_cvtps_pi32 (__m128 a) {}

// FORCE_INLINE __m64 _mm_cvtps_pi8 (__m128 a) {}

// FORCE_INLINE __m128 _mm_cvtpu16_ps (__m64 a) {}

// FORCE_INLINE __m128 _mm_cvtpu8_ps (__m64 a) {}

// FORCE_INLINE double _mm_cvtsd_f64 (__m128d a) {}

// FORCE_INLINE int _mm_cvtsd_si32 (__m128d a) {}

// FORCE_INLINE __int64 _mm_cvtsd_si64 (__m128d a) {}

// FORCE_INLINE __int64 _mm_cvtsd_si64x (__m128d a) {}

// FORCE_INLINE __m128 _mm_cvtsd_ss (__m128 a, __m128d b) {}

// FORCE_INLINE int _mm_cvtsi128_si32 (__m128i a) {}

// FORCE_INLINE __int64 _mm_cvtsi128_si64 (__m128i a) {}

// FORCE_INLINE __int64 _mm_cvtsi128_si64x (__m128i a) {}

// FORCE_INLINE __m128d _mm_cvtsi32_sd (__m128d a, int b) {}

// FORCE_INLINE __m128i _mm_cvtsi32_si128 (int a) {}

// FORCE_INLINE __m128 _mm_cvtsi32_ss (__m128 a, int b) {}

// FORCE_INLINE __m128d _mm_cvtsi64_sd (__m128d a, __int64 b) {}

// FORCE_INLINE __m128i _mm_cvtsi64_si128 (__int64 a) {}

// FORCE_INLINE __m128 _mm_cvtsi64_ss (__m128 a, __int64 b) {}

// FORCE_INLINE __m128d _mm_cvtsi64x_sd (__m128d a, __int64 b) {}

// FORCE_INLINE __m128i _mm_cvtsi64x_si128 (__int64 a) {}

// FORCE_INLINE float _mm_cvtss_f32 (__m128 a) {}

// FORCE_INLINE __m128d _mm_cvtss_sd (__m128d a, __m128 b) {}

// FORCE_INLINE int _mm_cvtss_si32 (__m128 a) {}

// FORCE_INLINE __int64 _mm_cvtss_si64 (__m128 a) {}

// FORCE_INLINE __m64 _mm_cvtt_ps2pi (__m128 a) {}

// FORCE_INLINE int _mm_cvtt_ss2si (__m128 a) {}

// FORCE_INLINE __m128i _mm_cvttpd_epi32 (__m128d a) {}

// FORCE_INLINE __m64 _mm_cvttpd_pi32 (__m128d a) {}

// FORCE_INLINE __m128i _mm_cvttps_epi32 (__m128 a) {}

// FORCE_INLINE __m64 _mm_cvttps_pi32 (__m128 a) {}

// FORCE_INLINE int _mm_cvttsd_si32 (__m128d a) {}

// FORCE_INLINE __int64 _mm_cvttsd_si64 (__m128d a) {}

// FORCE_INLINE __int64 _mm_cvttsd_si64x (__m128d a) {}

// FORCE_INLINE int _mm_cvttss_si32 (__m128 a) {}

// FORCE_INLINE __int64 _mm_cvttss_si64 (__m128 a) {}

// FORCE_INLINE __m128d _mm_div_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_div_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128d _mm_div_sd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_div_ss (__m128 a, __m128 b) {}

// FORCE_INLINE __m128d _mm_dp_pd (__m128d a, __m128d b, const int imm8) {}

// FORCE_INLINE __m128 _mm_dp_ps (__m128 a, __m128 b, const int imm8) {}

// FORCE_INLINE int _mm_extract_epi16 (__m128i a, int imm8) {}

// FORCE_INLINE int _mm_extract_epi32 (__m128i a, const int imm8) {}

// FORCE_INLINE __int64 _mm_extract_epi64 (__m128i a, const int imm8) {}

// FORCE_INLINE int _mm_extract_epi8 (__m128i a, const int imm8) {}

// FORCE_INLINE int _mm_extract_pi16 (__m64 a, int imm8) {}

// FORCE_INLINE int _mm_extract_ps (__m128 a, const int imm8) {}

// FORCE_INLINE __m128d _mm_floor_pd (__m128d a) {}

// FORCE_INLINE __m128 _mm_floor_ps (__m128 a) {}

// FORCE_INLINE __m128d _mm_floor_sd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_floor_ss (__m128 a, __m128 b) {}

// FORCE_INLINE void _mm_free (void * mem_addr) {}

// FORCE_INLINE unsigned int _MM_GET_EXCEPTION_MASK () {}

// FORCE_INLINE unsigned int _MM_GET_EXCEPTION_STATE () {}

// FORCE_INLINE unsigned int _MM_GET_FLUSH_ZERO_MODE () {}

// FORCE_INLINE unsigned int _MM_GET_ROUNDING_MODE () {}

// FORCE_INLINE unsigned int _mm_getcsr (void) {}

// FORCE_INLINE __m128i _mm_hadd_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_hadd_epi32 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128d _mm_hadd_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m64 _mm_hadd_pi16 (__m64 a, __m64 b) {}

// FORCE_INLINE __m64 _mm_hadd_pi32 (__m64 a, __m64 b) {}

// FORCE_INLINE __m128 _mm_hadd_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128i _mm_hadds_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m64 _mm_hadds_pi16 (__m64 a, __m64 b) {}

// FORCE_INLINE __m128i _mm_hsub_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_hsub_epi32 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128d _mm_hsub_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m64 _mm_hsub_pi16 (__m64 a, __m64 b) {}

// FORCE_INLINE __m64 _mm_hsub_pi32 (__m64 a, __m64 b) {}

// FORCE_INLINE __m128 _mm_hsub_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128i _mm_hsubs_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m64 _mm_hsubs_pi16 (__m64 a, __m64 b) {}

// FORCE_INLINE __m128i _mm_insert_epi16 (__m128i a, int i, int imm8) {}

// FORCE_INLINE __m128i _mm_insert_epi32 (__m128i a, int i, const int imm8) {}

// FORCE_INLINE __m128i _mm_insert_epi64 (__m128i a, __int64 i, const int imm8)
// {}

// FORCE_INLINE __m128i _mm_insert_epi8 (__m128i a, int i, const int imm8) {}

// FORCE_INLINE __m64 _mm_insert_pi16 (__m64 a, int i, int imm8) {}

// FORCE_INLINE __m128 _mm_insert_ps (__m128 a, __m128 b, const int imm8) {}

// FORCE_INLINE __m128i _mm_lddqu_si128 (__m128i const* mem_addr) {}

// FORCE_INLINE void _mm_lfence (void) {}

FORCE_INLINE __m128d _mm_load_pd(double const *mem_addr) {
  return vreinterpretq_f64_m128d(__riscv_vle64_v_f64m1(mem_addr, 2));
}

FORCE_INLINE __m128d _mm_load_pd1(double const *mem_addr) {
  double p[2] = {mem_addr[0], mem_addr[0]};
  return vreinterpretq_f64_m128d(__riscv_vle64_v_f64m1(p, 2));
}

FORCE_INLINE __m128 _mm_load_ps(float const *mem_addr) {
  return vreinterpretq_f32_m128(__riscv_vle32_v_f32m1(mem_addr, 4));
}

FORCE_INLINE __m128 _mm_load_ps1(float const *mem_addr) {
  float p[4] = {mem_addr[0], mem_addr[0], mem_addr[0], mem_addr[0]};
  return vreinterpretq_f32_m128(__riscv_vle32_v_f32m1(p, 4));
}

FORCE_INLINE __m128d _mm_load_sd(double const *mem_addr) {
  double p[2] = {mem_addr[0], 0};
  return vreinterpretq_f64_m128d(__riscv_vle64_v_f64m1(p, 2));
}

FORCE_INLINE __m128i _mm_load_si128(__m128i const *mem_addr) {
  return vreinterpretq_f64_m128i(
      __riscv_vle64_v_f64m1((double const *)mem_addr, 2));
}

FORCE_INLINE __m128 _mm_load_ss(float const *mem_addr) {
  float p[4] = {mem_addr[0], 0, 0, 0};
  return vreinterpretq_f32_m128(__riscv_vle32_v_f32m1(p, 4));
}

FORCE_INLINE __m128d _mm_load1_pd(double const *mem_addr) {
  return vreinterpretq_f64_m128d(__riscv_vfmv_v_f_f64m1(mem_addr[0], 2));
}

FORCE_INLINE __m128 _mm_load1_ps(float const *mem_addr) {
  return vreinterpretq_f32_m128(__riscv_vfmv_v_f_f32m1(mem_addr[0], 4));
}

// FORCE_INLINE __m128d _mm_loaddup_pd (double const* mem_addr) {}

// FORCE_INLINE __m128d _mm_loadh_pd (__m128d a, double const* mem_addr) {}

// FORCE_INLINE __m128 _mm_loadh_pi (__m128 a, __m64 const* mem_addr) {}

// FORCE_INLINE __m128i _mm_loadl_epi64 (__m128i const* mem_addr) {}

// FORCE_INLINE __m128d _mm_loadl_pd (__m128d a, double const* mem_addr) {}

// FORCE_INLINE __m128 _mm_loadl_pi (__m128 a, __m64 const* mem_addr) {}

// FORCE_INLINE __m128d _mm_loadr_pd (double const* mem_addr) {}

// FORCE_INLINE __m128 _mm_loadr_ps (float const* mem_addr) {}

FORCE_INLINE __m128d _mm_loadu_pd(double const *mem_addr) {
  return vreinterpretq_f64_m128d(__riscv_vle64_v_f64m1(mem_addr, 2));
}

FORCE_INLINE __m128 _mm_loadu_ps(float const *mem_addr) {
  return vreinterpretq_f32_m128(__riscv_vle32_v_f32m1(mem_addr, 4));
}

// FORCE_INLINE __m128i _mm_loadu_si128 (__m128i const* mem_addr) {}

// FORCE_INLINE __m128i _mm_loadu_si16 (void const* mem_addr) {}

// FORCE_INLINE __m128i _mm_loadu_si32 (void const* mem_addr) {}

// FORCE_INLINE __m128i _mm_loadu_si64 (void const* mem_addr) {}

// FORCE_INLINE __m128i _mm_madd_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_maddubs_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m64 _mm_maddubs_pi16 (__m64 a, __m64 b) {}

// FORCE_INLINE void* _mm_malloc (size_t size, size_t align) {}

// FORCE_INLINE void _mm_maskmove_si64 (__m64 a, __m64 mask, char* mem_addr) {}

// FORCE_INLINE void _mm_maskmoveu_si128 (__m128i a, __m128i mask, char*
// mem_addr) {}

// FORCE_INLINE void _m_maskmovq (__m64 a, __m64 mask, char* mem_addr) {}

// FORCE_INLINE __m128i _mm_max_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_max_epi32 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_max_epi8 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_max_epu16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_max_epu32 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_max_epu8 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128d _mm_max_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m64 _mm_max_pi16 (__m64 a, __m64 b) {}

// FORCE_INLINE __m128 _mm_max_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m64 _mm_max_pu8 (__m64 a, __m64 b) {}

// FORCE_INLINE __m128d _mm_max_sd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_max_ss (__m128 a, __m128 b) {}

// FORCE_INLINE void _mm_mfence (void) {}

// FORCE_INLINE __m128i _mm_min_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_min_epi32 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_min_epi8 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_min_epu16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_min_epu32 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_min_epu8 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128d _mm_min_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m64 _mm_min_pi16 (__m64 a, __m64 b) {}

// FORCE_INLINE __m128 _mm_min_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m64 _mm_min_pu8 (__m64 a, __m64 b) {}

// FORCE_INLINE __m128d _mm_min_sd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_min_ss (__m128 a, __m128 b) {}

// FORCE_INLINE __m128i _mm_minpos_epu16 (__m128i a) {}

// FORCE_INLINE __m128i _mm_move_epi64 (__m128i a) {}

// FORCE_INLINE __m128d _mm_move_sd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_move_ss (__m128 a, __m128 b) {}

// FORCE_INLINE __m128d _mm_movedup_pd (__m128d a) {}

// FORCE_INLINE __m128 _mm_movehdup_ps (__m128 a) {}

// FORCE_INLINE __m128 _mm_movehl_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128 _mm_moveldup_ps (__m128 a) {}

// FORCE_INLINE __m128 _mm_movelh_ps (__m128 a, __m128 b) {}

// FORCE_INLINE int _mm_movemask_epi8 (__m128i a) {}

// FORCE_INLINE int _mm_movemask_pd (__m128d a) {}

// FORCE_INLINE int _mm_movemask_pi8 (__m64 a) {}

// FORCE_INLINE int _mm_movemask_ps (__m128 a) {}

// FORCE_INLINE __m64 _mm_movepi64_pi64 (__m128i a) {}

// FORCE_INLINE __m128i _mm_movpi64_epi64 (__m64 a) {}

// FORCE_INLINE __m128i _mm_mpsadbw_epu8 (__m128i a, __m128i b, const int imm8)
// {}

// FORCE_INLINE __m128i _mm_mul_epi32 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_mul_epu32 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128d _mm_mul_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_mul_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128d _mm_mul_sd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_mul_ss (__m128 a, __m128 b) {}

// FORCE_INLINE __m64 _mm_mul_su32 (__m64 a, __m64 b) {}

// FORCE_INLINE __m128i _mm_mulhi_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_mulhi_epu16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m64 _mm_mulhi_pu16 (__m64 a, __m64 b) {}

// FORCE_INLINE __m128i _mm_mulhrs_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m64 _mm_mulhrs_pi16 (__m64 a, __m64 b) {}

// FORCE_INLINE __m128i _mm_mullo_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_mullo_epi32 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128d _mm_or_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_or_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128i _mm_or_si128 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_packs_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_packs_epi32 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_packus_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_packus_epi32 (__m128i a, __m128i b) {}

// FORCE_INLINE void _mm_pause (void) {}

FORCE_INLINE __m64 _m_pavgb(__m64 a, __m64 b) { return _mm_avg_pu8(a, b); }

FORCE_INLINE __m64 _m_pavgw(__m64 a, __m64 b) { return _mm_avg_pu16(a, b); }

// FORCE_INLINE int _m_pextrw (__m64 a, int imm8) {}

// FORCE_INLINE __m64 _m_pinsrw (__m64 a, int i, int imm8) {}

// FORCE_INLINE __m64 _m_pmaxsw (__m64 a, __m64 b) {}

// FORCE_INLINE __m64 _m_pmaxub (__m64 a, __m64 b) {}

// FORCE_INLINE __m64 _m_pminsw (__m64 a, __m64 b) {}

// FORCE_INLINE __m64 _m_pminub (__m64 a, __m64 b) {}

// FORCE_INLINE int _m_pmovmskb (__m64 a) {}

// FORCE_INLINE __m64 _m_pmulhuw (__m64 a, __m64 b) {}

// FORCE_INLINE void _mm_prefetch (char const* p, int i) {}

// FORCE_INLINE __m64 _m_psadbw (__m64 a, __m64 b) {}

// FORCE_INLINE __m64 _m_pshufw (__m64 a, int imm8) {}

// FORCE_INLINE __m128 _mm_rcp_ps (__m128 a) {}

// FORCE_INLINE __m128 _mm_rcp_ss (__m128 a) {}

// FORCE_INLINE __m128d _mm_round_pd (__m128d a, int rounding) {}

// FORCE_INLINE __m128 _mm_round_ps (__m128 a, int rounding) {}

// FORCE_INLINE __m128d _mm_round_sd (__m128d a, __m128d b, int rounding) {}

// FORCE_INLINE __m128 _mm_round_ss (__m128 a, __m128 b, int rounding) {}

// FORCE_INLINE __m128 _mm_rsqrt_ps (__m128 a) {}

// FORCE_INLINE __m128 _mm_rsqrt_ss (__m128 a) {}

// FORCE_INLINE __m128i _mm_sad_epu8 (__m128i a, __m128i b) {}

// FORCE_INLINE __m64 _mm_sad_pu8 (__m64 a, __m64 b) {}

FORCE_INLINE __m128i _mm_set_epi16(short e7, short e6, short e5, short e4,
                                   short e3, short e2, short e1, short e0) {
  short arr[8] = {e0, e1, e2, e3, e4, e5, e6, e7};
  return vreinterpretq_i16_m128i(__riscv_vle16_v_i16m1(arr, 8));
}

FORCE_INLINE __m128i _mm_set_epi32(int e3, int e2, int e1, int e0) {
  int arr[4] = {e0, e1, e2, e3};
  return vreinterpretq_i32_m128i(__riscv_vle32_v_i32m1(arr, 4));
}

FORCE_INLINE __m128i _mm_set_epi64(__m64 e1, __m64 e0) {
  vint32m1_t _e1 = vreinterpretq_m64_i32(e1);
  vint32m1_t _e0 = vreinterpretq_m64_i32(e0);
  return vreinterpretq_i32_m128i(__riscv_vslideup_vx_i32m1(_e0, _e1, 2, 4));
}

FORCE_INLINE __m128i _mm_set_epi64x(__int64 e1, __int64 e0) {
  __int64 arr[2] = {e0, e1};
  return vreinterpretq_i64_m128i(__riscv_vle64_v_i64m1(arr, 2));
}

FORCE_INLINE __m128i _mm_set_epi8(char e15, char e14, char e13, char e12,
                                  char e11, char e10, char e9, char e8, char e7,
                                  char e6, char e5, char e4, char e3, char e2,
                                  char e1, char e0) {
  char arr[16] = {e0, e1, e2,  e3,  e4,  e5,  e6,  e7,
                  e8, e9, e10, e11, e12, e13, e14, e15};
  return vreinterpretq_i8_m128i(
      __riscv_vle8_v_i8m1((const signed char *)arr, 16));
}

// FORCE_INLINE void _MM_SET_EXCEPTION_MASK (unsigned int a) {}

// FORCE_INLINE void _MM_SET_EXCEPTION_STATE (unsigned int a) {}

// FORCE_INLINE void _MM_SET_FLUSH_ZERO_MODE (unsigned int a) {}

FORCE_INLINE __m128d _mm_set_pd(double e1, double e0) {
  double arr[2] = {e0, e1};
  return vreinterpretq_f64_m128d(__riscv_vle64_v_f64m1(arr, 2));
}

FORCE_INLINE __m128d _mm_set_pd1(double a) {
  return vreinterpretq_f64_m128d(__riscv_vfmv_v_f_f64m1(a, 2));
}

FORCE_INLINE __m128 _mm_set_ps(float e3, float e2, float e1, float e0) {
  float arr[4] = {e0, e1, e2, e3};
  return vreinterpretq_f32_m128(__riscv_vle32_v_f32m1(arr, 4));
}

FORCE_INLINE __m128 _mm_set_ps1(float a) {
  return vreinterpretq_f32_m128(__riscv_vfmv_v_f_f32m1(a, 4));
}

// FORCE_INLINE void _MM_SET_ROUNDING_MODE (unsigned int a) {}

FORCE_INLINE __m128d _mm_set_sd(double a) {
  double arr[2] = {a, 0};
  return vreinterpretq_f64_m128d(__riscv_vle64_v_f64m1(arr, 2));
}

FORCE_INLINE __m128 _mm_set_ss(float a) {
  float arr[4] = {a, 0, 0, 0};
  return vreinterpretq_f32_m128(__riscv_vle32_v_f32m1(arr, 4));
}

FORCE_INLINE __m128i _mm_set1_epi16(short a) {
  return vreinterpretq_i16_m128i(__riscv_vmv_v_x_i16m1(a, 8));
}

FORCE_INLINE __m128i _mm_set1_epi32(int a) {
  return vreinterpretq_i32_m128i(__riscv_vmv_v_x_i32m1(a, 4));
}

FORCE_INLINE __m128i _mm_set1_epi64(__m64 a) {
  vint32m1_t _a = vreinterpretq_m64_i32(a);
  return vreinterpretq_i32_m128i(__riscv_vslideup_vx_i32m1(_a, _a, 2, 4));
}

FORCE_INLINE __m128i _mm_set1_epi64x(__int64 a) {
  return vreinterpretq_i64_m128i(__riscv_vmv_v_x_i64m1(a, 2));
}

FORCE_INLINE __m128i _mm_set1_epi8(char a) {
  return vreinterpretq_i8_m128i(__riscv_vmv_v_x_i8m1(a, 16));
}

FORCE_INLINE __m128d _mm_set1_pd(double a) {
  return vreinterpretq_f64_m128d(__riscv_vfmv_v_f_f64m1(a, 2));
}

FORCE_INLINE __m128 _mm_set1_ps(float a) {
  return vreinterpretq_f32_m128(__riscv_vfmv_v_f_f32m1(a, 4));
}

// FORCE_INLINE void _mm_setcsr (unsigned int a) {}

FORCE_INLINE __m128i _mm_setr_epi16(short e7, short e6, short e5, short e4,
                                    short e3, short e2, short e1, short e0) {
  short arr[8] = {e7, e6, e5, e4, e3, e2, e1, e0};
  return vreinterpretq_i16_m128i(__riscv_vle16_v_i16m1(arr, 8));
}

FORCE_INLINE __m128i _mm_setr_epi32(int e3, int e2, int e1, int e0) {
  int arr[4] = {e3, e2, e1, e0};
  return vreinterpretq_i32_m128i(__riscv_vle32_v_i32m1(arr, 4));
}

FORCE_INLINE __m128i _mm_setr_epi64(__m64 e1, __m64 e0) {
  vint32m1_t _e1 = vreinterpretq_m64_i32(e1);
  vint32m1_t _e0 = vreinterpretq_m64_i32(e0);
  return vreinterpretq_i32_m128i(__riscv_vslideup_vx_i32m1(_e1, _e0, 2, 4));
}

FORCE_INLINE __m128i _mm_setr_epi8(char e15, char e14, char e13, char e12,
                                   char e11, char e10, char e9, char e8,
                                   char e7, char e6, char e5, char e4, char e3,
                                   char e2, char e1, char e0) {
  char arr[16] = {e15, e14, e13, e12, e11, e10, e9, e8,
                  e7,  e6,  e5,  e4,  e3,  e2,  e1, e0};
  return vreinterpretq_i8_m128i(
      __riscv_vle8_v_i8m1((const signed char *)arr, 16));
}

FORCE_INLINE __m128d _mm_setr_pd(double e1, double e0) {
  double arr[2] = {e1, e0};
  return vreinterpretq_f64_m128d(__riscv_vle64_v_f64m1(arr, 2));
}

FORCE_INLINE __m128 _mm_setr_ps(float e3, float e2, float e1, float e0) {
  float arr[4] = {e3, e2, e1, e0};
  return vreinterpretq_f32_m128(__riscv_vle32_v_f32m1(arr, 4));
}

// FORCE_INLINE __m128d _mm_setzero_pd (void) {}

// FORCE_INLINE __m128 _mm_setzero_ps (void) {}

// FORCE_INLINE __m128i _mm_setzero_si128 () {}

// FORCE_INLINE void _mm_sfence (void) {}

// FORCE_INLINE __m128i _mm_shuffle_epi32 (__m128i a, int imm8) {}

// FORCE_INLINE __m128i _mm_shuffle_epi8 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128d _mm_shuffle_pd (__m128d a, __m128d b, int imm8) {}

// FORCE_INLINE __m64 _mm_shuffle_pi16 (__m64 a, int imm8) {}

// FORCE_INLINE __m64 _mm_shuffle_pi8 (__m64 a, __m64 b) {}

// FORCE_INLINE __m128 _mm_shuffle_ps (__m128 a, __m128 b, unsigned int imm8) {}

// FORCE_INLINE __m128i _mm_shufflehi_epi16 (__m128i a, int imm8) {}

// FORCE_INLINE __m128i _mm_shufflelo_epi16 (__m128i a, int imm8) {}

FORCE_INLINE __m128i _mm_sign_epi16(__m128i a, __m128i b) {
  vint16m1_t _a = vreinterpretq_m128i_i16(a);
  vint16m1_t _b = vreinterpretq_m128i_i16(b);

  vbool16_t lt_mask = __riscv_vmslt_vx_i16m1_b16(_b, 0, 8);
  vbool16_t zero_mask = __riscv_vmseq_vx_i16m1_b16(_b, 0, 8);
  vint16m1_t a_neg = __riscv_vneg_v_i16m1(_a, 8);
  vint16m1_t res_lt = __riscv_vmerge_vvm_i16m1(_a, a_neg, lt_mask, 8);
  return vreinterpretq_i16_m128i(
      __riscv_vmerge_vxm_i16m1(res_lt, 0, zero_mask, 8));
}

FORCE_INLINE __m128i _mm_sign_epi32(__m128i a, __m128i b) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _b = vreinterpretq_m128i_i32(b);

  vbool32_t lt_mask = __riscv_vmslt_vx_i32m1_b32(_b, 0, 4);
  vbool32_t zero_mask = __riscv_vmseq_vx_i32m1_b32(_b, 0, 4);
  vint32m1_t a_neg = __riscv_vneg_v_i32m1(_a, 4);
  vint32m1_t res_lt = __riscv_vmerge_vvm_i32m1(_a, a_neg, lt_mask, 4);
  return vreinterpretq_i32_m128i(
      __riscv_vmerge_vxm_i32m1(res_lt, 0, zero_mask, 4));
}

FORCE_INLINE __m128i _mm_sign_epi8(__m128i a, __m128i b) {
  vint8m1_t _a = vreinterpretq_m128i_i8(a);
  vint8m1_t _b = vreinterpretq_m128i_i8(b);

  vbool8_t lt_mask = __riscv_vmslt_vx_i8m1_b8(_b, 0, 16);
  vbool8_t zero_mask = __riscv_vmseq_vx_i8m1_b8(_b, 0, 16);
  vint8m1_t a_neg = __riscv_vneg_v_i8m1(_a, 16);
  vint8m1_t res_lt = __riscv_vmerge_vvm_i8m1(_a, a_neg, lt_mask, 16);
  return vreinterpretq_i8_m128i(
      __riscv_vmerge_vxm_i8m1(res_lt, 0, zero_mask, 16));
}

FORCE_INLINE __m64 _mm_sign_pi16(__m64 a, __m64 b) {
  vint16m1_t _a = vreinterpretq_m64_i16(a);
  vint16m1_t _b = vreinterpretq_m64_i16(b);

  vbool16_t lt_mask = __riscv_vmslt_vx_i16m1_b16(_b, 0, 4);
  vbool16_t zero_mask = __riscv_vmseq_vx_i16m1_b16(_b, 0, 4);
  vint16m1_t a_neg = __riscv_vneg_v_i16m1(_a, 4);
  vint16m1_t res_lt = __riscv_vmerge_vvm_i16m1(_a, a_neg, lt_mask, 4);
  return vreinterpretq_i16_m64(
      __riscv_vmerge_vxm_i16m1(res_lt, 0, zero_mask, 4));
}

FORCE_INLINE __m64 _mm_sign_pi32(__m64 a, __m64 b) {
  vint32m1_t _a = vreinterpretq_m64_i32(a);
  vint32m1_t _b = vreinterpretq_m64_i32(b);

  vbool32_t lt_mask = __riscv_vmslt_vx_i32m1_b32(_b, 0, 2);
  vbool32_t zero_mask = __riscv_vmseq_vx_i32m1_b32(_b, 0, 2);
  vint32m1_t a_neg = __riscv_vneg_v_i32m1(_a, 2);
  vint32m1_t res_lt = __riscv_vmerge_vvm_i32m1(_a, a_neg, lt_mask, 2);
  return vreinterpretq_i32_m64(
      __riscv_vmerge_vxm_i32m1(res_lt, 0, zero_mask, 2));
}

FORCE_INLINE __m64 _mm_sign_pi8(__m64 a, __m64 b) {
  vint8m1_t _a = vreinterpretq_m64_i8(a);
  vint8m1_t _b = vreinterpretq_m64_i8(b);

  vbool8_t lt_mask = __riscv_vmslt_vx_i8m1_b8(_b, 0, 8);
  vbool8_t zero_mask = __riscv_vmseq_vx_i8m1_b8(_b, 0, 8);
  vint8m1_t a_neg = __riscv_vneg_v_i8m1(_a, 8);
  vint8m1_t res_lt = __riscv_vmerge_vvm_i8m1(_a, a_neg, lt_mask, 8);
  return vreinterpretq_i8_m64(__riscv_vmerge_vxm_i8m1(res_lt, 0, zero_mask, 8));
}

// FORCE_INLINE __m128i _mm_sll_epi16 (__m128i a, __m128i count) {}

// FORCE_INLINE __m128i _mm_sll_epi32 (__m128i a, __m128i count) {}

// FORCE_INLINE __m128i _mm_sll_epi64 (__m128i a, __m128i count) {}

// FORCE_INLINE __m128i _mm_slli_epi16 (__m128i a, int imm8) {}

// FORCE_INLINE __m128i _mm_slli_epi32 (__m128i a, int imm8) {}

// FORCE_INLINE __m128i _mm_slli_epi64 (__m128i a, int imm8) {}

// FORCE_INLINE __m128i _mm_slli_si128 (__m128i a, int imm8) {}

// FORCE_INLINE __m128d _mm_sqrt_pd (__m128d a) {}

// FORCE_INLINE __m128 _mm_sqrt_ps (__m128 a) {}

// FORCE_INLINE __m128d _mm_sqrt_sd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_sqrt_ss (__m128 a) {}

// FORCE_INLINE __m128i _mm_sra_epi16 (__m128i a, __m128i count) {}

// FORCE_INLINE __m128i _mm_sra_epi32 (__m128i a, __m128i count) {}

// FORCE_INLINE __m128i _mm_srai_epi16 (__m128i a, int imm8) {}

// FORCE_INLINE __m128i _mm_srai_epi32 (__m128i a, int imm8) {}

// FORCE_INLINE __m128i _mm_srl_epi16 (__m128i a, __m128i count) {}

// FORCE_INLINE __m128i _mm_srl_epi32 (__m128i a, __m128i count) {}

// FORCE_INLINE __m128i _mm_srl_epi64 (__m128i a, __m128i count) {}

// FORCE_INLINE __m128i _mm_srli_epi16 (__m128i a, int imm8) {}

// FORCE_INLINE __m128i _mm_srli_epi32 (__m128i a, int imm8) {}

// FORCE_INLINE __m128i _mm_srli_epi64 (__m128i a, int imm8) {}

// FORCE_INLINE __m128i _mm_srli_si128 (__m128i a, int imm8) {}

FORCE_INLINE void _mm_store_pd(double *mem_addr, __m128d a) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  __riscv_vse64_v_f64m1(mem_addr, _a, 2);
}

FORCE_INLINE void _mm_store_pd1(double *mem_addr, __m128d a) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t a_arranged = __riscv_vrgather_vx_f64m1(_a, 0, 2);
  __riscv_vse64_v_f64m1(mem_addr, a_arranged, 2);
}

FORCE_INLINE void _mm_store_ps(float *mem_addr, __m128 a) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  __riscv_vse32_v_f32m1(mem_addr, _a, 4);
}

FORCE_INLINE void _mm_store_ps1(float *mem_addr, __m128 a) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t a_arranged = __riscv_vrgather_vx_f32m1(_a, 0, 4);
  __riscv_vse32_v_f32m1(mem_addr, a_arranged, 4);
}

FORCE_INLINE void _mm_store_sd(double *mem_addr, __m128d a) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  __riscv_vse64_v_f64m1(mem_addr, _a, 1);
}

FORCE_INLINE void _mm_store_si128(__m128i *mem_addr, __m128i a) {
  *mem_addr = a;
}

FORCE_INLINE void _mm_store_ss(float *mem_addr, __m128 a) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  __riscv_vse32_v_f32m1(mem_addr, _a, 1);
}

FORCE_INLINE void _mm_store1_pd(double *mem_addr, __m128d a) {
  return _mm_store_pd1(mem_addr, a);
}

FORCE_INLINE void _mm_store1_ps(float *mem_addr, __m128 a) {
  return _mm_store_ps1(mem_addr, a);
}

// FORCE_INLINE void _mm_storeh_pd (double* mem_addr, __m128d a) {}

// FORCE_INLINE void _mm_storeh_pi (__m64* mem_addr, __m128 a) {}

// FORCE_INLINE void _mm_storel_epi64 (__m128i* mem_addr, __m128i a) {}

// FORCE_INLINE void _mm_storel_pd (double* mem_addr, __m128d a) {}

// FORCE_INLINE void _mm_storel_pi (__m64* mem_addr, __m128 a) {}

// FORCE_INLINE void _mm_storer_pd (double* mem_addr, __m128d a) {}

// FORCE_INLINE void _mm_storer_ps (float* mem_addr, __m128 a) {}

// FORCE_INLINE void _mm_storeu_pd (double* mem_addr, __m128d a) {}

// FORCE_INLINE void _mm_storeu_ps (float* mem_addr, __m128 a) {}

// FORCE_INLINE void _mm_storeu_si128 (__m128i* mem_addr, __m128i a) {}

// FORCE_INLINE void _mm_storeu_si16 (void* mem_addr, __m128i a) {}

// FORCE_INLINE void _mm_storeu_si32 (void* mem_addr, __m128i a) {}

// FORCE_INLINE void _mm_storeu_si64 (void* mem_addr, __m128i a) {}

// FORCE_INLINE __m128i _mm_stream_load_si128 (void* mem_addr) {}

// FORCE_INLINE void _mm_stream_pd (void* mem_addr, __m128d a) {}

// FORCE_INLINE void _mm_stream_pi (void* mem_addr, __m64 a) {}

// FORCE_INLINE void _mm_stream_ps (void* mem_addr, __m128 a) {}

// FORCE_INLINE void _mm_stream_si128 (void* mem_addr, __m128i a) {}

// FORCE_INLINE void _mm_stream_si32 (void* mem_addr, int a) {}

// FORCE_INLINE void _mm_stream_si64 (void* mem_addr, __int64 a) {}

FORCE_INLINE __m128i _mm_sub_epi16(__m128i a, __m128i b) {
  vint16m1_t _a = vreinterpretq_m128i_i16(a);
  vint16m1_t _b = vreinterpretq_m128i_i16(b);
  return vreinterpretq_i16_m128i(__riscv_vsub_vv_i16m1(_a, _b, 8));
}

FORCE_INLINE __m128i _mm_sub_epi32(__m128i a, __m128i b) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _b = vreinterpretq_m128i_i32(b);
  return vreinterpretq_i32_m128i(__riscv_vsub_vv_i32m1(_a, _b, 4));
}

FORCE_INLINE __m128i _mm_sub_epi64(__m128i a, __m128i b) {
  vint64m1_t _a = vreinterpretq_m128i_i64(a);
  vint64m1_t _b = vreinterpretq_m128i_i64(b);
  return vreinterpretq_i64_m128i(__riscv_vsub_vv_i64m1(_a, _b, 2));
}

FORCE_INLINE __m128i _mm_sub_epi8(__m128i a, __m128i b) {
  vint8m1_t _a = vreinterpretq_m128i_i8(a);
  vint8m1_t _b = vreinterpretq_m128i_i8(b);
  return vreinterpretq_i8_m128i(__riscv_vsub_vv_i8m1(_a, _b, 16));
}

FORCE_INLINE __m128d _mm_sub_pd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  return vreinterpretq_f64_m128d(__riscv_vfsub_vv_f64m1(_a, _b, 2));
}

FORCE_INLINE __m128 _mm_sub_ps(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  return vreinterpretq_f32_m128(__riscv_vfsub_vv_f32m1(_a, _b, 4));
}

FORCE_INLINE __m128d _mm_sub_sd(__m128d a, __m128d b) {
  vfloat64m1_t _a = vreinterpretq_m128d_f64(a);
  vfloat64m1_t _b = vreinterpretq_m128d_f64(b);
  vfloat64m1_t sub = __riscv_vfsub_vv_f64m1(_a, _b, 2);
  vbool64_t mask = __riscv_vreinterpret_v_u8m1_b64(__riscv_vmv_v_x_u8m1(1, 8));
  return vreinterpretq_f64_m128d(__riscv_vmerge_vvm_f64m1(_a, sub, mask, 2));
}

FORCE_INLINE __m64 _mm_sub_si64(__m64 a, __m64 b) {
  vint64m1_t _a = vreinterpretq_m64_i64(a);
  vint64m1_t _b = vreinterpretq_m64_i64(b);
  return vreinterpretq_i64_m64(__riscv_vsub_vv_i64m1(_a, _b, 1));
}

FORCE_INLINE __m128 _mm_sub_ss(__m128 a, __m128 b) {
  vfloat32m1_t _a = vreinterpretq_m128_f32(a);
  vfloat32m1_t _b = vreinterpretq_m128_f32(b);
  vfloat32m1_t sub = __riscv_vfsub_vv_f32m1(_a, _b, 4);
  vbool32_t mask = __riscv_vreinterpret_v_u8m1_b32(__riscv_vmv_v_x_u8m1(1, 8));
  return vreinterpretq_f32_m128(__riscv_vmerge_vvm_f32m1(_a, sub, mask, 4));
}

// FORCE_INLINE __m128i _mm_subs_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_subs_epi8 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_subs_epu16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_subs_epu8 (__m128i a, __m128i b) {}

FORCE_INLINE int _mm_test_all_ones(__m128i a) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _mask = __riscv_vmv_v_x_i32m1(UINT32_MAX, 4);
  vint32m1_t a_not = __riscv_vnot_v_i32m1(_a, 4);
  vint32m1_t _and = __riscv_vand_vv_i32m1(a_not, _mask, 4);
  vint32m1_t redsum =
      __riscv_vredsum_vs_i32m1_i32m1(__riscv_vmv_v_x_i32m1(0, 4), _and, 4);
  return !(int)__riscv_vmv_x_s_i32m1_i32(redsum);
}

FORCE_INLINE int _mm_test_all_zeros(__m128i mask, __m128i a) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _mask = vreinterpretq_m128i_i32(mask);
  vint32m1_t _and = __riscv_vand_vv_i32m1(_a, _mask, 4);
  vint32m1_t redor =
      __riscv_vredor_vs_i32m1_i32m1(_and, __riscv_vmv_v_x_i32m1(0, 4), 4);
  return !(int)__riscv_vmv_x_s_i32m1_i32(redor);
}

FORCE_INLINE int _mm_test_mix_ones_zeros(__m128i mask, __m128i a) {
  vint32m1_t _mask = vreinterpretq_m128i_i32(mask);
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t zf_and = __riscv_vand_vv_i32m1(_a, _mask, 4);
  vint32m1_t zf_redor =
      __riscv_vredor_vs_i32m1_i32m1(zf_and, __riscv_vmv_v_x_i32m1(0, 4), 4);
  int zf_neg = (int)__riscv_vmv_x_s_i32m1_i32(zf_redor);

  vint32m1_t a_not = __riscv_vnot_v_i32m1(_a, 4);
  vint32m1_t cf_and = __riscv_vand_vv_i32m1(a_not, _mask, 4);
  vint32m1_t cf_redor =
      __riscv_vredor_vs_i32m1_i32m1(cf_and, __riscv_vmv_v_x_i32m1(0, 4), 4);
  int cf_neg = (int)__riscv_vmv_x_s_i32m1_i32(cf_redor);
  return !!(zf_neg | cf_neg);
}

FORCE_INLINE int _mm_testc_si128(__m128i a, __m128i b) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _b = vreinterpretq_m128i_i32(b);
  vint32m1_t a_not = __riscv_vnot_v_i32m1(_a, 4);
  vint32m1_t cf_and = __riscv_vand_vv_i32m1(a_not, _b, 4);
  vint32m1_t cf_redor =
      __riscv_vredor_vs_i32m1_i32m1(cf_and, __riscv_vmv_v_x_i32m1(0, 4), 4);
  return !(int)__riscv_vmv_x_s_i32m1_i32(cf_redor);
}

FORCE_INLINE int _mm_testnzc_si128(__m128i a, __m128i b) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _mask = vreinterpretq_m128i_i32(b);
  vint32m1_t zf_and = __riscv_vand_vv_i32m1(_a, _mask, 4);
  vint32m1_t zf_redor =
      __riscv_vredor_vs_i32m1_i32m1(zf_and, __riscv_vmv_v_x_i32m1(0, 4), 4);
  int zf_neg = !!(int)__riscv_vmv_x_s_i32m1_i32(zf_redor);

  vint32m1_t a_not = __riscv_vnot_v_i32m1(_a, 4);
  vint32m1_t cf_and = __riscv_vand_vv_i32m1(a_not, _mask, 4);
  vint32m1_t cf_redor =
      __riscv_vredor_vs_i32m1_i32m1(cf_and, __riscv_vmv_v_x_i32m1(0, 4), 4);
  int cf_neg = !!(int)__riscv_vmv_x_s_i32m1_i32(cf_redor);
  return zf_neg & cf_neg;
}

FORCE_INLINE int _mm_testz_si128(__m128i a, __m128i b) {
  vint32m1_t _a = vreinterpretq_m128i_i32(a);
  vint32m1_t _b = vreinterpretq_m128i_i32(b);
  vint32m1_t zf_and = __riscv_vand_vv_i32m1(_a, _b, 4);
  vint32m1_t zf_redor =
      __riscv_vredor_vs_i32m1_i32m1(zf_and, __riscv_vmv_v_x_i32m1(0, 4), 4);
  return !(int)__riscv_vmv_x_s_i32m1_i32(zf_redor);
}

// FORCE_INLINE void _MM_TRANSPOSE4_PS (__m128 row0, __m128 row1, __m128 row2,
// __m128 row3) {}

FORCE_INLINE int _mm_ucomieq_sd(__m128d a, __m128d b) {
  return _mm_comieq_sd(a, b);
}

FORCE_INLINE int _mm_ucomieq_ss(__m128 a, __m128 b) {
  return _mm_comieq_ss(a, b);
}

FORCE_INLINE int _mm_ucomige_sd(__m128d a, __m128d b) {
  return _mm_comige_sd(a, b);
}

FORCE_INLINE int _mm_ucomige_ss(__m128 a, __m128 b) {
  return _mm_comige_ss(a, b);
}

FORCE_INLINE int _mm_ucomigt_sd(__m128d a, __m128d b) {
  return _mm_comigt_sd(a, b);
}

FORCE_INLINE int _mm_ucomigt_ss(__m128 a, __m128 b) {
  return _mm_comigt_ss(a, b);
}

FORCE_INLINE int _mm_ucomile_sd(__m128d a, __m128d b) {
  return _mm_comile_sd(a, b);
}

FORCE_INLINE int _mm_ucomile_ss(__m128 a, __m128 b) {
  return _mm_comile_ss(a, b);
}

FORCE_INLINE int _mm_ucomilt_sd(__m128d a, __m128d b) {
  return _mm_comilt_sd(a, b);
}

FORCE_INLINE int _mm_ucomilt_ss(__m128 a, __m128 b) {
  return _mm_comilt_ss(a, b);
}

FORCE_INLINE int _mm_ucomineq_sd(__m128d a, __m128d b) {
  return _mm_comineq_sd(a, b);
}

FORCE_INLINE int _mm_ucomineq_ss(__m128 a, __m128 b) {
  return _mm_comineq_ss(a, b);
}

// FORCE_INLINE _mm_undefined_pd (void) {}

// FORCE_INLINE __m128 _mm_undefined_ps (void) {}

// FORCE_INLINE __m128i _mm_undefined_si128 (void) {}

// FORCE_INLINE __m128i _mm_unpackhi_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_unpackhi_epi32 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_unpackhi_epi64 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_unpackhi_epi8 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128d _mm_unpackhi_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_unpackhi_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128i _mm_unpacklo_epi16 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_unpacklo_epi32 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_unpacklo_epi64 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128i _mm_unpacklo_epi8 (__m128i a, __m128i b) {}

// FORCE_INLINE __m128d _mm_unpacklo_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_unpacklo_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128d _mm_xor_pd (__m128d a, __m128d b) {}

// FORCE_INLINE __m128 _mm_xor_ps (__m128 a, __m128 b) {}

// FORCE_INLINE __m128i _mm_xor_si128 (__m128i a, __m128i b) {}

/* AES */

// In the absence of crypto extensions, implement aesenc using regular NEON
// intrinsics instead. See:
// https://www.workofard.com/2017/01/accelerated-aes-for-the-arm64-linux-kernel/
// https://www.workofard.com/2017/07/ghash-for-low-end-cores/ and
// for more information.
// FORCE_INLINE __m128i _mm_aesenc_si128(__m128i a, __m128i RoundKey) {}

// Perform one round of an AES decryption flow on data (state) in a using the
// round key in RoundKey, and store the result in dst.
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_aesdec_si128
// FORCE_INLINE __m128i _mm_aesdec_si128(__m128i a, __m128i RoundKey) {}

// Perform the last round of an AES encryption flow on data (state) in a using
// the round key in RoundKey, and store the result in dst.
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_aesenclast_si128
// FORCE_INLINE __m128i _mm_aesenclast_si128(__m128i a, __m128i RoundKey) {}

// Perform the last round of an AES decryption flow on data (state) in a using
// the round key in RoundKey, and store the result in dst.
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_aesdeclast_si128
// FORCE_INLINE __m128i _mm_aesdeclast_si128(__m128i a, __m128i RoundKey) {}

// Perform the InvMixColumns transformation on a and store the result in dst.
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_aesimc_si128
// FORCE_INLINE __m128i _mm_aesimc_si128(__m128i a) {}

// Assist in expanding the AES cipher key by computing steps towards generating
// a round key for encryption cipher using data from a and an 8-bit round
// constant specified in imm8, and store the result in dst.
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_aeskeygenassist_si128
//
// Emits the Advanced Encryption Standard (AES) instruction aeskeygenassist.
// This instruction generates a round key for AES encryption. See
// https://kazakov.life/2017/11/01/cryptocurrency-mining-on-ios-devices/
// for details.
// FORCE_INLINE __m128i _mm_aeskeygenassist_si128(__m128i a, const int rcon) {}

/* Others */

// Perform a carry-less multiplication of two 64-bit integers, selected from a
// and b according to imm8, and store the results in dst.
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_clmulepi64_si128
// FORCE_INLINE __m128i _mm_clmulepi64_si128(__m128i _a, __m128i _b, const int
// imm) {}

// FORCE_INLINE unsigned int _sse2rvv_mm_get_denormals_zero_mode(void) {}

// Count the number of bits set to 1 in unsigned 32-bit integer a, and
// return that count in dst.
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_popcnt_u32
// FORCE_INLINE int _mm_popcnt_u32(unsigned int a) {}

// Count the number of bits set to 1 in unsigned 64-bit integer a, and
// return that count in dst.
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_popcnt_u64
// FORCE_INLINE int64_t _mm_popcnt_u64(uint64_t a) {}

// FORCE_INLINE void _sse2rvv_mm_set_denormals_zero_mode(unsigned int flag) {}

// Return the current 64-bit value of the processor's time-stamp counter.
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=rdtsc
// FORCE_INLINE uint64_t _rdtsc(void) {}

#if defined(__GNUC__) || defined(__clang__)
#pragma pop_macro("ALIGN_STRUCT")
#pragma pop_macro("FORCE_INLINE")
#endif

// #if defined(__GNUC__) && !defined(__clang__)
// #pragma GCC pop_options
// #endif

#endif
