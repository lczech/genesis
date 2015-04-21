#ifndef GENESIS_MAIN_OPTIONS_H_
#define GENESIS_MAIN_OPTIONS_H_

/**
 * @brief
 *
 * @file
 * @ingroup main
 */

#include <string>
#include <vector>

namespace genesis {

// =============================================================================
//     Options
// =============================================================================

/**
 * @brief Simple Options class for application-wide configuration and settings.
 */
class Options
{
public:

    // -----------------------------------------------------
    //     Initialization
    // -----------------------------------------------------

    static void Init (int argc, char* argv[]);

    // -----------------------------------------------------
    //     Getter for Properties
    // -----------------------------------------------------

    static std::vector<std::string> GetCommandLine ();
    static std::string              GetCommandLineString ();

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    static unsigned int             number_of_threads;

private:
    static std::vector<std::string> arguments;

    // -----------------------------------------------------
    //     Hidden Class Members
    // -----------------------------------------------------

private:
    // everything private as this class is intended to be used fully static.
    Options();
    Options(const Options&);
    Options& operator=(const Options&);

};

} // namespace genesis

#endif // include guard
