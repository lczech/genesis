[![genesis](/doc/logo/logo_readme.png?raw=true "genesis")](http://genesis-lib.org/)

A library for working with phylogenetic data.

[![Build Status](https://travis-ci.org/lczech/genesis.svg?branch=master)](https://travis-ci.org/lczech/genesis)
[![License](https://img.shields.io/badge/license-GPLv3-blue.svg)](http://www.gnu.org/licenses/gpl.html)
![Language](https://img.shields.io/badge/language-C%2B%2B11-lightgrey.svg)
<!--![Language](https://img.shields.io/badge/language-python-lightgrey.svg)-->

Features
-------------------

Genesis is a C++ library for working with phylogenetic data.
A main focus are evolutionary placements of short environmental sequences
on a reference phylogenetic tree.
Such data is typically produced by tools like [EPA-ng](https://github.com/Pbdas/epa-ng),
[RAxML-EPA](http://sco.h-its.org/exelixis/web/software/epa/index.html) or
[pplacer](http://matsen.fhcrc.org/pplacer/) and usually stored in `jplace` files.

Some of the features of genesis:

 *  **Placements**
     *  Read, manipulate and write `jplace` files.
     *  Manipulate placement data: extract, filter, merge, and much more.
     *  Calculate distance measures (e.g., KR distance, EDPL).
     *  Run analyses like k-means Clustering, Squash Clustering, Edge PCA.
     *  Visualize aspects like read abundances or correlation with meta-data on the branches of the tree.
 *  **Trees**
     *  Read, annotate and write trees in various formats.
     *  Versatile tree data structure that can store any data on the edges and nodes.
     *  Easily iterate trees with different policies (e.g., postorder, preorder).
     *  Directly draw trees with colored branches to SVG files.
 *  **Sequences** and **Taxonomies**
     * Read, filter, manipulate and write sequences in `fasta` and `phylip` format.
     * Calculate consensus sequences with different methods.
     * Work with taxonomic paths and build a taxonomic hierarchy.
 *  **Utilities**
     * Math tools (matrices, histograms, statistics functions etc)
     * Color support (color lists, gradients etc, for making colored trees)
     * Various supportive file formats (bmp, csv, json, xml and more)

This is just an overview of the more prominent features.
See the [API reference](http://doc.genesis-lib.org/namespaces.html) for more.

Setup and Getting Started
-------------------

<!--
The functionality of genesis can be used in two ways:

 *  As a C++ library.
 *  As a Python module.
-->

For download and build instructions, see **[Setup](http://doc.genesis-lib.org/setup.html)**.

You futhermore find all the information for getting started with genesis in the
**[documentation](http://doc.genesis-lib.org/)**.
It contains a user manual with setup instructions and tutorials, as well as the full API reference.

For **user support**, please see our [support forum](http://support.genesis-lib.org/).
It is intended for questions on how to do things with genesis.

For **bug reports and feature requests**, please
[open an issue on our GitHub page](https://github.com/lczech/genesis/issues).

Showcase
-------------------

A focus point of the toolkit is to work with phylogenetic placements.
The following figure summarized the placement position of 7.5 mio short reads on a
reference tree with 190 taxa. The colour code indicates the number of reads placed
on each branch.

![Phylogenetic tree with coloured branches.](/doc/img/placement/visualize_placements.png?raw=true "Phylogenetic tree with coloured branches.")
