#ifndef GENESIS_PLACEMENT_SAMPLE_SET_H_
#define GENESIS_PLACEMENT_SAMPLE_SET_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/placement/sample.hpp"

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

    /**
     * @brief Add a Sample to the SampleSet.
     *
     * The Sample is copied. The name is set to empty string.
     */
    void add( Sample const& smp );

    /**
     * @brief Add a Sample with a name to the SampleSet.
     *
     * The Sample is copied.
     */
    void add( Sample const& smp, std::string const& name );

    /**
     * @brief Add a Sample to the SampleSet.
     *
     * The Sample is moved. The name is set to empty string.
     */
    void add( Sample&& smp );

    /**
     * @brief Add a Sample with a name to the SampleSet.
     *
     * The Sample is moved.
     */
    void add( Sample&& smp, std::string const& name );

    /**
     * @brief Remove the Sample at a certain index position.
     *
     * As this function moves Sample%s in the container around, all iterators and pointers to
     * the elements of this SampleSet are considered to be invalidated.
     */
    void remove_at( size_t index );

    /**
     * @brief Delete all Sample%s in this SampleSet.
     */
    void clear();

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    iterator       begin();
    const_iterator begin() const;

    iterator       end();
    const_iterator end() const;

    /**
     * @brief Get the NamedSample at a certain index position.
     */
    NamedSample& at ( size_t index );

    /**
     * @brief Get the NamedSample at a certain index position.
     */
    const NamedSample& at ( size_t index ) const;


    /**
     * @brief Get the NamedSample at a certain index position.
     */
    NamedSample& operator [] ( size_t index );

    /**
     * @brief Get the NamedSample at a certain index position.
     */
    const NamedSample& operator [] ( size_t index ) const;

    /**
     * @brief Return whether the SampleSet is empty.
     */
    bool  empty() const;

    /**
     * @brief Return the size of the SampleSet, i.e., the number of Sample%s.
     */
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
