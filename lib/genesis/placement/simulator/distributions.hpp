#ifndef GENESIS_PLACEMENT_SIMULATOR_DISTRIBUTIONS_H_
#define GENESIS_PLACEMENT_SIMULATOR_DISTRIBUTIONS_H_

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

#include "genesis/placement/sample.hpp"
#include "genesis/utils/core/options.hpp"

#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Simulator Edge Distribution
// =================================================================================================

class SimulatorEdgeDistribution
{
public:

    // -------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------

    SimulatorEdgeDistribution()  = default;
    ~SimulatorEdgeDistribution() = default;

    SimulatorEdgeDistribution( SimulatorEdgeDistribution const& ) = default;
    SimulatorEdgeDistribution( SimulatorEdgeDistribution&& )      = default;

    SimulatorEdgeDistribution& operator= ( SimulatorEdgeDistribution const& ) = default;
    SimulatorEdgeDistribution& operator= ( SimulatorEdgeDistribution&& )      = default;

    // -------------------------------------------------
    //     Generate Random Edges
    // -------------------------------------------------

    /**
     * @brief Prepare the distribution for usage. Needs to be called before generate().
     */
    void prepare( Sample const& sample )
    {
        // If nothing was set, initialize to uniform distrib.
        if( edge_weights.size() == 0 ) {
            edge_weights = std::vector<double>( sample.tree().edge_count(), 1.0 );
        }

        // Check size.
        if( edge_weights.size() != sample.tree().edge_count() ) {
            throw std::runtime_error(
                "Incorrect number of edge weights for SimulatorEdgeDistribution."
            );
        }

        // Prepare the actual distrib.
        distrib_ = std::discrete_distribution<size_t>( edge_weights.begin(), edge_weights.end() );
    }

    /**
     * @brief Return a randomly chosen edge index, according to the distribution.
     */
    size_t generate()
    {
        return distrib_( utils::Options::get().random_engine() );
    }

    // -------------------------------------------------
    //     Data Members
    // -------------------------------------------------

public:

    std::vector<double> edge_weights;

private:

    std::discrete_distribution<size_t> distrib_;
};

// =================================================================================================
//     Placement Simulator Extra Placement Distribution
// =================================================================================================

/**
 * @brief Generate a certain number of additional PqueryPlacement%s around a given
 * ::PlacementTreeEdge.
 *
 * This class models both the distribution of the number of generated placements and their
 * distribution around the given edge. It is easier to model it this way (both distributions in one
 * class), because they have a dependency: Once the number of placements is drawn, they have to be
 * distributed along the edges of the tree without repetition according to a distribution of
 * distances (path lengths) from the central edge. Doing those two steps in one class is easier.
 */
class SimulatorExtraPlacementDistribution
{
public:

    // -----------------------------------------------------
    //     Typedefs and Structs
    // -----------------------------------------------------

    struct EdgeProximity
    {
        using EdgeCandidates = std::vector<size_t>;
        std::vector<EdgeCandidates> candidates_per_level;
        size_t total_candidates = 0;
    };

    // -----------------------------------------------------
    //     Constructor and Rule of Five
    // -----------------------------------------------------

    SimulatorExtraPlacementDistribution()  = default;
    ~SimulatorExtraPlacementDistribution() = default;

    SimulatorExtraPlacementDistribution( SimulatorExtraPlacementDistribution const& ) = default;
    SimulatorExtraPlacementDistribution( SimulatorExtraPlacementDistribution&& )      = default;

    SimulatorExtraPlacementDistribution& operator= ( SimulatorExtraPlacementDistribution const& ) = default;
    SimulatorExtraPlacementDistribution& operator= ( SimulatorExtraPlacementDistribution&& )      = default;

    // -----------------------------------------------------
    //     Generate Random Positions
    // -----------------------------------------------------

    void prepare( Sample const& sample );
    std::vector<size_t> generate( PlacementTreeEdge const& edge );

    std::string         dump_edge_proximities() const;
    std::vector<size_t> edge_proximity_maxima() const;

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

public:

    std::vector<double> placement_number_weights;
    std::vector<double> placement_path_length_weights;

private:

    std::discrete_distribution<size_t> placement_number_distrib_;
    std::discrete_distribution<size_t> placement_path_length_distrib_;

    std::vector<EdgeProximity> edge_proximities_;
};

