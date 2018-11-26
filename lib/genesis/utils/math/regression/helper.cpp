/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.        See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.        If not, see <http://www.gnu.org/licenses/>.

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

/*
    =================================================================
            License
    =================================================================

    The implementation is based on the snp.matrix and X.snp.matrix classes by
    David Clayton <david.clayton@cimr.cam.ac.uk> and Hin-Tak Leung <htl10@users.sourceforge.net>.
    The source is in C, but is originally intended for usage in R.
    This file in particular is based on snpStats_1.32.0/src/mla.c

    The original code is published under the GNU General Public Licence version 3 (GPLv3).
    We use the same license, hence see above for details.

    The snp.matrix and X.snp.matrix classes.
    Copyright (C) 2008 David Clayton and Hin-Tak Leung

    The package does not seem to be maintained any more, and does not seem to
    have a proper repository. For more information, try these sites:
    https://bioconductor.org/packages/release/bioc/html/snpStats.html
    https://www.rdocumentation.org/packages/snpStats/
    http://www-gene.cimr.cam.ac.uk/clayton/software/
*/

#include "genesis/utils/math/regression/helper.hpp"

#include <cstdlib>
#include <limits>

namespace genesis {
namespace utils {

// =============================================================================
//     Linear Algebra Helper Functions
// =============================================================================

/* If resid==0, return a vector containing the appropriate stratum (weighted)
     means. Otherwise, center the input    vector around these. i.e    calculate
     either the "fitted value" or the residual from a model in which only
     strata are fitted. In this and following functions, ynew can coincide with
     y. Matrices are stored in Fortran order
     Returns number of empty strata */

int wcenter(const double *y, int n, const double *weight, const int *stratum,
                        int nstrata, int resid, double *ynew) {
    int i = 0, s = 0;
    if (!stratum) {
        if (!nstrata) {
            /* Nothing to do ... if necessary copy input to output */
            if (ynew != y)
                for (i = 0; i < n; i++)
                    ynew[i] = y[i];
            return (0);
        } else
            nstrata = 1;
    }
    int empty = 0;
    if (nstrata > 1) {
        double *swy, *swt;
        swy = (double *)calloc(nstrata, sizeof(double));
        swt = (double *)calloc(nstrata, sizeof(double));
        for (int i = 0; i < nstrata; ++i) {
            swy[i] = 0.0;
            swt[i] = 0.0;
        }

        if (weight) {
            for (i = 0; i < n; i++) {
                double wi = weight[i];
                int s = stratum[i] - 1;
                swt[s] += wi;
                swy[s] += wi * y[i];
            }
        } else {
            for (i = 0; i < n; i++) {
                int s = stratum[i] - 1;
                swt[s]++;
                swy[s] += y[i];
            }
        }
        for (s = 0; s < nstrata; s++) {
            double sws = swt[s];
            if (sws > 0.0)
                swy[s] /= sws;
            else
                empty++;
        }
        for (i = 0; i < n; i++) {
            int s = stratum[i] - 1;
            if (swt[s])
                ynew[i] = resid ? y[i] - swy[s] : swy[s];
        }
        free(swy);
        free(swt);
    } else {
        double swt = 0.0, swy = 0.0;
        if (weight) {
            for (i = 0; i < n; i++) {
                double wi = weight[i];
                swt += wi;
                swy += wi * y[i];
            }
        } else {
            for (i = 0; i < n; i++) {
                swy += y[i];
            }
            swt = (double)n;
        }
        swy /= swt;
        if (swt > 0)
            for (i = 0; i < n; i++)
                ynew[i] = resid ? y[i] - swy : swy;
        else
            empty = 1;
    }
    return (empty);
}

/* Replace y by residual from (weighted) regression through the origin
     Returns regression coefficient */

double wresid(const double *y, int n, const double *weight, const double *x,
                            double *ynew) {
    double swxx, swxy;
    swxy = swxx = 0.0;
    int i;
    if (weight) {
        for (i = 0; i < n; i++) {
            double wi = weight[i];
            double xi = x[i];
            double wx = wi * xi;
            swxy += wx * y[i];
            swxx += wx * xi;
        }
    } else {
        for (i = 0; i < n; i++) {
            double xi = x[i];
            swxy += xi * y[i];
            swxx += xi * xi;
        }
    }
    if (swxx > 0) {
        swxy /= swxx;
        for (i = 0; i < n; i++)
            ynew[i] = y[i] - swxy * x[i];
        return (swxy);
    } else {
        if (ynew != y) {
            for (i = 0; i < n; i++)
                ynew[i] = y[i];
        }
        return std::numeric_limits<double>::quiet_NaN();
    }
}

/* Weighted sum of squares */

double wssq(const double *y, int n, const double *weights) {
    double res = 0.0;
    if (weights) {
        for (int i = 0; i < n; i++) {
            double yi = y[i];
            res += weights[i] * yi * yi;
        }
    } else {
        for (int i = 0; i < n; i++) {
            double yi = y[i];
            res += yi * yi;
        }
    }
    return (res);
}

/* Weighted inner product */

double wspr(const double *y, const double *x, int n, const double *weights) {
    double res = 0.0;
    if (weights)
        for (int i = 0; i < n; i++)
            res += weights[i] * y[i] * x[i];
    else
        for (int i = 0; i < n; i++)
            res += y[i] * x[i];
    return (res);
}

/* Weighted sum */

double wsum(const double *y, int n, const double *weights) {
    double res = 0.0;
    if (weights)
        for (int i = 0; i < n; i++)
            res += weights[i] * y[i];
    else
        for (int i = 0; i < n; i++)
            res += y[i];
    return (res);
}

} // namespace utils
} // namespace genesis
