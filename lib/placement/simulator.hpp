#ifndef GENESIS_PLACEMENT_SIMULATOR_H_
#define GENESIS_PLACEMENT_SIMULATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/sample.hpp"

#include <random>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Simulator
// =================================================================================================

/**
 * @brief Simulates Placements on a Tree.
 */
class PlacementSimulator
{
public:

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    PlacementSimulator(Sample& placements) : placements_(placements) {}

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    virtual void generate (size_t n) = 0;

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------
protected:

    Sample& placements_;
};

// =================================================================================================
//     Placement Simulator Two Step
// =================================================================================================

/**
 * @brief
 */
class PlacementSimulatorTwostep : public PlacementSimulator
{
public:

    // -----------------------------------------------------
    //     From Base Class
    // -----------------------------------------------------

    PlacementSimulatorTwostep(Sample& placements) :
        PlacementSimulator(placements),
        edge_distribution_(placements)
    {};
    void generate (size_t n) override;

    // =========================================================================
    //     Edge Distribution
    // =========================================================================

    class EdgeDistribution
    {
        // -------------------------------------------------
        //     Constructor
        // -------------------------------------------------

        friend PlacementSimulatorTwostep;

        // TODO this ctor is public because otherwise the py binding does not work. this is ugly.
    public:
        EdgeDistribution(Sample& placements) : placements_(placements) {
            set_uniform_weights();
        }

        // -------------------------------------------------
        //     Set Weights
        // -------------------------------------------------

    public:
        void set_uniform_weights ();
        void set_depths_distributed_weights (const std::vector<int>& depth_weights);
        bool transfer_weights (const Sample& from_map);

        void set_random_weights();
        void set_random_subtree_weights();

        // -------------------------------------------------
        //     Generate Random Edges
        // -------------------------------------------------

    protected:
        void   prepare();
        size_t generate();

        // -------------------------------------------------
        //     Data Members
        // -------------------------------------------------

    public:
        std::vector<double> weights;

    protected:
        std::discrete_distribution<size_t> distrib_;
        Sample& placements_;
    };

    // =========================================================================
    //     Proximal Length Distribution
    // =========================================================================

    class ProximalLengthDistribution
    {
        // -------------------------------------------------
        //     Constructor
        // -------------------------------------------------

        friend PlacementSimulatorTwostep;

        // -----------------------------------------------------
        //     Generate Random Positions
        // -----------------------------------------------------

    protected:
        void   prepare();
        double generate(typename PlacementTree::EdgeType* edge);

    protected:
        std::uniform_real_distribution<double> distrib_;
    };

    // =========================================================================
    //     Pendant Length Distribution
    // =========================================================================

    class PendantLengthDistribution
    {
        // -------------------------------------------------
        //     Constructor
        // -------------------------------------------------

        friend PlacementSimulatorTwostep;

        // -----------------------------------------------------
        //     Generate Random Length
        // -----------------------------------------------------

    public:
        double min = 0.0;
        double max = 1.0;

    protected:
        void   prepare();
        double generate(typename PlacementTree::EdgeType* edge);

    protected:
        std::uniform_real_distribution<double> distrib_;
    };

    // =========================================================================
    //     Main Class
    // =========================================================================

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

    inline EdgeDistribution& edge_distribution()
    {
        return edge_distribution_;
    }

    inline ProximalLengthDistribution& proximal_length_distribution()
    {
        return proximal_length_distribution_;
    }

    inline PendantLengthDistribution& pendant_length_distribution()
    {
        return pendant_length_distribution_;
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

protected:
    EdgeDistribution           edge_distribution_;
    ProximalLengthDistribution proximal_length_distribution_;
    PendantLengthDistribution  pendant_length_distribution_;
};

// =================================================================================================
//     Placement Simulator Subtree
// =================================================================================================

/**
 * @brief
 */
class PlacementSimulatorSubtree : public PlacementSimulator
{
public:

    PlacementSimulatorSubtree(Sample& placements) : PlacementSimulator(placements) {}

    void generate (size_t n) override;
};

} // namespace placement
} // namespace genesis

#endif // include guard
