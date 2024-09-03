#include "../include/simulation.hpp"
#include "../include/debug.hpp"
#include "../include/constants.hpp"
#include <iostream>
#include <stdexcept>


int main(int argc, char* argv[]){
	try {
		Logger::init("NbodySim.log");
		Logger::setLogLevel(LogLevel::ERROR);
		LOG_INFO("Starting Nbody simulation");
		
		int width = Constants::width;
		int height = Constants::height;
		int numPoints = Constants::defaultNumPoints;
		if (argc > 1){
			numPoints = std::stoi(argv[1]);
			if (numPoints > Constants::MAX_POINTS){
				LOG_WARNING("Exceeded maximum points. Restting to allowed value (%d)", Constants::MAX_POINTS);
				numPoints = Constants::MAX_POINTS;
			}
		}
		LOG_INFO("Initializing simulation with dimensions %dx%d and %d points", width, height, numPoints);

		Simulation sim(width, height, numPoints);
    sim.run();

    LOG_INFO("Simulation completed successfully");

		Logger::close();
		return 0;
	}
	catch(const std::exception& e){
		LOG_ERROR("An error Occured: %s", e.what());
		std::cerr << "An error occured: " << e.what() << std::endl;
		Logger::close();
		return 1;
	}
}