// =================================================================================================
//     Placement Simulator Like Weight Ratio Distribution
// =================================================================================================

class SimulatorLikeWeightRatioDistribution
{
public:

    // -------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------

    SimulatorLikeWeightRatioDistribution()  = default;
    ~SimulatorLikeWeightRatioDistribution() = default;

    SimulatorLikeWeightRatioDistribution( SimulatorLikeWeightRatioDistribution const& ) = default;
    SimulatorLikeWeightRatioDistribution( SimulatorLikeWeightRatioDistribution&& )      = default;

    SimulatorLikeWeightRatioDistribution& operator= ( SimulatorLikeWeightRatioDistribution const& ) = default;
    SimulatorLikeWeightRatioDistribution& operator= ( SimulatorLikeWeightRatioDistribution&& )      = default;

    // -----------------------------------------------------
    //     Generate Random Ratios
    // -----------------------------------------------------

    /**
     * @brief Prepare the distribution for usage. Needs to be called before generate().
     */
    void prepare( Sample const& sample );

    /**
     * @brief Return a randomly chosen like weight ratio.
     */
    double generate()
    {
        return distrib_( utils::Options::get().random_engine() );
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

public:

    std::vector<double> intervals;
    std::vector<double> weights;

private:

    std::piecewise_linear_distribution<double> distrib_;
};

// =================================================================================================
//     Placement Simulator Pendant Length Distribution
// =================================================================================================

class SimulatorPendantLengthDistribution
{
public:

    // -------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------

    SimulatorPendantLengthDistribution()  = default;
    ~SimulatorPendantLengthDistribution() = default;

    SimulatorPendantLengthDistribution( SimulatorPendantLengthDistribution const& ) = default;
    SimulatorPendantLengthDistribution( SimulatorPendantLengthDistribution&& )      = default;

    SimulatorPendantLengthDistribution& operator= ( SimulatorPendantLengthDistribution const& ) = default;
    SimulatorPendantLengthDistribution& operator= ( SimulatorPendantLengthDistribution&& )      = default;

    // -----------------------------------------------------
    //     Generate Random Length
    // -----------------------------------------------------

    /**
     * @brief Prepare the distribution for usage. Needs to be called before generate().
     */
    void prepare( Sample const& sample )
    {
        (void) sample;
        distrib_ = std::uniform_real_distribution<double>(min, max);
    }

    /**
     * @brief Return a randomly chosen position on an edge.
     */
    double generate( PlacementTreeEdge const& edge )
    {
        // We don't use the edge in the default distribution.
        (void) edge;

        return distrib_( utils::Options::get().random_engine() );
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

public:

    double min = 0.0;
    double max = 1.0;

private:

    std::uniform_real_distribution<double> distrib_;
};

// =================================================================================================
//     Placement Simulator Proximal Length Distribution
// =================================================================================================

class SimulatorProximalLengthDistribution
{
public:

    // -------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------

    SimulatorProximalLengthDistribution()  = default;
    ~SimulatorProximalLengthDistribution() = default;

    SimulatorProximalLengthDistribution( SimulatorProximalLengthDistribution const& ) = default;
    SimulatorProximalLengthDistribution( SimulatorProximalLengthDistribution&& )      = default;

    SimulatorProximalLengthDistribution& operator= ( SimulatorProximalLengthDistribution const& ) = default;
    SimulatorProximalLengthDistribution& operator= ( SimulatorProximalLengthDistribution&& )      = default;

    // -----------------------------------------------------
    //     Generate Random Positions
    // -----------------------------------------------------

    /**
     * @brief Prepare the distribution for usage. Needs to be called before generate().
     */
    void prepare( Sample const& sample )
    {
        (void) sample;
        distrib_ = std::uniform_real_distribution<double>( 0.0, 1.0 );
    }

    /**
     * @brief Return a randomly chosen position on an edge.
     */
    double generate( PlacementTreeEdge const& edge )
    {
        // We do a multiplication with the branch length here, because this allows for a single
        // distribution instance instead of one per different length.
        auto branch_length = edge.data<PlacementEdgeData>().branch_length;
        return distrib_( utils::Options::get().random_engine() ) * branch_length;
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    std::uniform_real_distribution<double> distrib_;
};

} // namespace placement
} // namespace genesis

#endif // include guard
