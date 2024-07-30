#ifndef GENESIS_POPULATION_GENOME_WINDOW_WINDOW_VIEW_H_
#define GENESIS_POPULATION_GENOME_WINDOW_WINDOW_VIEW_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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

    Contact:
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/window/base_window.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/population/window/window_view.hpp"

#include <cassert>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

namespace genesis {
namespace population {

// =================================================================================================
//     Whole Genome Window View
// =================================================================================================

/**
 * @brief Window view over a whole genome.
 *
 * This derivation of WindowView adds a special case for when we are streaming over a whole genome.
 * In that case, we cannot use our usual notation of first and last positions on a chromosome,
 * as we are using the whole genome instead. As the notation is however useful in all other cases,
 * we want to keep it. A cleaner approach from a software design perspective would be to have base
 * classes for both cases, but well, this one works well enough for that one case for now.
 */
template<class D>
class GenomeWindowView final : public WindowView<D>
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using Data = D;
    using self_type = GenomeWindowView<Data>;

    using iterator_category      = std::input_iterator_tag;

    using value_type             = Data;
    using reference              = value_type&;
    using const_reference        = value_type const&;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    GenomeWindowView() = default;
    virtual ~GenomeWindowView() override = default;

    GenomeWindowView( GenomeWindowView const& ) = default;
    GenomeWindowView( GenomeWindowView&& )      = default;

    GenomeWindowView& operator= ( GenomeWindowView const& ) = default;
    GenomeWindowView& operator= ( GenomeWindowView&& )      = default;

    // -------------------------------------------------------------------------
    //     Data Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Get the list of all chromosomes along the genome, with their length.
     *
     * This is based on the chromsomoes and their lengths as encountered in the input data,
     * or on the sequence dictionary if provided to the GenomeWindowStream.
     */
    std::unordered_map<std::string, size_t> const& chromosomes() const
    {
        return chromosomes_;
    }

    /**
     * @brief Get the list of all chromosomes along the genome, with their length.
     */
    std::unordered_map<std::string, size_t>& chromosomes()
    {
        return chromosomes_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::unordered_map<std::string, size_t> chromosomes_;

};

} // namespace population
} // namespace genesis

#endif // include guard
