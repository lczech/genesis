Placement {#placement}
===========

<!--
# Evolutionary Placement

Metagenomic studies often need to biologically classify millions of reads. This assignment of
reads to known reference sequences helps to assess the composition and diversity of microbial
communities and allows for comparing them.

The Evolutionary Placement Algorithm (EPA) takes as input:

 *  An alignment of reference sequences (e.g., single 16S or barcoding gene).
 *  A species tree (usually inferred from the reference sequences).
 *  Aligned query sequences (e.g., Illumina reads).

![EPA input data.](epa_input.png)

The algorithm finds the most likely (via maximum likelihood) insertion position for every query
sequence on the reference tree. The resulting assignment of a query sequence to a branch is
called a "placement". A query sequence can have multiple possible placement positions at different
branches of the tree, with different likelihoods.

![Placement positions for one query sequence.](epa_placement.png)

See Section @ref placement_references for the original articles describing evolutionary placement
of short reads `[1,2]` and the file format for storing those placements, the `jplace` format `[3]`.
-->

# Samples

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
// Filter out all placements with a like_weight_ration below 0.5,
// filter out all but the two most likely placements.
filter_min_weight_threshold(    sample, 0.5 );
filter_n_max_weight_placements( sample, 2   );
~~~

Writing back your results to a new `jplace` file is done using a
@ref genesis::placement::JplaceWriter "JplaceWriter":

~~~{.cpp}
// Write the results back to a file.
JplaceWriter().to_file( sample, "path/to/new_file.jplace" );
~~~

You can find the whole code listing of this tutorial at

    ./doc/tutorials/samples.cpp

# References
<!-- # References {#placement_references} -->

Please refer to the following articles for more information on phylogenetic placement of short
reads:

> `[1]` S. Berger, D. Krompass, and A. Stamatakis,
> **Performance, accuracy, and web server for evolutionary placement of short sequence reads
> under maximum likelihood**,
> *Syst. Biol., vol. 60, no. 3, pp. 291–302, 2011*.

> `[2]` F. A. Matsen, R. B. Kodner, and E. V. Armbrust,
> **pplacer: linear time maximum-likelihood and Bayesian phylogenetic placement of sequences
> onto a fixed reference tree**,
> *BMC Bioinformatics, vol. 11, no. 1, p. 538, 2010*.

> `[3]` F. A. Matsen, N. G. Hoffman, A. Gallagher, and A. Stamatakis,
> **A format for phylogenetic placements**,
> *PLoS One, vol. 7, no. 2, pp. 1–4, Jan. 2012*.
