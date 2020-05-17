Placement Basics {#tutorials_placement_basics}
===========

# Evolutionary Placement {#tutorials_placement_basics_epa}

Metagenomic studies often need to biologically classify millions of DNA sequences, for example
so-called *short reads*, coming from environments such as water, soil, or the human body.
The assignment of those reads to known reference sequences helps to assess
the composition and diversity of microbial communities and allows for comparing them.

Phylogenetic (or evolutionary) placement is a method to obtain a classification of metagenomic sequences in terms of a reference phylogenetic tree. The following publications introduce the topic in detail. They are the main programs to obtain phylogenetic placements:

 *  EPA-ng [[1]](#placement_references_1), the most recent, fastest, and most scalable implementation. [Webpage](https://github.com/Pbdas/epa-ng).
 *  Evolutionary Placement Algorithm (EPA) [[2]](#placement_references_2), implemented within RAxML.
    [Webpage](http://sco.h-its.org/exelixis/web/software/epa/index.html).
 *  pplacer [[3]](#placement_references_3).
    [Webpage](http://matsen.fhcrc.org/pplacer/).

<!-- In the following description, we will focus on EPA, although both programs work similarly. -->
The output of a phylogenetic placement analysis is standardized in the `jplace` file format [[4]](#placement_references_4).

The placement algorithm typically takes as input:

 *  An alignment of *reference sequences* (e.g., single 16S or barcoding gene).
 *  A species *tree* (usually inferred from the reference sequences).
 *  Aligned *query sequences* (e.g., Illumina reads).

![EPA input data.](placement/epa_input.png)

The algorithm finds the most likely (via maximum likelihood) insertion positions for every query
sequence on the reference tree. The resulting assignment of a query sequence to a branch is
called a *placement*. A query sequence can have multiple possible placement positions at different
branches of the tree, with different likelihoods.

The likelihoods of placement positions are usually transformed into the *likelihood weight ratio*.
For a given query sequence, those values sum up to 1.0 for all branches of the tree.
They can thus be seen as a probability distribution of possible placement positions on the tree.

![Placement positions for one query sequence.](placement/epa_placement.png)

The set of placements for a query sequence is called a *Pquery*. It contains a name (usually, that
is the name of the original query sequence) and the placements with their features (e.g., an ID of
the edge where the placement is located, its likelihood, etc.).
See [[4]](#placement_references_4) for details.

# Samples {#tutorials_placement_basics_samples}

Genesis has classes and functions to work with all relevant data of evolutionary placement.
This tutorial focuses on the actual placement data, that is, Pqueries with their placement
positions.
See the tutorial pages @ref tutorials_tree_basics and @ref tutorials_sequence for details on those
related topics.

The most important class for evolutionary placement is the @ref genesis::placement::Sample "Sample".
A sample is a representation of a whole `jplace` file: It stores the
@ref genesis::placement::PlacementTree "reference tree" and a set of
@ref genesis::placement::Pquery "Pqueries".

To read the data from a `jplace` file into a Sample, use a
@ref genesis::placement::JplaceReader "JplaceReader":

~~~{.cpp}
using namespace genesis::placement;
using namespace genesis::utils;

// Read a jplace file into a Sample object.
Sample sample = JplaceReader().read( from_file( "path/to/file.jplace" ));
~~~

Each Pquery contains the @ref genesis::placement::PqueryPlacement "Placements" of a query sequence,
as well as any @ref genesis::placement::PqueryName "Names" associated with it.

~~~{.cpp}
// Loop over the Pqueries of a Sample.
for( auto const& pquery : sample ) {

    LOG_INFO << "Inspected pquery has:";

    // Output all placement positions of this Pquery.
    for( auto const& placement : pquery.placements() ) {
        LOG_INFO << "  Placement with like_weight_ratio " << placement.like_weight_ratio
                 << " at edge with index " << placement.edge().index();
    }

    // Output all names of this Pquery.
    for( auto const& pname : pquery.names() ) {
        LOG_INFO << "Name " << pname.name;
    }
}
~~~

You can add Pqueries or find them like this:

~~~{.cpp}
// Add a new Pquery with a name and a placement.
auto new_pquery = sample.add();
new_pquery.add_name( "drosophila" );
new_pquery.add_placement( sample.tree().edge_at( 0 ));

// Find a Pquery given a name.
auto found_pquery = find_pquery( sample, "drosophila" );
if( found_pquery == nullptr ) {
    throw std::runtime_error( "Couldn't find Pquery." );
}
LOG_INFO << "Pquery contains " << found_pquery->placement_size() << " placements";
~~~

Removing placements with certain properties (filtering) works like this:

~~~{.cpp}
// Filter out all placements with a like_weight_ratio below 0.5.
filter_min_weight_threshold( sample, 0.5 );

// Filter out all but the two most likely placements.
filter_n_max_weight_placements( sample, 2 );

// Keep enough placements to get an accumulated like_weight_ratio of 98%, filter out the rest.
filter_min_accumulated_weight( sample, 0.98 );
~~~

Writing back your results to a new `jplace` file is done using a
@ref genesis::placement::JplaceWriter "JplaceWriter":

~~~{.cpp}
// Write the results back to a file.
JplaceWriter().write( sample, to_file( "path/to/new_file.jplace" ));
~~~

See the API reference for details and for more functions and classes related to this topic.

# References {#tutorials_placement_basics_references}

Please refer to the following articles for more information on phylogenetic placement of short
reads:

> <a name="placement_references_1">`[1]`</a> P. Barbera, A. M. Kozlov, L. Czech, B. Morel, D. Darriba, T.Flouri, A. Stamatakis,
> **EPA-ng: Massively Parallel Evolutionary Placement of Genetic Sequences**,
> *Systematic Biology, vol. 68, no. 2, pp. 365–369, 2018*.
> [DOI: 10.1093/sysbio/syy054](https://doi.org/10.1093/sysbio/syy054)

> <a name="placement_references_2">`[2]`</a> S. Berger, D. Krompass, and A. Stamatakis,
> **Performance, accuracy, and web server for evolutionary placement of short sequence reads
> under maximum likelihood**,
> *Systematic Biology, vol. 60, no. 3, pp. 291–302, 2011*.
> [DOI: 10.1093/sysbio/syr010](http://dx.doi.org/10.1093/sysbio/syr010)

> <a name="placement_references_3">`[3]`</a> F. A. Matsen, R. B. Kodner, and E. V. Armbrust,
> **pplacer: linear time maximum-likelihood and Bayesian phylogenetic placement of sequences
> onto a fixed reference tree**,
> *BMC Bioinformatics, vol. 11, no. 1, p. 538, 2010*.
> [DOI: 10.1186/1471-2105-11-538](http://dx.doi.org/10.1186/1471-2105-11-538)

> <a name="placement_references_4">`[4]`</a> F. A. Matsen, N. G. Hoffman, A. Gallagher, and A. Stamatakis,
> **A format for phylogenetic placements**,
> *PLoS One, vol. 7, no. 2, pp. 1–4, Jan. 2012*.
> [DOI: 10.1371/journal.pone.0031009](http://dx.doi.org/10.1371/journal.pone.0031009)
