## Multi-Dimensional Scaling (MDS) @anchor supplement_acknowledgements_code_reuse_mds

The implementation of our
@link genesis::utils::multi_dimensional_scaling() multi_dimensional_scaling()@endlink function
is based on the [SimpleMatrix library](https://sites.google.com/site/simpmatrix/)
by [Quan Wang](https://github.com/wq2012), with his explicit permission to use this code in Genesis.
The copyright (C) of the implementation is held by Quan Wang, 2013.
We adapted the implementation to our @link genesis::utils::Matrix Matrix@endlink class and
changed the error reporting mechanism to exceptions.

The implementation by Quan Wang (and our adaptation) offers two variants of MDS. The code of these
variants is based on two Matlab implementations, and re-implements those in C++.
The original sources are:

 * The @link genesis::utils::MdsAlgorithm::kUcf UCF@endlink variant re-implements
   Laurens van der Maaten's MDS in his
   [Matlab Toolbox for Dimensionality Reduction](http://crcv.ucf.edu/source/dimension).
   It is free to use, change, or redistribute this code for non-commercial purposes.
 * The @link genesis::utils::MdsAlgorithm::kSmacof SMACOF@endlink variant re-implements
   Michael Bronstein's SMACOF in his
   [Matlab Toolbox for Surface Comparison and Analysis](http://tosca.cs.technion.ac.il/).
   It may be used for academic purposes, which we consider Genesis to be.

See there for further details. Note however that none of the original code of these two
implementations is used in Genesis, as we solely rely on the ported and re-implemented code
by Quan Wang.
