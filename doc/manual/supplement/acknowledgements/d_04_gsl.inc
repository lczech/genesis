## GSL (GNU Scientific Library) @anchor supplement_acknowledgements_code_reuse_gsl

The implementation of the multinomial distribution
@link genesis::utils::multinomial_distribution() multinomial_distribution()@endlink
is based on the GSL (GNU Scientific Library) function `gsl_ran_multinomial()`.
We further have an unused function
@link genesis::utils::hypergeometric_distribution_gsl() hypergeometric_distribution_gsl()@endlink
based on `gsl_ran_hypergeometric()` from the GSL.

The original code is published under the GNU General Public Licence version 3 (GPLv3).

> randist/multinomial.c
> Copyright (C) 2002 Gavin E. Crooks <gec@compbio.berkeley.edu>
>
> randist/hyperg.c
> Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 James Theiler, Brian Gough

As we use the same license, see @ref supplement_license for the complete license.
