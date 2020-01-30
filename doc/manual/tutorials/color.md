Colors {#tutorials_colors}
===========

In this tutorial, we explain how color is handled in genesis. We assume the following namespace is given:

~~~{.cpp}
#include "genesis/genesis.hpp"
using namespace genesis::utils;
~~~

Let's start with the @ref genesis::utils::Color "Color" class, which stores one individual color.
It expects three double values for red, green, and blue (or four, with alpha), all of them in the range `[ 0.0, 1.0 ]`. Alternatively, it can create a Color object @ref genesis::utils::Color::from_bytes() "from_bytes()" (i.e., in the 0 to 255 range) or @ref genesis::utils::Color::from_hex() "from_hex()" (i.e., "#448EE4"), if needed.

~~~{.cpp}
Color c1 = Color( 1.0, 0.0, 1.0 );
Color c2 = Color::from_bytes( 255, 0, 255 );
Color c3 = Color::from_hex( "#FF00FF" );
~~~

For most use cases, we however want more than one color: We can use a `std::vector` of Color, as shown later.

# Picking individual colors from lists {#tutorials_colors_lists}

For simply picking individual colors (e.g., for a chart), we offer two lists of named colors:

 * The 140 named web colors, see [here](https://en.wikipedia.org/wiki/Web_colors#X11_color_names).
 * The 954 xkcd survey colors, see [here](https://xkcd.com/color/rgb/) for the list, and see [here](https://blog.xkcd.com/2010/05/03/color-survey-results/) for some background information on this list.

Given their name, @ref genesis::utils::color_from_name() "color_from_name()" retrieves the respective Color entry. Furthermore, @ref genesis::utils::color_palette_web() "color_palette_web()" and @ref genesis::utils::color_palette_xkcd() "color_palette_xkcd()" yield the whole lists.

# Picking sets of colors from palettes {#tutorials_colors_palettes}

For more advanced use cases, one usually wants a palette of colors that are tuned to look good in combination. To this end, we offer three different types of palettes:

 * Sequential color palettes, for increasing ranges of values, e.g., `0` to `100`.
 * Diverging color palettes, for ranges with two directions and a center value, e.g., `-1` to `1`.
 * Qualitative color palettes, that is, distinct colors that are useful for sets of items.

The available color palettes are:

![Color palettes.](utils/color_lists.png)

The above list contains color palettes from sources such as [ColorBrewer](https://rdrr.io/cran/RColorBrewer/man/ColorBrewer.html), and [viridis](https://cran.r-project.org/web/packages/viridis/vignettes/intro-to-viridis.html) that look nice and have favorable properties in terms of their color composition, for example for color blind people. Furthermore, we recommend our own palette `BuPuBk` (the first one) for visualizing trees, as its lightest color (light blue) is still well visible for the thin branches of a tree when printed on paper.

The palettes can be used by their name, e.g., color_list_viridis(), to obtain a vector of Colors. Furthermore, they can be used via functions that take the name of the list either from an enum or as a string, see e.g., sequential_color_list(). The latter is useful in applications where the user can pick a color scheme.

These palettes are simply stored as a `std::vector` of @ref genesis::utils::Color "Color" objects.
For artistic users, it is of course also possible to fill a `vector` with a user defined set of colors.

# Using color palettes to visualize ranges {#tutorials_colors_ranges}

Often, one wants to use color to visualize a range of values, for example using a color gradient. In order to most easily use the above color palettes to this end, we offer tools to normalize and pick them:

 * @ref genesis::utils::ColorMap "ColorMap" for storing the palette and interpolating between its entries.
 * @ref genesis::utils::ColorNormalization "ColorNormalization" and its derived classes to normalize arbitrary ranges.

Let's see how they work.

## ColorMap

A @ref genesis::utils::ColorMap "ColorMap" stores a palette and maps the `[ 0.0, 1.0 ]` interval to the palette by interpolating between colors. This is a simple means to use a color palette (which has a fixed number of entries) to obtain fitting colors for a continuous input:

~~~{.cpp}
ColorMap color_map = ColorMap( color_list_viridis() );
auto some_color = color_map( 0.3 );
~~~

Colors outside of the `[ 0.0, 1.0 ]` interval can either be clipped (set to either `0.0` or `1.0`), or marked in extra colors in order to visualize these overflows. Furthermore, a special @ref genesis::utils::ColorMap::mask_color() "mask_color()" can be set for non-finite values (nan or infinite), in order to visualize invalid input. These functions are useful to warn users that something is wrong with their data, by showing the respective issue in the resulting visualization (for example, by coloring a particular branch of a tree).

## ColorNormalization

As the @ref genesis::utils::ColorMap "ColorMap" expects its input to be in the unit interval, we offer @ref genesis::utils::ColorNormalization "ColorNormalization" and its derived classes to bring arbitrary input into that interval:

 * @ref genesis::utils::ColorNormalizationLinear "ColorNormalizationLinear" to linearly map any `[ min, max ]` interval into the unit interval.
 * @ref genesis::utils::ColorNormalizationDiverging "ColorNormalizationDiverging" for a diverging intervals around a midpoint, such as `[ -1.0, 1.0 ]` with `0.0` as the midpoint.
 * @ref genesis::utils::ColorNormalizationLogarithmic "ColorNormalizationLogarithmic" for a logarithmic mapping of a positive range, most useful for ranges that span many orders of magnitude.
 * @ref genesis::utils::ColorNormalizationBoundary "ColorNormalizationBoundary" for mapping fixed intervals to a set of colors.

See the respective classes for details on their usage.

A simple example with some values in a large range that we want to map logarithmically:

~~~{.cpp}
auto values = std::vector<double>{ 2.3, 43.5, 195.8, 566.0, 846.2 };
auto color_norm = ColorNormalizationLogarithmic( 1.0, 1000.0);
auto colors = color_map( color_norm, values );
~~~

This instance of @ref genesis::utils::ColorNormalizationLogarithmic "ColorNormalizationLogarithmic" maps `1.0` to the first color of the palette (which is at the `0.0` position of the @ref genesis::utils::ColorMap "ColorMap"), and `1000.0` to its last (at the `1.0` position of the @ref genesis::utils::ColorMap "ColorMap"). All values in between are mapped in a logarithmic manner, i.e., `10.0` to the first third (`0.33`), `100.0` to the second third (`0.66`), and so forth. The result of the above snippted is a `std::vector` of Color%s.

## Additional Information

We use this separation into @ref genesis::utils::ColorMap "ColorMap" and @ref genesis::utils::ColorNormalization "ColorNormalization" to keep each of these classes simple and easy to understand on its own. This is inspired by [Matplotlib](https://matplotlib.org):

 * See https://matplotlib.org/3.1.1/tutorials/colors/colormapnorms.html for additional information on normalization for intervals.
 * See https://matplotlib.org/3.1.1/tutorials/colors/colormaps.html for hints about how to chose fitting color palettes.
