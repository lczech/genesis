// include guard
#ifndef GNS_UTILS_LOG_H_
#define GNS_UTILS_LOG_H_

#include <sstream>
#include <string>
#include <vector>

// static max log level, everything above this will be pruned by the compiler
// (first check, if it is not yet set, for example by makefile)
#ifndef LOG_LEVEL_MAX
#define LOG_LEVEL_MAX kDebug4
#endif

// override this setting when debugging is turned on (eg by makefile)
#ifdef DEBUG
#define LOG_LEVEL_MAX kDebug4
#endif

// define the actual log macro, so that the compiler can prune calls
// to a log with levels above the static max log level
#define GNS_LOG(level) \
    if (level > LOG_LEVEL_MAX) ; \
    else if (level > Log::max_level()) ; \
    else Log().Get(__FILE__, __LINE__, level)

// define a similar log macro, this time changing the details of the message
#define GNS_LOG_DETAILS(level, ...) \
    if (level > LOG_LEVEL_MAX) ; \
    else if (level > Log::max_level()) ; \
    else Log().Get(__FILE__, __LINE__, level, {__VA_ARGS__})

// define standard log shortcuts
#define LOG_ERR  GNS_LOG(kError)
#define LOG_WARN GNS_LOG(kWarning)
#define LOG_INFO GNS_LOG(kInfo)
#define LOG_DBG  GNS_LOG(kDebug)
#define LOG_DBG1 GNS_LOG(kDebug1)
#define LOG_DBG2 GNS_LOG(kDebug2)
#define LOG_DBG3 GNS_LOG(kDebug3)
#define LOG_DBG4 GNS_LOG(kDebug4)

// define special log shortcuts. the list of bools represent
// the members of struct LogDetails and indicate which parts shall be included
#define LOG_MSG  GNS_LOG_DETAILS(kNone, \
    false, false, false, false, false, false, false, false)
#define LOG_TIME GNS_LOG_DETAILS(kDebug, \
    false, false, false, false, true,  false, false, false)

// define the log levels
enum LogLevel {
    kNone = 0,
    kError,
    kWarning,
    kInfo,
    kDebug,
    kDebug1,
    kDebug2,
    kDebug3,
    kDebug4
};

// settings for which information is included with each log message
typedef struct {
    bool count;
    bool date;
    bool time;
    bool runtime;
    bool rundiff;
    bool file;
    bool line;
    bool level;
} LogDetails;

// TODO define a function that logs the detail column headers
// TODO offer csv as output format
// TODO offer remote streams

/**
 * Log class with easy and fast usage.
 *
 * The basic usage works via macros for the different types of log messages:
 *     LOG_DBG << "you are here";
 *     LOG_ERR << "there was an error: " << 42;
 * The provided types of macros are:
 *     LOG_ERR, LOG_WARN, LOG_INFO, LOG_DBG, LOG_DBG1, LOG_DBG2, LOG_DBG3, LOG_DBG4
 *
 * Caveat: Because the macro already contains conditions depending on the log
 * level, do not use code in a log line that changes the program state.
 * Instead of
 *     LOG_INFO << "this is iteration " i++;
 * use
 *     LOG_INFO << "this is iteration " i;
 *     i++;
 * because the former will not work when the max log level is below info level.
 *
 * TODO document why it is fast using the macros, talk about the log details,
 * the output streams, say something about how the whole thing works with
 * the stream and output on dtor. also mention the special logs.
 */
class Log
{
    public:
        Log();
        ~Log();

        // getter for the singleton instance of log, is called by the macros
        std::ostringstream& Get (
            const std::string file, const int line,
            const LogLevel level
        );
        std::ostringstream& Get (
            const std::string file, const int line,
            const LogLevel level, const LogDetails dets
        );

        // methods to add output streams to write the log messages to
        static void AddOutputStream (std::ostream& os);
        static void AddOutputFile   (const std::string fn);

        // settings for which information is included with each log message
        static LogDetails details;

        // get/set the highest log level that is reported
        static LogLevel  max_level () {return max_level_;};
        static void      max_level (const LogLevel level);

        // return a string representation for a log level
        static std::string LevelToString (const LogLevel level);

    protected:
        // storage for information needed during one invocation of a log
        std::ostringstream buff_;
        std::string        file_;
        int                line_;
        LogLevel           level_;
        LogDetails         details_;

        // dynamic log level limit
        static LogLevel max_level_;

        // how many log calls were made so far
        static long    count_;
        static clock_t last_clock_;

        // array of streams that are used for output
        static std::vector<std::ostream*> ostreams_;

    private:
        // Log is singleton, do not allow other instances by
        // blocking copy and assignment ctors
        Log (const Log&);
        Log& operator = (const Log&);
};

#endif // include guard
