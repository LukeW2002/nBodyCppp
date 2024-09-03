#include "../include/debug.hpp"
#include <cstdarg>
#include <cstring>

LogLevel Logger::currentLogLevel = LogLevel::DEBUG;

std::ofstream Logger::logFile;
std::chrono::system_clock::time_point Logger::startTime;

const char* Logger::logLevelStrings[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
std::map<std::string, std::chrono::high_resolution_clock::time_point> Profiler::startTimes;
std::map<std::string, double> Profiler::totalTimes;
std::map<std::string, int> Profiler::callCounts;

void Profiler::start(const std::string& section) {
    startTimes[section] = std::chrono::high_resolution_clock::now();
}

void Profiler::stop(const std::string& section) {
    auto stopTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTimes[section]).count();
    totalTimes[section] += duration / 1000000.0; // Convert to seconds
    callCounts[section]++;
}

void Profiler::report() {
    LOG_INFO("Profiling Report:");
    for (const auto& [section, totalTime] : totalTimes) {
        double avgTime = totalTime / callCounts[section];
        LOG_INFO("  %s: Total time: %.6f s, Calls: %d, Avg time: %.6f s", 
                 section.c_str(), totalTime, callCounts[section], avgTime);
    }
}

void Logger::init(const std::string& filename){
	logFile.open(filename, std::ios::trunc);
	if(!logFile.is_open())
	{
		std::cerr << "Failed to open Log file." << filename << std::endl;
		exit(1);
	}

	startTime = std::chrono::system_clock::now();
	auto now = std::chrono::system_clock::to_time_t(startTime);
	logFile << "\n--- Logging Started at " << std::ctime(&now);
	logFile.flush();
}

void Logger::close(){
	if (logFile.is_open()){
		auto endTime = std::chrono::system_clock::now();
		auto now = std::chrono::system_clock::to_time_t(endTime);
		logFile << "--- Logging ended at" << std::ctime(&now);
	}
}

void Logger::setLogLevel(LogLevel level){
	currentLogLevel = level;
}

void Logger::log(LogLevel level, const char* file, int line, const char* func, const char* format, ...){
	if (level < currentLogLevel || !logFile.is_open()) return;
	auto now = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();

	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	logFile << "[" << elapsed << "] [" << logLevelStrings[static_cast<int>(level)] << "] "
            << file << ":" << line << " in " << func << "(): " << buffer << std::endl;
  logFile.flush();
}

void Logger::logPoint(const std::string& prefix, double x, double y) {
    LOG_DEBUG("%s: (%.2f, %.2f)", prefix.c_str(), x, y);
}

void Logger::logQuadTree(const std::string& prefix, double x, double y, double width, double height) {
    LOG_DEBUG("%s: (%.2f, %.2f) %.2f x %.2f", prefix.c_str(), x, y, width, height);
}

void Logger::logMemoryUsage() {
    LOG_DEBUG("Memory usage logging not implemented");
}

