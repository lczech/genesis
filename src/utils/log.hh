#ifndef GNS_UTILS_LOG_H_
#define GNS_UTILS_LOG_H_

/**
 * @brief Provides easy and fast logging functionality.
 *
 * All declarations in this file use fully qualified names in the
 * genesis::utils namespace in order to make it easy to include this header
 * in other files without the need for a `using namespace utils` there.
 *
 * For more information on the logging, see Log class.
 *
 * @file
 * @ingroup utils
 */

#include <sstream>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

#ifndef LOG_LEVEL_MAX
/**
 * Static maximal log level.
 * Everything above this level will be pruned by the compiler.
 */
#define LOG_LEVEL_MAX genesis::utils::Log::kDebug4
#endif

// TODO make DEBUG a special macro with proper usage makefile etc,
// also add maybe stuff like RELEASE TEST etc, prepend ENV_ or so!
// TODO do an undefine first

// override this setting when debugging is turned on (eg by makefile)
#ifdef DEBUG
#define LOG_LEVEL_MAX genesis::utils::Log::kDebug4
#endif

// define the actual log macro, so that the compiler can prune calls
// to a log with levels above the static max log level
#define GNS_LOG(level) \
    if (level > LOG_LEVEL_MAX) ; \
    else if (level > genesis::utils::Log::max_level()) ; \
    else genesis::utils::Log().Get(__FILE__, __LINE__, level)

// define a similar log macro, this time changing the details of the message
#define GNS_LOG_DETAILS(level, ...) \
    if (level > LOG_LEVEL_MAX) ; \
    else if (level > genesis::utils::Log::max_level()) ; \
    else genesis::utils::Log().Get(__FILE__, __LINE__, level, {__VA_ARGS__})

// define standard logging types as macro shortcuts
/** %Log an error. See genesis::LogLevel. */
#define LOG_ERR  GNS_LOG(genesis::utils::Log::kError)

/** %Log a warning. See genesis::LogLevel. */
#define LOG_WARN GNS_LOG(genesis::utils::Log::kWarning)

/** %Log an info message. See genesis::LogLevel. */
#define LOG_INFO GNS_LOG(genesis::utils::Log::kInfo)

/** %Log a debug message. See genesis::LogLevel. */
#define LOG_DBG  GNS_LOG(genesis::utils::Log::kDebug)

/** %Log a debug message. See genesis::LogLevel. */
#define LOG_DBG1 GNS_LOG(genesis::utils::Log::kDebug1)

/** %Log a debug message. See genesis::LogLevel. */
#define LOG_DBG2 GNS_LOG(genesis::utils::Log::kDebug2)

/** %Log a debug message. See genesis::LogLevel. */
#define LOG_DBG3 GNS_LOG(genesis::utils::Log::kDebug3)

/** %Log a debug message. See genesis::LogLevel. */
#define LOG_DBG4 GNS_LOG(genesis::utils::Log::kDebug4)

// define special log shortcuts. the list of bools represent
// the members of struct LogDetails and indicate which parts shall be included
/** Logging of a message that is always displayed. See Log. */
#define LOG_BOLD GNS_LOG_DETAILS(genesis::utils::Log::kNone, \
    false, false, false, false, false, false, false, false)

/** Logging of a message with timing information. See Log. */
#define LOG_TIME GNS_LOG_DETAILS(genesis::utils::Log::kDebug, \
    false, false, false, true , true,  false, false, false)

// TODO add TIME1..4 (or TMR maybe) for indented timing logs
// TODO change macros for timing to be out of usual log levels
// TODO define a function that logs the detail column headers (difficult because
// of length difference for file name log detail)
// TODO offer csv as output format
// TODO offer remote streams

/**
 * @brief Settings for which information is included with each log
 * message.
 *
 * The details are activated via accessing the static variable of the
 * log class:
 *
 *     Log::details.level = true;
 *
 * All active details are prepended to the actual log message and
 * separated by spaces (execpt file and line, they are separated by a
 * colon). Their order is fixed.
 *
 * A message with all details activates looks like this
 *
 *     0003 2014-10-28 11:40:47 0.001859 0.000103 src/main/main.cc:28 INFO Hello
 *
 * It was the third message being logged in this run of the program, at
 * a date and time, 0.001859 sec after the program started and 0.000103
 * sec after the last log message. It was called from main.cc line 28
 * and has level Info.
 */
