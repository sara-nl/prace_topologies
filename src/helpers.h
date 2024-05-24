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

#ifndef UNBALANCED_WORKLOAD_HELPERS_H
#define UNBALANCED_WORKLOAD_HELPERS_H

#include "General/structs.h"

class Helpers {
public:
    /*!
     * @brief Start the timer and return the current time (in seconds) starting
     *        from "some event in the past".
     */
    double tic();

    /*!
     * @brief Stop the timer and return the current time (in seconds) starting
     *        from "some event in the past".
     */
    double toc();

    /*!
     * @brief Parse the input parameters from CL.
     * @param argc Number of CL parameters.
     * @param argv CL parameters.
     * @param elts_glob Number of global elements in each direction.
     * @param num_procs Number of local elements in each direction.
     * @param type Decomposition type.
     */
    void parseInput(int argc, char** argv, IndicesIJ &elts_glob,
                    IndicesIJ &num_procs, int8_t &type);

private:
    /*!
     * @brief Terminate execution due to the error in the input parameters.
     */
    void terminateDueToParserFailure();
};


#endif //UNBALANCED_WORKLOAD_HELPERS_H
