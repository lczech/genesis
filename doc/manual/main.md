The documentation of genesis has two main parts:
The **User Manual** and the **API Reference**.

**Important remark:** genesis is under active development. The manual is not yet
exhaustive, the API might change with subsequent releases, there might be functions missing that
you consider essential, and of course, there might be bugs.
Please [report all issues on our GitHub page](https://github.com/lczech/genesis/issues).
Your feedback is highly appreciated!

# User Manual

The manual contains high level explanations, tutorials and code examples for the
functionality of genesis. It is intended for end users who want to use existing scripts and
applications, and for getting started with genesis.

 <!-- *  @subpage intro -->
 *  @subpage setup
 *  @subpage tutorials
 *  @subpage demos

Furthermore, see the @subpage supplement for additional information.

# API Reference

The documentation of the application programming interface (API) is intended for users who want
to use genesis as a library or toolkit for their own applications.
It provides detailed references for the symbols defined in genesis:

 *  [Namespaces](namespaces.html)
 *  [Classes](annotated.html)
 *  [Source Files](files.html)

<!--
As the API Reference is generated from the doc-blocks in the C++ source code, it is mainly
oriented towards the C++ interface. It is however also suitable for the Python interface.
-->

# Support

For bug reports and feature requests in genesis, please
[open an issue on our GitHub page](https://github.com/lczech/genesis/issues).

For user support on phylogenetic placement topics, please see our
[Phylogenetic Placement Google Group](https://groups.google.com/forum/#!forum/phylogenetic-placement).
It is intended for discussions about phylogenetic placement,
and for user support for our software tools such as [EPA-ng](https://github.com/Pbdas/epa-ng)
and [Gappa](https://github.com/lczech/gappa).

# Citation

When using Genesis, please cite

> Genesis and Gappa: processing, analyzing and visualizing phylogenetic (placement) data.<br />
> Lucas Czech, Pierre Barbera, and Alexandros Stamatakis.<br />
> Bioinformatics, 2020. https://doi.org/10.1093/bioinformatics/btaa070<br />

Also, see [Gappa](https://github.com/lczech/gappa) for our command line tool to run your own
analyses of phylogenetic placement data.

# Acknowledgements
<!-- # Acknowledgements {#main_acknowledgements} -->

This work is financially supported:

  * 2014-2019: [Klaus Tschira Foundation](http://www.klaus-tschira-stiftung.de/) and
    [HITS gGmbH - Heidelberg Institute for Theoretical Studies](http://www.h-its.org)
    in Heiderberg, Germany.
  * 2020-2021: [Carnegie Institution For Science](https://carnegiescience.edu/) in Stanford, USA.

See @ref supplement_acknowledgements for the full list of acknowledgements, particularly for
dependencies.
See the webpages of the [Exelixis Lab](http://exelixis-lab.org/) and the
[Moi Lab](http://moisesexpositoalonso.org/) for our other projects.

# License

Genesis - A toolkit for working with phylogenetic data.<br />
Copyright (C) 2014-2021 Lucas Czech

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Contact:<br />

2014-2019:<br />
Lucas Czech, <a href="mailto:lucas.czech@h-its.org">lucas.czech@h-its.org</a><br />
Exelixis Lab, Heidelberg Institute for Theoretical Studies<br />
Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany<br />

2020-2021:<br />
Lucas Czech, <a href="mailto:lczech@carnegiescience.edu">lczech@carnegiescience.edu</a><br />
Department of Plant Biology, Carnegie Institution For Science<br />
260 Panama Street, Stanford, CA 94305, USA
