[![genesis](/doc/logo/logo_readme.png?raw=true "genesis")](http://genesis-lib.org/)

A library for working with phylogenetic and population genetic data.

[![Build Status](https://travis-ci.org/lczech/genesis.svg?branch=master)](https://travis-ci.org/lczech/genesis)
[![License](https://img.shields.io/badge/license-GPLv3-blue.svg)](http://www.gnu.org/licenses/gpl.html)
[![Softwipe Score](https://img.shields.io/badge/softwipe-9.0/10.0-blue)](https://github.com/adrianzap/softwipe/wiki/Code-Quality-Benchmark)
![Language](https://img.shields.io/badge/language-C%2B%2B11-lightgrey.svg)
[![Platforms](https://img.shields.io/conda/pn/bioconda/gappa)](https://anaconda.org/bioconda/gappa)
<!--![Language](https://img.shields.io/badge/language-python-lightgrey.svg)-->

Features
-------------------

Genesis is a C++ library for working with phylogenetic and population genetic data:
<!-- Some of the features of genesis: -->

 *  **Trees**
     *  Read, annotate and write trees in various formats.
     *  Versatile tree data structure that can store any data on the edges and nodes.
     *  Easily iterate trees with different policies (e.g., postorder, preorder).
     *  Directly draw trees with colored branches to SVG files.
 *  **Placements**
     *  Read, manipulate and write `jplace` files from phylogenetic placement analyses.
     *  Manipulate placement data: extract, filter, merge, and much more.
     *  Calculate distance measures (e.g., KR distance, EDPL).
     *  Run analyses like k-means Clustering, Squash Clustering, Edge PCA.
     *  Visualize aspects like read abundances or correlation with meta-data on the branches of the tree.
 *  **Populations**
     *  Read and work with genome mapping and variant formats such as `pileup` and `vcf`.
     *  Iterate positions in a genome, individually or in windows.
     *  Compute statistics such as Tajima's D and F_ST for pool sequencing data.
 *  **Sequences** and **Taxonomies**
     *  Read, filter, manipulate and write sequences in `fasta`, `fastq`, and `phylip` format.
     *  Calculate consensus sequences with different methods.
     *  Work with taxonomic paths and build a taxonomic hierarchy.
 *  **Utilities**
     *  Math tools (matrices, histograms, statistics functions etc)
     *  Color support (color lists, gradients etc, for making colored trees)
     *  Various supportive file formats (bmp, csv, json, xml and more)

This is just an overview of the more prominent features.
See the [API reference](http://doc.genesis-lib.org/namespaces.html) for more.

Genesis is a library that is intended for researchers and developers who want to build their own
tools and methods, or run their own custom analyses. If you are simply interested in analyzing your
data with our methods, have a look at our command line tool [Gappa](https://github.com/lczech/gappa)
for many common phylogenetic placement analyses.

<!-- A main focus are evolutionary placements of short environmental sequences
on a reference phylogenetic tree.
Such data is typically produced by tools like [EPA-ng](https://github.com/Pbdas/epa-ng),
[RAxML-EPA](http://sco.h-its.org/exelixis/web/software/epa/index.html) or
[pplacer](http://matsen.fhcrc.org/pplacer/) and usually stored in `jplace` files. -->

Setup and Getting Started
-------------------

<!--
The functionality of genesis can be used in two ways:

 *  As a C++ library.
 *  As a Python module.
-->

For download and build instructions, see **[Setup](http://doc.genesis-lib.org/setup.html)**.

You furthermore find all the information for getting started with genesis in the
**[documentation](http://doc.genesis-lib.org/)**.
It contains a user manual with setup instructions and tutorials, as well as the full API reference.

For **bug reports and feature requests** of genesis, please
[open an issue on our GitHub page](https://github.com/lczech/genesis/issues).

For **user support** of the phylogenetic placement parts of the library, please see our
[Phylogenetic Placement Google Group](https://groups.google.com/forum/#!forum/phylogenetic-placement).
It is intended for discussions about phylogenetic placement,
and for user support for our software tools, such as [EPA-ng](https://github.com/Pbdas/epa-ng)
and [Gappa](https://github.com/lczech/gappa).

Showcases
-------------------

A focus point of the library is to work with phylogenetic placements.
The following figure summarized the placement position of 7.5 mio short reads on a
reference tree with 190 taxa. The color code indicates the number of reads placed
on each branch.

![Phylogenetic tree with coloured branches.](/doc/png/placement/visualize_placements.png?raw=true "Phylogenetic tree with coloured branches.")

This and other methods are presented in our manuscripts

> Methods for Inference of Automatic Reference Phylogenies and Multilevel Phylogenetic Placement.<br />
> Lucas Czech, Pierre Barbera, and Alexandros Stamatakis.<br />
> Bioinformatics, 2018. https://doi.org/10.1093/bioinformatics/bty767<br />
> <!-- bioRxiv, 2018. https://doi.org/10.1101/299792 -->

and

> Scalable Methods for Analyzing and Visualizing Phylogenetic Placement of Metagenomic Samples.<br />
> Lucas Czech and Alexandros Stamatakis.<br />
> PLOS One, 2019. https://doi.org/10.1371/journal.pone.0217050<br />
> <!-- bioRxiv, 2019. https://doi.org/10.1101/346353 -->

See there for more on what Genesis can do.

Citation
-------------------

When using Genesis, please cite

> Genesis and Gappa: processing, analyzing and visualizing phylogenetic (placement) data.<br />
> Lucas Czech, Pierre Barbera, and Alexandros Stamatakis.<br />
> Bioinformatics, 2020. https://doi.org/10.1093/bioinformatics/btaa070<br />

Also, see [Gappa](https://github.com/lczech/gappa) for our command line tool to run your own analyses.
