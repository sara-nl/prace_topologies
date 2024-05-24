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

#ifndef UNBALANCED_WORKLOAD_FIELD_H
#define UNBALANCED_WORKLOAD_FIELD_H

#include <vector>
#include <cmath>

#include "General/structs.h"

class Field {
public:
    Field();
    ~Field();

    /*!
     * @brief Initialize the field.
     * @param elts_loc Number of global elements.
     * @param elts_glob Number of local elements.
     */
    void initialize(IndicesIJ elts_loc, IndicesIJ elts_glob);

    /*!
     * @brief Generate the field.
     */
    void generate();

    /*!
     * @brief Print field into the file.
     * @param base_name Base name of the file.
     */
    void print(std::string base_name);

    /*!
     * @brif Distribute the field across processes.
     * Use the provided \e partitioning to distribute the field.
     * @param partitioning Partitioning of the field.
     * @param num_glob_elts Global number of elements.
     * @param root_pid PID of the process that stores the field and partitioning.
     */
    void distribute(int32_t* partitioning, int num_glob_elts, int root_pid);

    /*!
     * @brief Emulate some work by each process.
     */
    double performDummyWork();

    /*!
     * @brief Get number of elements in the field.
     */
    inline int getNumElts() {
        return data.size();
    }

    /*!
     * @brief Get reference to the field.
     */
    inline std::vector<double>& getData() {
        return data;
    }

    /*!
     * @brief Get reference to a particular element in the field.
     * @param n ID of the element.
     * @return Reference to the element.
     */
    inline double& operator()(int n) {
        return data[n];
    }

    /*!
     * @brief Get reference to a particular element in the field.
     * @param i i-th index of the element.
     * @param j j-th index of the element.
     * @return Reference to the element.
     */
    inline double& operator()(int i, int j) {
        return data[getLocalID(i, j)];
    }

    /*!
     * @brief Get local ID of the element.
     * @param i i-th index of the element.
     * @param j j-th index of the element.
     * @return Local for a process ID.
     */
    inline int getLocalID(int i, int j) {
        return j + _elts_loc.j * i;
    }

    /*!
     * @brief Evaluate the workload for the specified value.
     * @param value Value to be used during the evaluation.
     * @return Workload.
     */
    inline double getLocalLoad(double value) {
        return std::exp(15. * value);
    }

private:
    std::vector<double> data;
    IndicesIJ _elts_loc;
    IndicesIJ _elts_glob;
};


#endif //UNBALANCED_WORKLOAD_FIELD_H
