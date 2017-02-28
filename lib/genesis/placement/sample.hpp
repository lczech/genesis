#ifndef GENESIS_PLACEMENT_SAMPLE_H_
#define GENESIS_PLACEMENT_SAMPLE_H_

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

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "genesis/placement/placement_tree.hpp"
#include "genesis/placement/pquery.hpp"

#include "genesis/utils/core/range.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Sample
// =================================================================================================

/**
 * @brief Manage a set of @link Pquery Pqueries @endlink along with the ::PlacementTree where
 * the PqueryPlacement%s are placed on.
 *
 * This class stores both the tree and the set of pqueries with their placements. It is thus
 * a representation of a whole `jplace` file according to the standard.
 *
 * For more information on the `jplace` format, see the following publication:
 *
 *     Matsen FA, Hoffman NG, Gallagher A, Stamatakis A. 2012.
 *     A Format for Phylogenetic Placement.
 *     PLoS ONE 7(2): e31009. doi:10.1371/journal.pone.0031009
 *     http://journals.plos.org/plosone/article?id=10.1371/journal.pone.0031009
 *
 * This class and other related classes are modeled after this standard.
 */
class Sample
{

public:

    // -------------------------------------------------------------------
    //     Typedefs
    // -------------------------------------------------------------------

    using       iterator_pqueries = std::vector<Pquery>::iterator;
    using const_iterator_pqueries = std::vector<Pquery>::const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Default constructor.
     */
    Sample() = default;

    /**
     * @brief Constructor taking a reference tree.
     *
     * The tree has to have the data types PlacementNodeData and PlacementEdgeData at its nodes and
     * edges, respectively. If not, an exception is thrown.
     */
    Sample( PlacementTree const& tree );

    /**
     * @brief Copy constructor.
     */
    Sample( Sample const& );

    /**
    * @brief Move constructor.
    */
    Sample( Sample&& ) = default;

    /**
     * @brief Copy assignment.
     */
    Sample& operator= ( Sample const& );

    /**
    * @brief Move assignment.
    */
    Sample& operator= ( Sample&& ) = default;

    ~Sample() = default;

    /**
     * @brief Swap the contents of this Sample with the contents of another Sample.
     */
    void swap( Sample& other );

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Clears all data of this object.
     *
     * The @link Pquery Pqueries @endlink, the PlacementTree and the #metadata are deleted.
     */
    void clear();

    // -------------------------------------------------------------------------
    //     Tree Accessors and Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Get the PlacementTree of this Sample.
     */
    PlacementTree& tree();

    /**
     * @brief Get the PlacementTree of this Sample.
     */
    PlacementTree const& tree() const;

    // -------------------------------------------------------------------------
    //     Pquery Accessors and Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Return the number of @link Pquery Pqueries @endlink that are stored in this Sample.
     */
    size_t size() const;

    /**
     * @brief Return whether the tree or the set of @link Pquery Pqueries@endlink is empty.
     */
    bool empty() const;

    /**
     * @brief Create an empty Pquery, add it to the Sample and return it.
     *
     * The returned reference can then be used to add PqueryPlacement%s and PqueryName%s to the Pquery.
     *
     * As this function might reallocate the memory where Pqueries are stored, all iterators
     * and pointers to the Pqueries of this Sample are invalidated.
     */
    Pquery& add();

    /**
     * @brief Create a Pquery as a copy of the provided one, add it to the sample and return it.
     *
     * It is possible to provide a Pquery from a different Sample, as long as the tree topology of both
     * Sample%s is identical, including identical @link PlacementTreeEdge::index() edge indices @endlink
     * and @link PqueryPlacement::edge_num edge_nums@endlink. For that purpose, this function
     * automatically adjusts the internal pointers of the Pquery and its PqueryPlacement%s accordingly.
     * Furthermore, the `proximal_length` is also adjusted so that the relative position on the edge
     * maintained.
     *
     * However, if the trees are incompatible (i.e., have a different topology, indices or edge nums),
     * the PqueryPlacement%s will either point to different edges or the function might throw an
     * exception, in cases where the tree does not have a corresponding edge at all. To further ensure
     * correct behaviour, the function also checks whether the @link PqueryPlacement::edge_num
     * edge_num @endlink is the same for the edge of the original PqueryPlacement and the new one,
     * and throws an std::runtime_error if not.
     *
     * As this function might reallocate the memory where Pqueries are stored, all iterators
     * and pointers to the Pqueries of this Sample are invalidated.
     */
    Pquery& add( Pquery const& other );

    /**
     * @brief Return the Pquery at a certain index.
     */
    Pquery& at( size_t index );

    /**
     * @brief Return the Pquery at a certain index.
     */
    Pquery const& at( size_t index ) const;

    /**
     * @brief Remove the Pquery at a given `index` from the Sample.
     *
     * As this function might reallocate the memory where Pqueries are stored, all iterators
     * and pointer to the Pqueries of this Sample are invalidated.
     */
    void remove( size_t index );

    /**
    * @brief Remove the @link Pquery Pqueries @endlink between the `first_index` (inclusive) and
    *  the `last_index` (exclusive) from the Sample.
    */
    void remove( size_t first_index, size_t last_index );

    /**
     * @brief Remove the Pquery at a given iterator `position` from the Sample.
     */
    void remove( iterator_pqueries position );

    /**
    * @brief Remove the @link Pquery Pqueries @endlink between the `first` (inclusive) and the `last`
    * (exclusive) iterator position from the Sample.
    */
    void remove( iterator_pqueries first, iterator_pqueries last );

    /**
     * @brief Clear all @link Pquery Pqueries @endlink of this Sample.
     *
     * All Pqueries are deleted. However, the PlacementTree and the #metadata are left as they are.
     * Thus this is a useful method for e.g., simulating placements: Take a copy of a given sample,
     * clear its Pqueries, then generate new ones using the Simulator.
     */
    void clear_pqueries();

    // -------------------------------------------------------------------------
    //     Pquery Iterator
    // -------------------------------------------------------------------------

    /**
     * @brief Return an iterator to the beginning of the @link Pquery Pqueries @endlink of this Sample.
     */
    iterator_pqueries       begin();

    /**
     * @brief Return a constiterator to the beginning of the @link Pquery Pqueries @endlink of this
     * Sample.
     */
    const_iterator_pqueries begin() const;

    /**
     * @brief Return an iterator to the end of the @link Pquery Pqueries @endlink of this Sample.
     */
    iterator_pqueries       end();

    /**
     * @brief Return a const iterator to the end of the @link Pquery Pqueries @endlink of this Sample.
     */
    const_iterator_pqueries end() const;

    /**
     * @brief Return a Range iterator to the @link Pquery Pqueries @endlink.
     *
     * This makes iterating Pqueries via a range based for loop easy.
     */
    utils::Range<iterator_pqueries>       pqueries();

    /**
     * @brief Return a const Range iterator to the @link Pquery Pqueries @endlink.
     *
     * This makes iterating Pqueries via a range based for loop easy.
     */
    utils::Range<const_iterator_pqueries> pqueries() const;

    // -------------------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------------------

private:

    std::vector<Pquery> pqueries_;
    PlacementTree       tree_;

public:

    // There is not much to mess up here for a user, so we can simply make this public.
    // Okay, this is ugly. But for now, it's way easier. We never use the metadata anyway...
    std::unordered_map<std::string, std::string> metadata;

};

} // namespace placement
} // namespace genesis

#endif // include guard
