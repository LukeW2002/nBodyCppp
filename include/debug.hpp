#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <map>
#include <iomanip>

#define DEBUG_ALL

enum class LogLevel {
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

class Logger {
	public:
		static void init (const std::string& filename);
		static void close();
		static void setLogLevel(LogLevel level);
		static void log(LogLevel level, const char* file, int line, const char* func, const char* format, ...);
		// Helpers
		static void logPoint( const std::string& prefix, double x, double y);
		static void logQuadTree(const std::string& prefix, double x, double y, double width, double height);
		static void logMemoryUsage();

	private:
		static LogLevel currentLogLevel;
		static std::ofstream logFile;
		static std::chrono::system_clock::time_point startTime;
		static const char* logLevelStrings[];
};

class Profiler {
public:
    static void start(const std::string& section);
    static void stop(const std::string& section);
    static void report();

private:
    static std::map<std::string, std::chrono::high_resolution_clock::time_point> startTimes;
    static std::map<std::string, double> totalTimes;
    static std::map<std::string, int> callCounts;
};


#ifndef NO_LOGGING
    #define LOG_DEBUG(fmt, ...) Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
    #define LOG_INFO(fmt, ...) Logger::log(LogLevel::INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
    #define LOG_WARNING(fmt, ...) Logger::log(LogLevel::WARNING, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
		#define PROFILE_START(section) Profiler::start(section)
		#define PROFILE_STOP(section) Profiler::stop(section)
		#define PROFILE_REPORT() Profiler::report()
    #define LOG_ERROR(fmt, ...) Logger::log(LogLevel::ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#else
		#define PROFILE_START(section)
		#define PROFILE_STOP(section) 
		#define PROFILE_REPORT() 
    #define LOG_DEBUG(fmt, ...)
    #define LOG_INFO(fmt, ...)
    #define LOG_WARNING(fmt, ...)
    #define LOG_ERROR(fmt, ...)
#endif


#endif
