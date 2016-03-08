#ifndef GENESIS_PLACEMENT_PQUERY_NAME_H_
#define GENESIS_PLACEMENT_PQUERY_NAME_H_

/**
 * @brief Header of PqueryName class.
 *
 * @file
 * @ingroup placement
 */

#include <string>

namespace genesis {
namespace placement {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Pquery;

// =================================================================================================
//     Pquery Name
// =================================================================================================

/**
 *
 */
class PqueryName
{
public:
// protected:

    friend Pquery;

    /**
     * @brief Default constructor.
     */
    PqueryName(std::string name = "", double multiplicity = 0.0) :
        name(name),
        multiplicity(multiplicity),
        pquery(nullptr)
    {}

    /**
     * @brief Copy constructor.
     */
    PqueryName(const PqueryName& other) :
        name(other.name),
        multiplicity(other.multiplicity),
        pquery(nullptr)
    {}

// public:

    std::string name;
    double      multiplicity;

    Pquery* pquery;
};

} // namespace placement
} // namespace genesis

#endif // include guard
