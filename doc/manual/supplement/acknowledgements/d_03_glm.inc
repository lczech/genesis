## Generalized Linear Model (GLM) @anchor supplement_acknowledgements_code_reuse_glm

The implementation of our Generalized Linear Model, in particular
@link genesis::utils::glm_fit() glm_fit()@endlink, but also some related helper functions,
is based on the snp.matrix and X.snp.matrix classes by
David Clayton <david.clayton@cimr.cam.ac.uk> and Hin-Tak Leung <htl10@users.sourceforge.net>.
The source is in C, but is originally intended for usage in R.

The package does not seem to be maintained any more, and does not seem to
have a proper repository. For more information, try these sites:
- https://bioconductor.org/packages/release/bioc/html/snpStats.html
- https://www.rdocumentation.org/packages/snpStats/
- http://www-gene.cimr.cam.ac.uk/clayton/software/

The original code is published under the GNU General Public Licence version 3 (GPLv3).

> The snp.matrix and X.snp.matrix classes.
> Copyright (C) 2008 David Clayton and Hin-Tak Leung

As we use the same license, see @ref supplement_license for the complete license.
