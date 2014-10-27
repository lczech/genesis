#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

#include "log.hh"

// init static members
bool      Log::log_count   = true;
bool      Log::log_date    = true;
bool      Log::log_time    = true;
bool      Log::log_runtime = true;
bool      Log::log_file    = true;
bool      Log::log_line    = true;
bool      Log::log_level   = true;
LogLevel  Log::max_level_  = kDebug4;
int       Log::count_      = 0;
std::vector<std::ostream*> Log::ostreams_;

/**
 * Set the highest log level that is reported.
 *
 * Invocations of log with higher levels will create no output.
 */
void Log::max_level (const LogLevel level)
{
    if (level > LOG_LEVEL_MAX) {
        LOG_WARN << "Log max level set to " << level << ", but compile time "
                 << "max level is " << LOG_LEVEL_MAX << ", so that "
                 << "everything above that will not be logged.";
    }

    max_level_ = level;
}

/**
 * Return a string representation of a log level
 */
std::string Log::LevelToString(const LogLevel level)
{
    static const char* const buffer[] = {
        "NONE", "ERR ", "WARN", "INFO", "DBG ", "DBG1", "DBG2", "DBG3", "DBG4"
    };
    return buffer[level];
}

/**
 * Add an output stream to which log messages are written.
 */
void Log::AddOutputStream (std::ostream& os)
{
    ostreams_.push_back (&os);
}

/**
 * Add an output file to which log messages are written.
 */
void Log::AddOutputFile (const std::string fn)
{
    std::ofstream* file = new std::ofstream();
    file->open (fn, std::ios::out | std::ios::app);
    if (file->is_open()) {
        ostreams_.push_back (file);
    } else {
        LOG_WARN << "Cannot open log file " << fn;
    }
}

// util to get the current date in nice string form
inline std::string CurrentDate()
{
    std::ostringstream out;
    out.str("");

    time_t now = time(0);
    tm*    ltm = localtime(&now);

    out << ltm->tm_year + 1900;
    out << "-";
    out.fill('0');
    out.width(2);
    out << ltm->tm_mon + 1;
    out << "-";
    out.fill('0');
    out.width(2);
    out << ltm->tm_mday;

    return out.str();
}

// util to get the current time in nice string form
inline std::string CurrentTime()
{
    std::ostringstream out;
    out.str("");

    time_t now = time(0);
    tm*    ltm = localtime(&now);

    out.fill('0');
    out.width(2);
    out << ltm->tm_hour;
    out << ":";
    out.fill('0');
    out.width(2);
    out << ltm->tm_min;
    out << ":";
    out.fill('0');
    out.width(2);
    out << ltm->tm_sec;

    return out.str();
}

// util to get the currently elapsed time for the program in nice string form
// TODO also add a function for the header with elapsed time since last log msg
inline std::string CurrentRuntime()
{
    std::ostringstream out;
    out.str("");
    out << double(clock()) / CLOCKS_PER_SEC;
    return out.str();
}

/**
 * Ctor, does nothing.
 */
Log::Log()
{
}

// TODO the output of the log is not thread safe
/**
 * Dtor that is invoked at the end of each log line and does the actual output.
 */
Log::~Log()
{
    // build the header for the log message
    std::ostringstream header;
    header.str("");
    if (log_count) {
        header.fill('0');
        header.width(4);
        header << count_ << " ";
    }
    if (log_date) {
        header << CurrentDate() << " ";
    }
    if (log_time) {
        header << CurrentTime() << " ";
    }
    if (log_runtime) {
        header << CurrentRuntime() << " ";
    }
    if (log_file) {
        header << file_ << (log_line ? "" : " ");
    }
    if (log_line) {
        header << ":" << line_ << " ";
    }
    if (log_level) {
        header << LevelToString(level_) << " ";
    }

    // add spaces for nested debug levels
    if (level_ > kDebug) {
        for (int i = 0; i < level_ - kDebug; i++) {
            header << "  ";
        }
    }

    // output the message to every stream
    for (std::ostream* out : ostreams_) {
        (*out) << header.str() << os_.str() << std::endl << std::flush;
    }

    // inc log message counter
    count_++;
}

/**
 * Getter for the singleton instance of log, is called by the macros.
 */
std::ostringstream& Log::Get(const std::string file, const int line, const LogLevel level)
{
    // save the information given when called from the macros
    os_.str("");
    file_  = file;
    line_  = line;
    level_ = level;
    return os_;
}
