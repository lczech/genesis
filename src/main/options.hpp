#ifndef GNS_MAIN_OPTIONS_H_
#define GNS_MAIN_OPTIONS_H_

/**
 * @brief
 *
 * @file
 * @ingroup main
 */

#include <string>
#include <vector>

namespace genesis {

/**
 * @brief Simple Options class for application-wide configuration and settings.
 */
class Options
{
public:
    /**
     * @brief Singleton Getter.
     */
    static Options& Get()
    {
        static Options instance_;
        return instance_;
    }

    bool                     SetCommandLine (int argc, char* argv[]);
    std::vector<std::string> GetCommandLine ();
    std::string              GetCommandLineString ();

#   ifdef PTHREADS
        unsigned int number_of_threads;
#   endif

private:
    // everything private due to singleton.
    Options();
    Options(const Options&);
    Options& operator=(const Options&);

    std::vector<std::string> arguments;
};

} // namespace genesis

#endif // include guard
