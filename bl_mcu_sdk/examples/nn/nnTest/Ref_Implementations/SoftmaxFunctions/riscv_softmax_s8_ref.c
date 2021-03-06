/*
 * Copyright (C) 2010-2020 Arm Limited or its affiliates. All rights reserved.
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
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

/* ----------------------------------------------------------------------
 * Project:      NMSIS NN Library
 * Title:        riscv_softmax_s8.c
 * Description:  S8 softmax function
 *
 * $Date:        April 6, 2020
 * $Revision:    V.2.0.0
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "ref_functions.h"
#include "riscv_nnsupportfunctions.h"

#define ACCUM_BITS 12

/**
 *  @ingroup groupNN
 */

/**
 * @addtogroup Softmax
 * @{
 */

void riscv_softmax_s8_ref(const int8_t *input,
                    const int32_t num_rows,
                    const int32_t row_size,
                    const int32_t mult,
                    const int32_t shift,
                    const int32_t diff_min,
                    int8_t *output)
{
   const int32_t mask = (1 << shift);

    int32_t col = 0;
    int32_t row_idx;

    for (row_idx = 0; row_idx < num_rows; ++row_idx)
    {
        // Find the maximum value in order to ensure numerical stability
        int8_t max = *input;

        for (col = 1; col < row_size; ++col)
        {
            max = MAX(max, input[col]);
        }

        int32_t diff = 0;
        int32_t sum = 0;

        for (col = 0; col < row_size; ++col)
        {
            diff = input[col] - max;
            if (diff >= diff_min)
            {
                sum += DIV_POW2(EXP_ON_NEG(MUL_SAT(diff * mask, mult)), ACCUM_BITS);
            }
        }

        const int32_t headroom = __CLZ(sum);
        const int32_t bits_over_unit = ACCUM_BITS - headroom + 23;
        const int32_t shifted_scale = ONE_OVER1((sum << headroom) - (1 << 31));

        for (col = 0; col < row_size; ++col)
        {
            diff = input[col] - max;
            if (diff >= diff_min)
            {
                const int32_t res = DIV_POW2(MUL_SAT(shifted_scale, EXP_ON_NEG(MUL_SAT(diff * mask, mult))), bits_over_unit) - 128;
                output[col] = (int8_t)CLAMP(res, (int32_t)127, (int32_t)-128);
            }
            else
            {
                output[col] = -128;
            }
        }
        input += row_size;
        output += row_size;
    }
}
/**
 * @} end of Softmax group
 */
