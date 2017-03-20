Extract Clade Placements {#demos_extract_clade_placements}
===========

@section Summary

Takes a `jplace` file and a mapping of the taxa of its reference tree to some clade names.
It outputs a `jplace` file for each clade, where each file contains all
@ref genesis::placement::Pquery "Pqueries" that fell into that clade.

This demo is located at

    genesis/doc/demos/extract_clade_placements.cpp

@section Details

The program takes three input arguments in the following order:

 1. A `jplace` input file. The pqueries in this file are then split into different samples. Each
    such sample contains all pqueries whose placements are placed in a certain clade of the
    reference tree with more than a cutoff threshold of their accumulated likelihood weight.

    According to the `jplace` standard, each pquery can have multiple possible placement positions.
    Each position has a value `like_weight_ratio`, which can be interpreted as a measure of
    probability of how likely the placement belongs to the branch that it is attached to.
    The ratios for all branches of the tree thus sum up to 1.0.

    If more of this placement mass than the threshold is placed on the branches of a single
    clade of the tree, the according pquery is assigned to that clade. The threshold is
    hardcoded in this demo and set to 0.95 (but can be changed if needed, of course).

    It is possible that the placement algorithm (e.g., EPA or pplacer) did not output placements
    with low like_weight_ratios, depending on the selected options (see the respective manual
    for more details on how to change this). This means that the provided sum might be lower
    than 1.0 for some pqueries. In order to compensate for this (thus, to avoid classifying those
    pqueries as uncertain), we normalize the like_weight_ratios first, so that their sum is 1.0
    again. This step thus ignores the uncertainties resulting from the placement algorithm.
 2. A path to a file, which needs to contain a single line for each taxon of the reference tree.
    Each line needs to contain a tab-separated entry that maps from a taxon of the tree to the
    clade name that this taxon belongs to:

        Taxon_1 <tab> clade_a

    (where the " <tab> " of course is just a single tab character).
    The taxa names need to be the same as the node names of the reference tree in the `jplace` file.

    If a taxon in a clade file is not found on the tree, a warning is issued, and the taxon is
    ignored. If the tree contains taxa which are not in any clade file, those branches are
    assigned to a special clade "basal_branches". This is also the case for the inner branches
    of the tree: all those branches which do not belong to one of the clades are collected in
    this special clade.

    As a second special clade, the "uncertain" clade is used to collect all those pqueries
    which did not fall into any clade with more than the threshold of accumulated likelihood
    weights.

    The edges that belong to a clade are determined by finding the smalles subtree (split) of
    the tree that contains all nodes of the clade. That means, the clades should be monophyletic
    in order for this algorithm to work properly. Furthermore, the user needs to make sure that
    each taxon is contained in at most one clade. Otherwise, the algorithm won't work properly.

    Remark: The rooting of the tree is insignificant for this program. Even if the root
    coincidentally lies within one of the clades, the result is the same. The program does not
    change the root; thus, when visualizing the clades, be aware that the tree might look different
    depending on the rooting.
 3. An output directory path. For each clade (including the two special clades), a `jplace` file
    named after the clade is written to that path. Each `jplace` file then contains all pqueries
    that were assigned to that clade.

A typical use case for this program is to extract pqueries that were placed in a particular
clade of interest in an evolutionary placement analysis. The extracted placements can then be
further examined in downstream analyses.

It is also possible to do a second run of evolutionary placement with the original sequences of
the pqueries of one clade, using a refined reference tree for that clade with a higher resolution
(more reference taxa). This two-step placement approach allows for finely grained
placement positions while keeping the computational load relatively small.

@section Example

Example files to test the demo are located at

    genesis/doc/demos/extract_clade_placements/

This directory contains an `example.jplace` file, a `clades.tsv` file listing the taxa of the
reference tree of the `jplace` file and assigning each of them to either `clade_a` or `clade_b`,
and an empty `output` directory.

After compiling the demo program (using `make update`), you can run the example like this

    cd genesis/doc/demos/extract_clade_placements/
    ../../../bin/extract_clade_placements example.jplace clades.tsv output/

The expected output of the program is

    INFO Using jplace file      example.jplace
    INFO Using clade file       clades.tsv
    INFO Using output directory output/
    INFO Found 2 clades
    INFO Finished.

You will then find four output files in `./doc/demos/extract_clade_placements/output/`:

 *  `basal_branches.jplace` contains 3 pqueries, which have most of their placement mass in the
    branches of the tree that do not belong to any of the two clades.
 *  `clade_a.jplace` and `clade_b.jplace` contain 6 and 4 pqueries, which were located in those
    two clades.
 *  `uncertain.jplace` contains a jplace file without any pqueries, as in this example all
    pqueries have more than the treshold of their mass in one clade (or in the basal branches).

Using this scaffolding, you can run the demo with your own data.
