/******************************************************************************
 * @file     arm_math.h
 * @brief    Public header file for CMSIS DSP LibraryU
 * @version  V1.5.3
 * @date     10. January 2018
 ******************************************************************************/
/*
 * Copyright (c) 2010-2018 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ARM_MATH_H
#define _ARM_MATH_H

#include <math.h>
#include "string.h"
#include <stdint.h>

#include "omvhalmath.h"

#ifdef   __cplusplus
extern "C"
{
#endif
  /**
   * @brief 8-bit fractional data type in 1.7 format.
   */
  typedef int8_t q7_t;

  /**
   * @brief 16-bit fractional data type in 1.15 format.
   */
  typedef int16_t q15_t;

  /**
   * @brief 32-bit fractional data type in 1.31 format.
   */
  typedef int32_t q31_t;

  /**
   * @brief 64-bit fractional data type in 1.63 format.
   */
  typedef int64_t q63_t;

  /**
   * @brief 32-bit floating-point type definition.
   */
  typedef float float32_t;

  /**
   * @brief 64-bit floating-point type definition.
   */
  typedef double float64_t;

#if defined ( __GNUC__ )
  #define __SIMD32_TYPE int32_t
  #define CMSIS_UNUSED __attribute__((unused))
  #define CMSIS_INLINE __attribute__((always_inline))
  #define __STATIC_INLINE  static inline
#endif

#ifndef PI
  #define PI               3.14159265358979f
#endif


  /*
   * @brief C custom defined SMLAD for M3 and M0 processors
   */
  CMSIS_INLINE __STATIC_INLINE uint32_t __SMLAD(
  uint32_t x,
  uint32_t y,
  uint32_t sum)
  {
    return ((uint32_t)(((((q31_t)x << 16) >> 16) * (((q31_t)y << 16) >> 16)) +
                       ((((q31_t)x      ) >> 16) * (((q31_t)y      ) >> 16)) +
                       ( ((q31_t)sum    )                                  )   ));
  }

 /*
   * @brief C custom defined SMUAD for M3 and M0 processors
   */
  CMSIS_INLINE __STATIC_INLINE uint32_t __SMUAD(
  uint32_t x,
  uint32_t y)
  {
    return ((uint32_t)(((((q31_t)x << 16) >> 16) * (((q31_t)y << 16) >> 16)) +
                       ((((q31_t)x      ) >> 16) * (((q31_t)y      ) >> 16))   ));
  }

  /**
   * @brief definition to pack two 16 bit values.
   */
#define __PKHBT(ARG1, ARG2, ARG3) ( (((int32_t)(ARG1) <<    0) & (int32_t)0x0000FFFF) | \
                                    (((int32_t)(ARG2) << ARG3) & (int32_t)0xFFFF0000)  )
#define __PKHTB(ARG1, ARG2, ARG3) ( (((int32_t)(ARG1) <<    0) & (int32_t)0xFFFF0000) | \
                                    (((int32_t)(ARG2) >> ARG3) & (int32_t)0x0000FFFF)  )

__attribute__((always_inline)) __STATIC_INLINE uint32_t __RBIT(uint32_t value)
{
  uint32_t result;
  uint32_t s = (4U /*sizeof(v)*/ * 8U) - 1U; /* extra shift needed at end */

  result = value;                      /* r will be reversed bits of v; first get LSB of v */
  for (value >>= 1U; value != 0U; value >>= 1U)
  {
    result <<= 1U;
    result |= value & 1U;
    s--;
  }
  result <<= s;                        /* shift when v's highest bits are zero */
  return result;
}

/**
  \brief   Signed Saturate
  \details Saturates a signed value.
  \param [in]  value  Value to be saturated
  \param [in]    sat  Bit position to saturate to (1..32)
  \return             Saturated value
 */
__attribute__((always_inline)) __STATIC_INLINE int32_t __SSAT(int32_t val, uint32_t sat)
{
  if ((sat >= 1U) && (sat <= 32U))
  {
    const int32_t max = (int32_t)((1U << (sat - 1U)) - 1U);
    const int32_t min = -1 - max ;
    if (val > max)
    {
      return max;
    }
    else if (val < min)
    {
      return min;
    }
  }
  return val;
}

/**
  \brief   Unsigned Saturate
  \details Saturates an unsigned value.
  \param [in]  value  Value to be saturated
  \param [in]    sat  Bit position to saturate to (0..31)
  \return             Saturated value
 */
__attribute__((always_inline)) __STATIC_INLINE uint32_t __USAT(int32_t val, uint32_t sat)
{
  if (sat <= 31U)
  {
    const uint32_t max = ((1U << sat) - 1U);
    if (val > (int32_t)max)
    {
      return max;
    }
    else if (val < 0)
    {
      return 0U;
    }
  }
  return (uint32_t)val;
}

/*
   * @brief C custom defined QADD16 for M3 and M0 processors
   */
CMSIS_INLINE __STATIC_INLINE uint32_t __QADD16(uint32_t x, uint32_t y)
{
  /*  q31_t r,     s;  without initialisation 'arm_offset_q15 test' fails  but 'intrinsic' tests pass! for armCC */
  q31_t r = 0, s = 0;

  r = __SSAT(((((q31_t)x << 16) >> 16) + (((q31_t)y << 16) >> 16)), 16) & (int32_t)0x0000FFFF;
  s = __SSAT(((((q31_t)x) >> 16) + (((q31_t)y) >> 16)), 16) & (int32_t)0x0000FFFF;

  return ((uint32_t)((s << 16) | (r)));
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __USAT_ASR(int32_t v, uint32_t bitpos, uint32_t rs)
{
  return __USAT(v >> rs, bitpos);
}

#ifdef   __cplusplus
}
#endif

#endif /* _ARM_MATH_H */
