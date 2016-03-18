#ifndef GENESIS_PLACEMENT_SAMPLE_SET_H_
#define GENESIS_PLACEMENT_SAMPLE_SET_H_

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

class SampleSet
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs
    // -------------------------------------------------------------------------

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

    void add( std::string const& name, Sample const& smp);

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
