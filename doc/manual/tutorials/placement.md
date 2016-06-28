Placement {#placement}
===========

# Evolutionary Placement {#placement_epa}

Metagenomic studies often need to biologically classify millions of DNA sequences, for example
so-called *short reads*. The assignment of those reads to known reference sequences helps to assess
the composition and diversity of microbial communities and allows for comparing them.

For obtaining these assignments, you can use
 *  Evolutionary Placement Algorithm (EPA) [[1]](#placement_references_1), implemented within RAxML.
    [Webpage](http://sco.h-its.org/exelixis/web/software/epa/index.html).
 *  pplacer [[2]](#placement_references_2).
    [Webpage](http://matsen.fhcrc.org/pplacer/).

In the following description, we will focus on EPA, although both programs work similarly.
Their output is standardized in the `jplace` file format [[3]](#placement_references_3),
thus, genesis works with both of them.

The EPA takes as input:

 *  An alignment of *reference sequences* (e.g., single 16S or barcoding gene).
 *  A species *tree* (usually inferred from the reference sequences).
 *  Aligned *query sequences* (e.g., Illumina reads).

![EPA input data.](epa_input.png)

The algorithm finds the most likely (via maximum likelihood) insertion positions for every query
sequence on the reference tree. The resulting assignment of a query sequence to a branch is
called a *placement*. A query sequence can have multiple possible placement positions at different
branches of the tree, with different likelihoods.

The likelihoods of placement positions are usually transformed into the *likelihood weight ratio*.
For a given query sequence, those values sum up to 1.0 for all branches of the tree.
They can thus be seen as a probability distribution of possible placement positions on the tree.

![Placement positions for one query sequence.](epa_placement.png)

The set of placements for a query sequence is called a *Pquery*. It contains a name (usually, that
is the name of the original query sequence) and the placements with their features (e.g., an ID of
the edge where the placement is located, its likelihood, etc.).
See [[3]](#placement_references_3) for details.

# Samples {#placement_samples}

Genesis has classes and functions to work with all relevant data of evolutionary placement.
This tutorial focuses on the actual placement data, that is, Pqueries with their placement
positions.
See the tutorial pages @subpage tree and @subpage sequence for details on those related topics.

The most important class for evolutionary placement is the @ref genesis::placement::Sample "Sample".
A sample is a representation of a whole `jplace` file: It stores the
@ref genesis::placement::PlacementTree "reference tree" and a set of
@ref genesis::placement::Pquery "Pqueries".

To read the data from a `jplace` file into a Sample, use a
@ref genesis::placement::JplaceReader "JplaceReader":

~~~{.cpp}
using namespace genesis::placement;

// Read a jplace file into a Sample object.
Sample sample;
JplaceReader().from_file( "path/to/file.jplace", sample );
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
auto new_pquery = sample.add_pquery();
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
JplaceWriter().to_file( sample, "path/to/new_file.jplace" );
~~~

You can find the whole code listing of this tutorial at

    ./doc/tutorials/samples.cpp

See the API reference for details and for more functions and classes related to this topic.

# References {#placement_references}

Please refer to the following articles for more information on phylogenetic placement of short
reads:

> <a name="placement_references_1">`[1]`</a> S. Berger, D. Krompass, and A. Stamatakis,
> **Performance, accuracy, and web server for evolutionary placement of short sequence reads
> under maximum likelihood**,
> *Syst. Biol., vol. 60, no. 3, pp. 291–302, 2011*.

> <a name="placement_references_2">`[2]`</a> F. A. Matsen, R. B. Kodner, and E. V. Armbrust,
> **pplacer: linear time maximum-likelihood and Bayesian phylogenetic placement of sequences
> onto a fixed reference tree**,
> *BMC Bioinformatics, vol. 11, no. 1, p. 538, 2010*.

> <a name="placement_references_3">`[3]`</a> F. A. Matsen, N. G. Hoffman, A. Gallagher, and A. Stamatakis,
> **A format for phylogenetic placements**,
> *PLoS One, vol. 7, no. 2, pp. 1–4, Jan. 2012*.
