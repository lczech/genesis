#ifndef GENESIS_PLACEMENT_SAMPLE_SET_H_
#define GENESIS_PLACEMENT_SAMPLE_SET_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <string>
#include <vector>

#include "placement/sample.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     SampleSet
// =================================================================================================

/**
 * @brief Store a set of Sample%s with associated names.
 *
 * The elements in this set are stored as a NamedSample. They are stored in the sequence in which
 * they are added to the set, and can be accessed via an index.
 */
class SampleSet
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs
    // -------------------------------------------------------------------------

    /**
     * @brief Store a Sample together with a name for it.
     */
    struct NamedSample
    {
        std::string name;
        Sample      sample;
    };

    typedef std::vector<NamedSample>::iterator       iterator;
    typedef std::vector<NamedSample>::const_iterator const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SampleSet()  = default;
    ~SampleSet() = default;

    SampleSet( SampleSet const& ) = default;
    SampleSet( SampleSet&& )      = default;

    SampleSet& operator= ( SampleSet const& ) = default;
    SampleSet& operator= ( SampleSet&& )      = default;

    void swap( SampleSet& other );

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void add( Sample const& smp );
    void add( Sample const& smp, std::string const& name );

    void remove_at( size_t index );
    void clear();

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    iterator       begin();
    const_iterator begin() const;

    iterator       end();
    const_iterator end() const;

          NamedSample& at ( size_t index );
    const NamedSample& at ( size_t index ) const;

          NamedSample& operator [] ( size_t index );
    const NamedSample& operator [] ( size_t index ) const;

    bool  empty() const;
    size_t size() const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<NamedSample> smps_;

};

} // namespace placement
} // namespace genesis

#endif // include guard
