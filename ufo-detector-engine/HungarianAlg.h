/*
 * UFO Detector | www.UFOID.net
 *
 * Copyright (C) 2016 UFOID
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HUNGARIANALG_H
#define HUNGARIANALG_H

#include <vector>
#include <iostream>
#include <limits>
#include <time.h>
#include <cstdlib>
#include <cfloat>

using namespace std;

/**
 * @brief Optimal assignment problem (minimum total cost) solver using Hungarian/Munkres algorithm.
 *
 * @see http://community.topcoder.com/tc?module=Static&d1=tutorials&d2=hungarianAlgorithm
 */
class AssignmentProblemSolver
{
private:
    /**
     * @brief Compute the optimal assignment (minimum overall costs) using Munkres algorithm.
     */
    void assignmentoptimal(int *assignment, double *cost, double *distMatrix, int nOfRows, int nOfColumns);

    void buildassignmentvector(int *assignment, bool *starMatrix, int nOfRows, int nOfColumns);
    void computeassignmentcost(int *assignment, double *cost, double *distMatrix, int nOfRows);
    void step2a(int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim);
    void step2b(int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim);
    void step3 (int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim);
    void step4 (int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim, int row, int col);
    void step5 (int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim);

    /**
     * @brief Compute a suboptimal solution. Good for cases with many forbidden assignments.
     */
    void assignmentsuboptimal1(int *assignment, double *cost, double *distMatrixIn, int nOfRows, int nOfColumns);

    /**
     * @brief Compute a suboptimal solution. Good for cases with many forbidden assignments.
     */
    void assignmentsuboptimal2(int *assignment, double *cost, double *distMatrixIn, int nOfRows, int nOfColumns);

public:
    enum TMethod { optimal, many_forbidden_assignments, without_forbidden_assignments };
    AssignmentProblemSolver();
    ~AssignmentProblemSolver();
    double Solve(vector<vector<double> >& DistMatrix,vector<int>& Assignment,TMethod Method=optimal);
};

#endif  // HUNGARIANALG_H
