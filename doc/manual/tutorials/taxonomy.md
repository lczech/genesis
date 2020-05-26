Taxonomy {#tutorials_taxonomy}
===========

A taxonomy is a classification of groups of biological organisms on the basis of shared characteristics. Typically, a taxonomy forms a hierarchy of taxonomic names, where higher level taxa are more general groups of organisms that subsume lower, more specialized taxonomic levels.

In genesis, we model this hierarchy using two classes:

 * A @ref genesis::taxonomy::Taxonomy "Taxonomy" is the container that stores the taxonomy.
 * A @ref genesis::taxonomy::Taxon "Taxon" is one element in a taxonomy, that is, one taxonomic name. Due to the nested nature of the hierarchy, a @ref genesis::taxonomy::Taxon "Taxon" is itself a @ref genesis::taxonomy::Taxonomy "Taxonomy", as it can contain child taxa of its own.

See the description of these classes for details.

Furthermore, we call a string of the form

    Animalia;Vertebrata;Mammalia;Carnivora

a taxonomic path. Those strings are often used in taxonomic databases, and usually use semicola to separate their parts. We model a taxonomic path in the @ref genesis::taxonomy::Taxopath "Taxopath" class, which basically just consists of a `std::vector< std::string >` to store the individual elements of the path. This class is a helper to read from databases, or write taxonomies in human-readable formats.

In the following, we assume the following headers and namespaces are used:

~~~{.cpp}
#include "genesis/genesis.hpp"
using namespace genesis::taxonomy;
using namespace genesis::utils;
~~~

# Reading and Writing a Taxonomic Database {#tutorials_taxonomy_read_write_database}

Taxonomies can be stored in several formats. We find the SILVA format the most convenient to work with, but also support to read from NCBI.

## SILVA Database

We use the [SILVA Taxonomy](https://www.arb-silva.de/documentation/silva-taxonomy/) as an exemplary database here. Their taxonomy file (here, `tax_slv_ssu_123.1.txt`) starts like this:

    Archaea;	2	domain
    Archaea;Aenigmarchaeota;	11084	phylum		123
    Archaea;Aenigmarchaeota;Aenigmarchaeota Incertae Sedis;	11085	class		123
    Archaea;Aenigmarchaeota;Aenigmarchaeota Incertae Sedis;Unknown Order;	11086	order		123
    Archaea;Aenigmarchaeota;Aenigmarchaeota Incertae Sedis;Unknown Order;Unknown Family;	11087	family		123
    Archaea;Aenigmarchaeota;Aenigmarchaeota Incertae Sedis;Unknown Order;Unknown Family;Candidatus Aenigmarchaeum;	11088	genus		123
    ...

That is, it first contains the taxonomic path, followed by some meta-data. The @ref genesis::taxonomy::Taxon "Taxon" allows to store the rank (`domain`, `phylum`, `class`, `order`, `family`, `genus` etc), which is stored in the third field of the database (at position `2` for zero-based counting).

Let's read that file:

~~~{.cpp}
TaxonomyReader tax_reader;
tax_reader.name_field_position( 0 );
tax_reader.rank_field_position( 2 );
auto taxonomy = tax_reader.read( from_file( "path/to/tax_slv_ssu_123.1.txt" ));
~~~

The position of the name field is initialized to `0` by default, so that line is superfluous and included here only for reference. Then, we set the position (column of the file) where the rank is found. By default, this is set to `-1`, meaning that it is omitted. In short, with the defaults, any database where the first column of a tab-separated table contains the taxonomic names can be read in one line of code.

## NCBI Database

The [NCBI Database](https://www.ncbi.nlm.nih.gov/taxonomy) has a more complex format, with a "node" file that defines the hierarchy of the taxonomy, and a "name" file that defines the corresponding names of the nodes.
This format can be read by read_ncbi_taxonomy(). Typically, the files are named "nodes.dmp" and "names.dmp", respectively.

## Writing

Writing can be done with the @ref genesis::taxonomy::TaxonomyWriter "TaxonomyWriter". It simply takes a @ref genesis::taxonomy::Taxonomy "Taxonomy" and writes its taxonomic paths to a file, potentially with the ranks as well:

~~~{.cpp}
// Write to a file
TaxonomyWriter tax_writer;
tax_writer.write_ranks( true );
tax_writer.write( taxonomy, to_file( "path/to/file.txt" ));
~~~

It is possible to set the delimiter between taxonomic path elements (`;` by default), and other details.
We currently do not support to write to the NCBI format, as it is quite cumbersome to use anyway.

## Further Details

See also @ref genesis::taxonomy::TaxopathParser "TaxopathParser" and @ref genesis::taxonomy::TaxopathGenerator "TaxopathGenerator" for additional tools to parse and generate taxonomic paths. These are used in the background by the @ref genesis::taxonomy::TaxonomyReader "TaxonomyReader" and @ref genesis::taxonomy::TaxonomyWriter "TaxonomyWriter".

# Traversing the Taxonomy {#tutorials_taxonomy_traversal}

Similar to the @ref tutorials_tree_basics_traversing_tree "tree traversal", a @ref genesis::taxonomy::Taxonomy "Taxonomy" can be traversed. As the algorithms that are typically performed on a Taxonomy are less involved than the ones on @ref genesis::tree::Tree "Trees", we use a simpler approach here, that takes a function to be applied to each @ref genesis::taxonomy::Taxon "Taxon" in the Taxonomy:

 * preorder_for_each()
 * postorder_for_each()
 * levelorder_for_each()

Furthermore, for the common use case of preorder traversal, we offer an iterator similar to the one used for Trees:

~~~{.cpp}
// Traverse in preorder and print content
for( auto const& tax_it : preorder( taxonomy )) {
    auto const indent = std::string( ' ', taxon_level( tax_it.taxon() ));
    auto const rank = ( tax_it.taxon().rank().empty() ? "" : " (" + tax_it.taxon().rank() + ")" );
    LOG_INFO << indent << tax_it.taxon().name() << rank;
}
~~~

This prints the taxonomy, indented according to the level in the hierarchy of each Taxon, followed by the rank (`genus` etc in plain text) of each Taxon, if present.

#  Data Model {#tutorials_taxonomy_data_model}

Similar to the @ref tutorials_tree_advanced_data_model "Data Model" that we use for the @ref genesis::tree::Tree "Tree" class, a @ref genesis::taxonomy::Taxon "Taxon" offers a data model to store additional data on top of the name and rank that we already have seen above. Any class that is derived from @ref genesis::taxonomy::BaseTaxonData "BaseTaxonData" can be used.

Currently, we only use this to store additional data in our [PhAT](https://github.com/lczech/gappa/wiki/Subcommand:-phat) method. The method stores information about all the @ref genesis::sequence::Sequence "Sequences" that belong to each Taxon, as well as the entropy of these sequences. Hence, we use this as an example here:

~~~{.cpp}
// Function that adds some data to a given Taxon instance (using a lambda here)
auto add_entropy_data = [&]( Taxon& taxon ){
    taxon.reset_data( EntropyTaxonData::create() );
    // ... do initialization and further processing of the taxon data as needed
};

// Apply the function to the whole Taxonomy
preorder_for_each( taxonomy, add_entropy_data );
~~~

For an in-depth example of how to use this, see [the PhAT implementation](https://github.com/lczech/gappa/blob/master/src/commands/prepare/phat.cpp).
