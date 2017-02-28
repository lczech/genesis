#ifndef GENESIS_UTILS_MATH_MATRIX_PCA_H_
#define GENESIS_UTILS_MATH_MATRIX_PCA_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/matrix.hpp"

#include <vector>

namespace genesis {
namespace utils {

// ================================================================================================
//     Settings for Functions
// ================================================================================================

/**
 * @brief Setting for principal_component_analysis() to determine which form of standardization
 * of the data to use prior to perfoming the PCA.
 */
enum class PcaStandardization
{
    /**
     * @brief Standardize the mean and variance of the data before performing the PCA.
     *
     * Best to use when the data has widely varying scales in between the columns, i.e., if the
     * columns present data from different domains.
     * Also, see correlation_matrix().
     */
    kCorrelation,

    /**
     * @brief Standardize the mean, but not the variance of the data before performing the PCA.
     *
     * Best to use when the data has similar scales in between the columns, i.e., if the columns
     * present data from the same domain.
     * Also, see covariance_matrix().
     */
    kCovariance,

    /**
     * @brief Do not perform any standardization prior to performing the PCA.
     *
     * Not recommended.
     * Also, see sums_of_squares_and_cross_products_matrix().
     */
    kSSCP
};

// ================================================================================================
//     Structs for Storing Results
// ================================================================================================

/**
 * @brief Helper structure used for the eigenvalue decomposition in reduce_to_tridiagonal_matrix()
 * and tridiagonal_ql_algorithm(). See there for details.
 */
struct TridiagonalDecompositionData
{
    std::vector<double> eigenvalues;
    std::vector<double> intermediates;
};

/**
 * @brief Helper stucture that collects the output of principal_component_analysis().
 */
struct PcaData
{
    std::vector<double> eigenvalues;
    Matrix<double>      eigenvectors;
    Matrix<double>      projection;
};

// ================================================================================================
//     Helper Functions
// ================================================================================================

/**
 * @brief Triangular decomposition of a symmetric matrix.
 *
 * This function performs an intermediate step that is usually followed by an execution of the
 * tridiagonal_ql_algorithm().
 *
 * It uses the Householder reduction of matrix a to tridiagonal form, according to
 *
 *     Martin et al., Num. Math. 11, 181-195, 1968.
 *     Ref: Smith et al., Matrix Eigensystem Routines -- EISPACK Guide
 *     Springer-Verlag, 1976, pp. 489-494.
 *     W H Press et al., Numerical Recipes in C, Cambridge U P, 1988, pp. 373-374.
 *
 * There, the function is called `tred2()`.
 *
 * The implementation of this function is largely borrowed from
 * http://www.classification-society.org/csna/mda-sw/pca.c
 * published as Public Domain by Fionn Murtagh.
 *
 * @param data Input data matrix. Operations are performed inline, that is, the matrix is changed
 *             during execution of the function.
 */
TridiagonalDecompositionData reduce_to_tridiagonal_matrix(
    Matrix<double>& data
);

/**
 * @brief Reduce a symmetric matrix to a symmetric tridiagonal matrix.
 *
 * Uses the data and the intermediate result after executing reduce_to_tridiagonal_matrix()
 * in order to run the algorithm. See there for details.
 *
 * Both parameters are modified during the algorithm. After it is finished,
 * `tri.eigenvalues` contains the eigenvalus and `data` the associated eigenvectors of the
 * original data.
 *
 * The implementation of this function is largely borrowed from
 * http://www.classification-society.org/csna/mda-sw/pca.c
 * published as Public Domain by Fionn Murtagh.
 *
 * @param data           Input data matrix. Operations are performed inline, that is, the matrix is
 *                       changed during execution of the function.
 * @param tri            Tridiagonal matrix coming from reduce_to_tridiagonal_matrix().
 * @param max_iterations Maximum number of iterations to perform, default is 1000. If set to 0, the
 *                       algorithm continues until convergence. As convergence is not given for all
 *                       data, this might result in an infinite loop.
 */
void tridiagonal_ql_algorithm(
    Matrix<double>&               data,
    TridiagonalDecompositionData& tri,
    size_t                        max_iterations = 1000
);

// ================================================================================================
//     Principal Component Analysis
// ================================================================================================

/**
 * @brief Perfom a Principal Component Analysis on a given `data` Matrix.
 *
 * Principal Components Analysis or the Karhunen-Loeve expansion is a classical method for
 * dimensionality reduction or exploratory data analysis. One reference among many is:
 * F. Murtagh and A. Heck, Multivariate Data Analysis, Kluwer Academic, Dordrecht, 1987.
 *
 * @param data            Matrix with the data, samples in rows, features in columns.
 * @param components      Intended number of PCA components to calculate. If 0 (default),
 *                        all PCA components are calculated, i.e., as many components as
 *                        `data` has columns.
 * @param standardization Indicate the standardization algorithm to perfom on the `data` before
 *                        calculating the PCA components, see ::PcaStandardization.
 * @return                A struct that contains the eigenvalues and corresponding eigenvectors
 *                        (i.e., the PCA components), and a Matrix with the projected `data`.
 *                        See PcaData for details.
 */
PcaData principal_component_analysis(
    Matrix<double> const& data,
    size_t                components      = 0,
    PcaStandardization    standardization = PcaStandardization::kCorrelation
);

} // namespace utils
} // namespace genesis

#endif // include guard
