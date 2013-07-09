/**
 * @file   logger.hpp
 * @author yuhonglin <yuhonglin1986@gmail.com>
 * @date   Tue Jan 29 20:55:38 2013
 * 
 * @brief  a logger, only output to standard output
 *         mainly copied from darwin project by Stephen Gould <stephen.gould@anu.edu.au>
 * 
 * 
 */


#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>


#include "constant.hpp"

using namespace std;

/** 
 * commonly used log micro, not suggested for direct use
 * 
 * @param L the log level
 * @param M the log message
 */
#define LOG(L, M) \
  if ((L) > Logger::getLogLevel()) { } \
  else { std::stringstream __s; if (L == LL_FATAL) { __s << "(" << __FILE__ << ", " << __LINE__ << ") "; } \
      __s << M; Logger::logMessage(L, __s.str()); }
/** 
 * log micro that only logs once
 * 
 * @param L the log level
 * @param M the log message
 */
#define LOG_ONCE(L, M) \
  static bool _LoggedMessage ## __LINE__ = false; \
  if (((L) > Logger::getLogLevel()) || _LoggedMessage ## __LINE__) { } \
  else { std::stringstream __s; if (L == LL_FATAL) { __s << "(" << __FILE__ << ", " << __LINE__ << ") "; } \
      __s << M; Logger::logMessage(L, __s.str()); _LoggedMessage ## __LINE__ = true;}

#define LOG_EXCEPT(M) {std::stringstream __s; __s << "(" << __FILE__ << ", " << __LINE__ << ") " << M; throw runtime_error(__s.str());}
#define LOG_FATAL(M) LOG(LL_FATAL, M)
#define LOG_ERROR(M)   LOG(LL_ERROR, M)
#define LOG_WARNING(M) LOG(LL_WARNING, M)
#define LOG_MESSAGE(M) LOG(LL_MESSAGE, M)
#define LOG_STATUS(M)  LOG(LL_STATUS, M)
#define LOG_VERBOSE(M) LOG(LL_VERBOSE, M)
#define LOG_METRICS(M) LOG(LL_METRICS, M)
#define LOG_DEBUG(M)   LOG(LL_DEBUG, M)

#define LOG_ERROR_ONCE(M)   LOG_ONCE(LL_ERROR, M)
#define LOG_WARNING_ONCE(M) LOG_ONCE(LL_WARNING, M)
#define LOG_MESSAGE_ONCE(M) LOG_ONCE(LL_MESSAGE, M)
#define LOG_VERBOSE_ONCE(M) LOG_ONCE(LL_VERBOSE, M)
#define LOG_METRICS_ONCE(M) LOG_ONCE(LL_METRICS, M)
#define LOG_DEBUG_ONCE(M)   LOG_ONCE(LL_DEBUG, M)

#define ASSERT(C) \
  if (!(C)) { LOG(LL_FATAL, #C); }

#define ASSERT_MSG(C, M) \
  if (!(C)) { LOG(LL_FATAL, #C << ": " << M); }

#define TODO LOG_FATAL("not implemented yet");

#define DEPRECATED(C) \
    {LOG_WARNING_ONCE("deprecated code (" << __FILE__ << ", " << __LINE__ << ")"); C;}

#define START_PROGRESS(S, M) Logger::initProgress(S, M)
#define INC_PROGRESS         if (!Logger::incrementProgress()) break;
#define END_PROGRESS         Logger::initProgress()
#define SET_PROGRESS(P)      Logger::updateProgress(P)

// LogLevel ---------------------------------------------------------------
//! Verbosity level in logging.

typedef enum _LogLevel {
    LL_FATAL = 0,  //!< An unrecoverable error has occurred and the code will terminate
    LL_ERROR,      //!< A recoverable error has occurred, e.g., a missing file. 
    LL_WARNING,    //!< Something unexpected happened, e.g., a parameter is zero.
    LL_MESSAGE,    //!< Standard messages, e.g., application-level progress information.
    LL_STATUS,     //!< Status messages, e.g., image names and sizes during loading.
    LL_VERBOSE,    //!< Verbose messages, e.g., intermediate performance results.
    LL_METRICS,    //!< Metrics messages, e.g., detailed process statistics.
    LL_DEBUG	//!< Debugging messages, e.g., matrix inversion results, etc. 
} LogLevel;

// Logger -----------------------------------------------------------------
//! Message and error logging. This class is not thread-safe in the interest
//! of not having to flush the log on every message.
//!
//! \sa \ref LoggerDoc
//!
class Logger
{
 public:
    //! callback for fatal errors
    static void (*showFatalCallback)(const char *message);
    //! callback for non-fatal errors
    static void (*showErrorCallback)(const char *message);
    //! callback for warnings
    static void (*showWarningCallback)(const char *message);
    //! callback for status updates
    static void (*showStatusCallback)(const char *message);
    //! callback for messages (standard, verbose and debug)
    static void (*showMessageCallback)(const char *message);

    //! callback for displaying progress
    static void (*showProgressCallback)(const char *status, double p);

 private:
    static ofstream _log;
    static LogLevel _logLevel;
    static LogLevel _lastMessageLevel;

    static string _progressStatus;
    static int _lastProgress;
    static int _progressLimit;
    static bool _bRunning;

 public:
    Logger();
    ~Logger();

    //! open a log file
    static void initialize(const char *filename,
        bool bOverwrite = false);
    //! open a log file with particular logging level
    static void initialize(const char *filename,
        bool bOverwrite, LogLevel level);
    //! set the current verbosity level
    static inline void setLogLevel(LogLevel level) {
        _logLevel = level;
    }
    //! set the current verbosity level unless already at a lower level
    //! and return previous log level
    static inline LogLevel setAtLeastLogLevel(LogLevel level) {
        const LogLevel lastLevel = _logLevel;
        if (_logLevel < level) _logLevel = level;
        return lastLevel;
    }
    //! get the current verbosity level
    static inline LogLevel getLogLevel() {
        return _logLevel;
    }
    //! check whether current verbosity level is above given level
    static inline bool checkLogLevel(LogLevel level) {
        return (_logLevel >= level);
    }

    //! log a message (see also \p LOG_* macros)
    static void logMessage(LogLevel level, const string& msg);

    // progress feedback
    static void setRunning(bool bRunning) {
        _bRunning = bRunning;
    }
    static bool isRunning() {
        return _bRunning;
    }
    static void initProgress(const char *status = NULL, int maxProgress = 100) {
        _progressStatus = status == NULL ? string("") : string(status);
        _lastProgress = 0; _progressLimit = maxProgress; updateProgress(0.0);
    }
    static void setProgressStatus(const char *status) {
        if (status == NULL) {
            _progressStatus.clear();
        } else {
            _progressStatus = string(status);
        }
        if (showProgressCallback != NULL) {
            showProgressCallback(_progressStatus.c_str(),
                (double)_lastProgress / (double)_progressLimit);
        }
    }
    static bool incrementProgress() {
        if (_lastProgress < _progressLimit) {
            _lastProgress += 1;
            updateProgress((double)_lastProgress / (double)_progressLimit);
        }
        return _bRunning;
    }
    static bool updateProgress(double p) {
        if (showProgressCallback != NULL) {
            showProgressCallback(_progressStatus.c_str(), p);
        }
        return _bRunning;
    }
};


#endif /* _LOGGER_H_ */
