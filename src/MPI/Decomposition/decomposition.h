/*
 * Copyright (c) 2024 Maksim Masterov, SURF
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef UNBALANCED_WORKLOAD_DECOMPOSITION_H
#define UNBALANCED_WORKLOAD_DECOMPOSITION_H

#include <vector>

#include "../../common.h"
#include "../../General/macro.h"
#include "../../General/structs.h"
#include "../../field.h"

/*!
 * \class DecompositionStruct
 * @brief Responsible for the structured data decomposition in a 1D or 2D way.
 */
class DecompositionStruct {
public:
    /*!
     * @brief Default constructor.
     */
    DecompositionStruct() : num_subdomains(1, 1) { }
    ~DecompositionStruct() { part.clear(); }

    /*!
     * @brief Decompose the domain.
     * @param num_procs [in] Number of subdomains in each direction.
     * @param elts_glob [in] Global number of elements/cells in each direction.
     * @param elts_loc [out] Local number of elements/cells in each direction.
     * @param beg_ind_glob [out] Global indices of the very first cell.
     * @return Returns EXIT_SUCCESS on success and EXIT_FAILURE on error.
     */
    int decompose(const IndicesIJ num_procs, const IndicesIJ elts_glob);

    void print(const std::string file_name, const IndicesIJ elts_glob);

    inline std::vector<int32_t>& getPartitioning() {
        return part;
    }

private:
    /*!
     * @brief Calculate the coordinates of the sub-domain based on its rank.
     * @param rank Rank of the process.
     * @param proc_ind_i [out] Coordinate in i-th direction.
     * @param proc_ind_j [out] Coordinate in j-th direction.
     */
    void getProcCoord(int rank, int &proc_ind_i, int &proc_ind_j);

private:
    std::vector<int32_t> part;

    IndicesIJ num_subdomains;   // Total number of subdomains in each direction
};

#endif //UNBALANCED_WORKLOAD_DECOMPOSITION_H
