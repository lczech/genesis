Compare Jplace Files {#demos_compare_jplace_files}
===========

@section Summary

Takes two `jplace` files with the same reference tree and compares the placement of each
@ref genesis::placement::Pquery "Pquery". It outputs tables that show which and how Pqueries
differ in their placement on the tree.

This demo is located at

    genesis/doc/demos/compare_jplace_files.cpp

@section Details

This program is useful to compare two `jplace` files that were computed with different settings
or even different programs (EPA or pplacer). It is not meant for evaluating differences in the
microbial communities that are represented by the input. Instead, it is meant for files that
share @ref genesis::placement::Pquery "Pqueries" (identified by their names), and gives information
about differences between the Placements in those Pqueries.

The program takes two input `jplace` file paths as input. It compares the
@ref genesis::placement::Pquery "Pqueries" and their Placements and prints two tables:

  1. An overview table that lists all Pqueries of the two files that have a PqueryName in common.
     This table indicates whether the top Placement (the one with the highest `like_weight_ratio`)
     of both Pqueries is the same (i.e., is located at the same branch); it furthermore indicates
     whether all Placements (sorted by their `like_weight_ratio`) are the same, that is, if they
     are located on the same branches. Lastly, the difference in log-likelhood and the
     @link earth_movers_distance() Earth Movers Distance@endlink between the Pqueries is printed.
  2. A detail table that lists all Placements of the Pqueries that were marked invalid in the
     overview table - that is, if either the top rank or any other placement was not equally
     placed in a Pquery. This table lists the Placements for such Pqueries, sorted by their
     `like_weight_ratio`, and shows on which branches (edge_num) they are placed in the two
     Pqueries. If the Placements are on the same branch, they are considered correct.

The program expects that the reference Tree%s of the input are topologically identical. In order
to compensate for differences in branch lengths, both Trees are normalized in the beginning, so
that their length (sum of branch lengths) is 1.0. This also means that the Earth Movers Distance
yields comparable values in the range `[ 0.0, 1.0 ]`.

@section  Example

Example files to test the demo are located at

    genesis/doc/demos/compare_jplace_files/

This directory contains two `jplace` files `jplace_a.jplace` and `jplace_b.jplace`.

After compiling the demo program (using `make update`), you can run the example like this

    cd genesis/doc/demos/compare_jplace_files/
    ../../../bin/compare_jplace_files jplace_a.jplace jplace_b.jplace

The expected output are two tables with information about the Pqueries in those two files.
