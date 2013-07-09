#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <cstring>

#include "logger.hpp"

using namespace std;

// logging callbacks
void (*Logger::showFatalCallback)(const char *message) = NULL;
void (*Logger::showErrorCallback)(const char *message) = NULL;
void (*Logger::showWarningCallback)(const char *message) = NULL;
void (*Logger::showStatusCallback)(const char *message) = NULL;
void (*Logger::showMessageCallback)(const char *message) = NULL;

void (*Logger::showProgressCallback)(const char *status, double p) = NULL;

ofstream Logger::_log;
LogLevel Logger::_logLevel = LL_MESSAGE;
LogLevel Logger::_lastMessageLevel = LL_MESSAGE;

bool Logger::_bRunning = true;
string Logger::_progressStatus;
int Logger::_lastProgress = 0;
int Logger::_progressLimit = 100;

Logger::Logger()
{
    // do nothing
}

Logger::~Logger()
{
    // do nothing
}

void Logger::initialize(const char *filename, bool bOverwrite)
{
    if (_log.is_open()) {
        _log.close();
    }

    if ((filename != NULL) && (strlen(filename) > 0)) {
        _log.open(filename, bOverwrite ? ios_base::out : ios_base::out | ios_base::app);
        assert(!_log.fail());

        time_t t = time(NULL);
        struct tm *lt = localtime(&t);
        char buffer[256];
        if (strftime(&buffer[0], sizeof(buffer), "%c", lt) == 0) {
            buffer[0] = '\0';
        }

        _log << "-" << setw(2) << setfill('0') << lt->tm_hour
             << ":" << setw(2) << setfill('0') << lt->tm_min
             << ":" << setw(2) << setfill('0') << lt->tm_sec
             << "--- log opened: " << buffer << " (version: " << VERSION << ") --- \n";
    }
}

void Logger::initialize(const char *filename,
    bool bOverwrite, LogLevel level)
{
    _logLevel = level;
    initialize(filename, bOverwrite);
}

void Logger::logMessage(LogLevel level, const string& msg)
{
    if (level > _logLevel) return;

    char prefix[4] = "---";
    switch (level) {
      case LL_FATAL:   prefix[1] = '*'; break;
      case LL_ERROR:   prefix[1] = 'E'; break;
      case LL_WARNING: prefix[1] = 'W'; break;
      case LL_MESSAGE: prefix[1] = '-'; break;
      case LL_STATUS:  prefix[1] = '-'; break;
      case LL_VERBOSE: prefix[1] = '-'; break;
      case LL_METRICS: prefix[1] = '-'; break;
      case LL_DEBUG:   prefix[1] = 'D'; break;
    }

    // write message to file
    if (_log.is_open() && (level != LL_STATUS)) {
        time_t t = time(NULL);
        struct tm *lt = localtime(&t);

        _log << "-" << setw(2) << setfill('0') << lt->tm_hour
             << ":" << setw(2) << setfill('0') << lt->tm_min
             << ":" << setw(2) << setfill('0') << lt->tm_sec
             << prefix << ' ' << msg << "\n";
    }

    // display the message
    if ((_lastMessageLevel == LL_STATUS) && (level != LL_STATUS)) {
        cerr << "\n";
    }

    switch (level) {
    case LL_FATAL:
        if (showFatalCallback == NULL) {
            cerr << prefix << ' ' << msg << endl;
        } else {
            showFatalCallback(msg.c_str());
        }
        break;
    case LL_ERROR:
        if (showErrorCallback == NULL) {
            cerr << prefix << ' ' << msg << endl;
        } else {
            showErrorCallback(msg.c_str());
        }
        break;
    case LL_WARNING:
        if (showWarningCallback == NULL) {
            cerr << prefix << ' ' << msg << endl;
        } else {
            showWarningCallback(msg.c_str());
        }
        break;
    case LL_STATUS:
        if (showStatusCallback == NULL) {
            cerr << prefix << ' ' << msg << '\r';
            cerr.flush();
        } else {
            showStatusCallback(msg.c_str());
        }
        break;
    default:
        if (showMessageCallback == NULL) {
            cout << prefix << ' ' << msg << endl;
        } else {
            showMessageCallback(msg.c_str());
        }
    }

    _lastMessageLevel = level;

    // abort on fatal errors
    if (level == LL_FATAL) {
        abort();
    }
}