typedef struct {
    /**
     * @brief Include a counter of how many messages have been logged so
     * far.
     */
    bool count;

    /** @brief Include the current date. */
    bool date;

    /** @brief Include the current time. */
    bool time;

    /** @brief Include the current run time of the program in sec. */
    bool runtime;

    /**
     * @brief Include the run time difference to the last log message
     * in sec.
     *
     * Useful for timing and profiling code sections. Is 0.0 at the first
     * log message.
     */
    bool rundiff;

    /** @brief Include the filename where the log message was generated. */
    bool file;

    /**
     * @brief Include the line of the file where the log message was
     * generated.
     */
    bool line;

    /** @brief Include the level (e.g. Info, Debug) of the message. */
    bool level;
} LogDetails;

/**
 * @brief Logging class with easy and fast usage.
 *
 * The basic usage of this class is to invoke the macros for the different types
 * of log messages and send a stream of messages to them:
 *
 *     LOG_DBG << "you are here";
 *     LOG_ERR << "there was an error: " << 42;
 *
 * The provided types of macros are: #LOG_ERR, #LOG_WARN, #LOG_INFO, #LOG_DBG,
 * #LOG_DBG1, #LOG_DBG2, #LOG_DBG3, #LOG_DBG4 for all levels of logging
 * explained in ::LogLevel.
 *
 * The details that are logged with each message can be changed via accessing
 * Log::details -- see LogDetails for more on that.
 *
 * In order to use this class, at least one output stream has to be added first
 * by invoking either AddOutputStream or AddOutputFile.
 *
 * The depths of logging can be changed in order to reduce the amount of written
 * messages. First, at compiletime the macro constant LOG_LEVEL_MAX can be set
 * (e.g. in the Makefile) to the highest level that shall be logged. All log
 * invoakations with a higher level are never called, and moreover will be
 * pruned from the code by most compilers completetly. This makes the log class
 * fast -- instead of deleting all log invokations by hand, it is sufficient to
 * set the constant to a low level.
 * Second, the depths of logging can be changed dynamically at run time by
 * setting Log::max_level to the desired value. Of course, this value
 * cannot be higher than LOG_LEVEL_MAX, because those logs are already pruned
 * by the compiler.
 *
 * There are also two more special log types that create a different output than
 * the previously mentioned types:
 *
 * * #LOG_BOLD does not include any details with its message stream (thus, is
 * independent of Log::details) and is always logged (independent of the max
 * levels). This is used to log the program header and footer on startup and
 * termination.
 *
 * * #LOG_TIME includes the run time difference to the last log message in sec
 * as its only detail (independent of Log::details). This is particularly useful
 * for timing and profiling code sections. Its level is ::kDebug, so
 * that in can be easily turned of for production code.
 *
 * The inner working of this class is as follows: Upon invokation via one of the
 * macros, an instance is created that stays alive only for the rest of the
 * source code line. In this line, the log message is inserted to the buffer
 * stream. After the line finishes, the object is automatically destroyed, so
 * that its destructor is called. In the destructor, the message is composed and
 * logged to all given output streams.
 *
 * Caveat: Because the macro contains conditions depending on the log level,
 * do not use code in a log line that changes the program state.
 * Therefore, instead of
 *
 *     LOG_INFO << "this is iteration " ++i;
 *
 * use
 *
 *     ++i;
 *     LOG_INFO << "this is iteration " i;
 *
 * because the former will not work when the max log level is below info level.
 */
class Log
{
public:
    /**
     * @brief Levels of severity used for logging.
     *
     * The levels are in ascending order and are used both to signal what
     * kind of message is being logged and to provide a threshold for less
     * important messages that can be filtered out, for example debug
     * messages in the production build of the program. Because some of the
     * filtering is already done at compile time, log messages with a level
     * higher than #LOG_LEVEL_MAX do not produce any overhead. See also Log
     * for more on this.
     */
    enum LogLevel {
        /** Special messages that are always logged, e.g. program header. */
        kNone = 0,

        /** Errors, usually non-recoverable. */
        kError,

        /** Warnings if somthing went wront, but program can continue. */
        kWarning,

        /** Infos, for example when a file was written. */
        kInfo,

        /** Basic debugging message. */
        kDebug,

        /** Debugging message with indent level 1 (e.g. for loops). */
        kDebug1,

        /** Debugging message with indent level 2. */
        kDebug2,

        /** Debugging message with indent level 3. */
        kDebug3,

        /** Debugging message with indent level 4. */
        kDebug4
    };

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

    // methods to handle the output streams to write the log messages to
    static void AddOutputStream (std::ostream& os);
    static void AddOutputFile   (const std::string fn);

    /**
     * Settings for which information is included with each log message.
     * See LogDetails for usage.
     */
    static LogDetails details;

    /** Get the highest log level that is reported. */
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
    // Log is kind of singleton, its instances are only provided via the
    // Get functions. do not allow other instances by blocking the copy
    // constructors and copy assignment
    Log (const Log&);
    Log& operator = (const Log&);
};

} // namespace utils
} // namespace genesis

#endif // include guard
