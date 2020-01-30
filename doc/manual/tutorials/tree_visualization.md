Tree Visualization {#tutorials_tree_visualization}
===========

We provide functionality for writing a @ref genesis::tree::Tree "Tree" to different file formats that allow visualization via external tree viewers:

 * @ref genesis::tree::NewickWriter "NewickWriter", for general `newick` tree files that can be read and visualized by most other tools.
 * @ref genesis::tree::PhyloxmlWriter "PhyloxmlWriter", for `phyloxml` tree files, best suited for the [Archaeopteryx](https://sites.google.com/site/cmzmasek/home/software/archaeopteryx) tree viewer.
 * @ref genesis::utils::NexusWriter "NexusWriter", for `nexus` files, which internally also use `newick` to store trees; best suited for the [FigTree](http://tree.bio.ed.ac.uk/software/figtree/) tree viewer.
 * Additionally, we offer an easy way to draw Trees directly to `svg` files, yielding publication quality vector graphics. This is the only type of visualization that directly supports to also draw a legend for  edge colors.

<b>Remarks:</b> See [Czech <i>et al.</i> 2017](https://doi.org/10.1093/molbev/msx055) for a comprehensive review on tree viewers.

In the following, we assume that the following namespaces are available:

~~~{.cpp}
#include "genesis/genesis.hpp"
using namespace genesis::tree;
using namespace genesis::utils;
~~~

# Edge colors {#tutorials_tree_visualization_edge_colors}

Many of the tree visualization that we for example explain in the @ref tutorials_placement_analyses "Placement Analyses" tutorial or that are part of our analysis program [gappa](https://github.com/lczech/gappa) visualize trees with individually colored edges. Here, we explain how this is done in genesis.

Firstly, get a tree and see how many edges it has:

~~~{.cpp}
Tree tree = CommonTreeNewickReader().read( from_file( "path/to/tree.newick" ));
LOG_INFO << "Tree has " << tree.edge_count() << " edges.";
~~~

Now, create a vector of @ref genesis::utils::Color "Color" objects for each edge of the tree:

~~~{.cpp}
auto edge_colors = std::vector<Color>( tree.edge_count(), Color( 1.0, 0.0, 0.0 ) );
~~~

See the @ref tutorials_colors "Colors" tutorial on how color is handled in genesis.

In the above snippet, we made all edges red. The @ref genesis::tree::TreeEdge::index() "TreeEdge::index()" of each edge corresponds to one position in the vector. See the @ref tutorials_tree_advanced_indices "Indices of Links, Nodes, and Edges" section for details on indices. One can change edge colors in the vector according to these indices. For example, make all edges leading to tip nodes blue:

~~~{.cpp}
for( auto const& edge : tree.edges() ) {
    if( is_leaf( edge )) {
        edge_colors[ edge.index() ] = Color( 0.0, 0.0, 1.0 );
    }
}
~~~

In an actual algorithm for analyzing some data, the colors can then be set as needed, for example by using a color gradient. Color gradients can be achieved using @ref genesis::utils::ColorMap "ColorMap" and @ref genesis::utils::ColorNormalization "ColorNormalization", see the @ref tutorials_colors "Colors" tutorial for details.

Now that we have a `std::vector` of @ref genesis::utils::Color "Colors" for each edge of the tree, we can store the result in various formats.

# Writing to tree format files {#tutorials_tree_visualization_files}

As discussed in the sections @ref tutorials_tree_basics_reading_writing "Reading and Writing" and @ref tutorials_tree_advanced_reading_writing_revisited "Reading and Writing Revisited", one can use @ref genesis::tree::NewickWriter "NewickWriter" and @ref genesis::tree::PhyloxmlWriter "PhyloxmlWriter" to write Trees to `newick` and `phyloxml` files.

## Write to newick

The `newick` format does not support colors. However, for visualizing just the tree topology with taxon names and branch lengths, it is still useful. If just these defaults are needed, we offer a simple function that wraps the @ref genesis::tree::CommonTreeNewickWriter "CommonTreeNewickWriter":

~~~{.cpp}
write_tree_to_newick_file( tree, "path/to/new_tree.newick" );
~~~

This can be visualized with most tree viewers.

## Write to phyloxml

The `phyloxml` format offers some more annotations, and allows for color annotations for its branches. This is for example used by the [guppy tool](http://matsen.github.io/pplacer/generated_rst/guppy.html) that is part of the [pplacer](http://matsen.github.io/pplacer/generated_rst/pplacer.html) suite of programs of phylogenetic placement. See [their notes on phyloxml](http://matsen.github.io/pplacer/generated_rst/guppy.html#phyloxml-viewing-notes) for some details. See [phyloxml.org](http://www.phyloxml.org/") for details on the format.

To write a tree with colored edges, we can again use a simple wrapper function:

~~~{.cpp}
write_color_tree_to_phyloxml_file( tree, edge_colors, "path/to/new_tree.phyloxml" );
~~~

The resulting `phyloxml` file is best viewed with the [Archaeopteryx](https://sites.google.com/site/cmzmasek/home/software/archaeopteryx) tree viewer. There is also write_tree_to_phyloxml_file() as a wrapper without colors, as well as a version that takes a list of per-edge values and a @ref genesis::utils::ColorMap "ColorMap" and @ref genesis::utils::ColorNormalization "ColorNormalization", and performs the transformation of values into colors as well. See their implementation to get the gist of how this works behind the scenes.

## Write to nexus

The `nexus` format is a container format that also allows to store trees. See for example [this page](http://wiki.christophchamp.com/index.php?title=NEXUS_file_format) for some details on the format.

It works similar to the `phyloxml` wrappers:

~~~{.cpp}
write_color_tree_to_nexus_file( tree, edge_colors, "path/to/new_tree.nexus" );
~~~

The resulting file is best viewed with the [FigTree](http://tree.bio.ed.ac.uk/software/figtree/) tree viewer. Again, there is write_tree_to_nexus_file() for a version without colors, and one that also transforms per-edge values to colors first. See the implementation of these wrapper functions to understand how to build a @ref genesis::utils::NexusDocument "NexusDocument" containing a tree from scratch.

# Drawing to SVG {#tutorials_tree_visualization_svg}

The above functions yield files that can be viewed with external tree viewers. This is cumbersome for automatization and for testing purposes. Furthermore, tree viewers do not support advanced features such as putting a legend for the color scale next to the tree. Hence, we offer to draw trees to `svg` files directly.

This means that all layout parameters have to be set in the code. Most importantly, these are:

 * The @ref genesis::tree::LayoutShape "LayoutShape" of the tree: Is the tree drawn circularly or rectangularly?
 * The @ref genesis::tree::LayoutType "LayoutType" of the tree: Is the tree drawn as a phylogram (with branch lengths), or as a cladogram (without branch lengths - all leaf nodes end at the same distance)?
 * Do we want to ladderize() the tree, i.e., sort the subtrees by number of leaves?
 * What kind of @ref genesis::utils::SvgStroke "SvgStroke" do we want for the edges? Line widths, dashed lines, and line colors.

## Simple wrapper functions

These properties are summarized in @ref genesis::tree::LayoutParameters "LayoutParameters", which can be used again with simple wrapper functions to get `svg` drawings of a tree. We here use the `edge_colors` from above to show edges that lead to leaf nodes in blue:

~~~{.cpp}
// Prepare the layout parameters
LayoutParameters params;
params.type = LayoutType::kPhylogram;
params.shape = LayoutShape::kRectangular;
params.stroke.width = 3.0;

// Use them to write an svg file
write_color_tree_to_svg_file( tree, params, edge_colors, "path/to/new_tree.svg" );
~~~

This yields the following figure:

![Tree with edges leading to leaf nodes marked in blue.](tree/red_blue_tree.png)

Again, there also is a version write_tree_to_svg_file() that does not use colors, but simply write a tree with black branches.

## Adding a color legend

Writing to `svg` is the only way to directly draw the legend for the edge colors into the document. This allows to interpret which edge color corresponds to which value. Let's visualize the branch lengths of the tree:

~~~{.cpp}
// Collect the branch lengths of the tree
auto edge_values = std::vector<double>( tree.edge_count() );
for( size_t i = 0; i < tree.edge_count(); ++i ) {
    edge_values[i] = tree.edge_at(i).data<CommonEdgeData>().branch_length;
}

// Make a color map and a normalization that is scaled to the largest value in the data,
// while keeping the min at 0
auto color_map = ColorMap( color_list_viridis() );
auto color_norm = ColorNormalizationLinear();
color_norm.autoscale_max( edge_values );

// Use this to create an svg drawing including a color legend
write_color_tree_to_svg_file( tree, params, edge_values, color_map, color_norm, "branch_length_tree.svg" );
~~~

See the @ref tutorials_colors "Colors" tutorial on how color is handled in genesis, and on the @ref genesis::utils::ColorMap "ColorMap" and @ref genesis::utils::ColorNormalization "ColorNormalization" classes in particular.

The above snippet yields a figure similar to the following one, where longer branches have a darker color:

![Tree with branch lengths visualized by colors.](tree/branch_length_tree.png)

The figure was fine tuned afterwards, because programmatically setting all font sizes, line widths, spaces between figure elements, etc, to nice values is tricky. We however did not remove or add any elements to the drawing: For example, all parts of the legend, including the labeling with fitting intervals (`0` to `0.3`), are automatic. We recommend [Inkscape](https://inkscape.org/) for fine tuning of the resulting `svg` files.

## Beyond the wrapper functions

The above functions are all wrappers for simplicity. They cover the most common use cases, and in particular cover per-edge colors. They however do not allow for other types of individual edge styles (dashed, line widths, etc). If this is required, one can use the implementation of the wrappers as a starting point to create their own visualizations.

The @ref genesis::tree::LayoutBase "LayoutBase" class is a good starting point for more specialized visualizations. It allows:

 * Individual strokes for the two parts of an edge (the one that spreads away from the parent, and the one that symbolizes branch lengths). These strokes can have individual colors, widths, dashes, etc.
 * Aligning the taxon labels to each other, as well as an additional stroke towards these labels (typically, a gray dashed line or something similar can be used here).
 * Individual per-edge and per-node shapes, for example to mark leaf nodes with colored dots that symbolize additional information about the taxa of the tree.

A final set of functions that we want to mention are the heat_tree() functions, which allow to draw a tree to `svg`, with a heat matrix next to it for each of its taxa, which can for example show per-sample data for each of the taxa of the tree. This is commonly used in metagenomic and environmental studies.
