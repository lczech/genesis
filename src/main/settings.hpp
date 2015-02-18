#ifndef GNS_MAIN_SETTINGS_H_
#define GNS_MAIN_SETTINGS_H_

/**
 * @brief
 *
 * @file
 * @ingroup main
 */

namespace genesis {

/**
 * @brief Simple settings class for application-wide configuration and options.
 */
class Settings
{
public:
    /**
     * @brief Singleton Getter.
     */
    static Settings& Get()
    {
        static Settings instance_;
        return instance_;
    }

#ifdef PTHREADS
    unsigned int number_of_threads;
#endif

private:
    // everything private due to singleton.
    Settings();
    Settings(const Settings&);
    Settings& operator=(const Settings&);
};

} // namespace genesis

#endif // include guard
