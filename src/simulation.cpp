#include "../include/simulation.hpp"
#include "../include/constants.hpp"
#include <random>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstdio>

Simulation::Simulation(int width, int height, int numPoints) : root(std::make_unique<QuadTree>(Node (0, 0, width, height), 0)), 
																															 renderer(std::make_unique<Renderer>(width, height))  {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis_angle(0, 2 * M_PI);
  std::uniform_real_distribution<> dis_radius(0, std::min(width, height) / 12.0);
	std::uniform_real_distribution<> dis_mass(1e1,1e4);
	auto centralBody = std::make_shared<Point>(
    width / 2 - 250,   // x
    height / 2,  // y
    0, 0,        // vx, vy
    1e15,        // mass
    0, 0         // fx, fy
  );
  points.push_back(centralBody);
	
	auto centralBody2 = std::make_shared<Point>(
    width / 2 + 250,   // x
    height / 2,  // y
    0, 0,        // vx, vy
    1e12,        // mass
    0, 0         // fx, fy
  );
  

	for (int i = 0; i < numPoints; ++i) {
		std::shared_ptr<Point> galaxy =  (i % 2 == 0) ? centralBody2 : centralBody2;
		double angle = dis_angle(gen);
		double radius = dis_radius(gen);
		double x = galaxy->getX() + radius * std::cos(angle); 
		double y = galaxy->getY() + radius * std::sin(angle); 

		double speed = std::sqrt(Constants::G * galaxy->getMass() / radius);
		double vx = -speed * std::sin(angle) + galaxy->getVx();
    double vy = speed * std::cos(angle)  + galaxy->getVy();
		std::normal_distribution<> velocity_perturbation(0, 500);  // 10 pc/Gyr standard deviation
    vx += velocity_perturbation(gen);
    vy += velocity_perturbation(gen);

        
    double mass = dis_mass(gen);
		points.push_back(std::make_shared<Point>(x,y,vx,vy,mass,0,0));
  }
	LOG_INFO("SIMULATION INITIALISED");
}

void printQuadTree(QuadTree *qt, int depth = 0){

    printf("%*sQuadTree at (%d, %d), width: %d, height: %d, points: %zu\n",
           depth * 2, "", 
           qt->boundary.x, qt->boundary.y ,
           qt->boundary.width, qt->boundary.height ,
           qt->getPoints().size());

    for (const auto& point : qt->getPoints()) {
        printf("%*sPoint at (%.2f, %.2f)\n", (depth + 1) * 2, "", point->getX() , point->getY() );
        printf("%*sHas velocity of (%.2f, %.2f)\n", (depth + 2) * 2, "", point->getVx() , point->getVy() );
        printf("%*sHas mass of %.2f\n", (depth + 2) * 2, "", point->getMass() );
        printf("%*sExperiencing force of (%.2f, %.2f)\n", (depth + 2) * 2, "", point->getFx(), point->getFy());
    }

		    bool hasChildren = false;
    for (size_t i = 0; i < qt->m_child.size(); ++i) {
        const auto& child = qt->m_child[i];
        if (child) {
            hasChildren = true;
            printf("%*sChild %zu:\n", (depth + 1) * 2, "", i);
            printQuadTree(child.get(), depth + 2);
        } else {
            printf("%*sChild %zu: NULL\n", (depth + 1) * 2, "", i);
        }
    }

    if (!hasChildren) {
        printf("%*sNo children\n", (depth + 1) * 2, "");
    }
}

void Simulation::run(){

	PROFILE_START("Frame");
	bool quit = false;
	SDL_Event e;
	auto previousTime = std::chrono::high_resolution_clock::now();

	while (!quit){

		auto currentTime = std::chrono::high_resolution_clock::now();
		double frameTime = std::chrono::duration<double>(currentTime - previousTime).count();
		previousTime = currentTime;
		accumulator += frameTime;

		PROFILE_START("Events");
		while(SDL_PollEvent(&e) != 0){
			renderer->handleEvent(quit, e);
		}
		PROFILE_STOP("Events");
	
		while (accumulator >= fixedDt) {
			update();
    	accumulator -= fixedDt;
    }

		//printQuadTree(root.get(), 0);
		PROFILE_START("Rendering");
		render();
		PROFILE_STOP("Rendering");
		PROFILE_STOP("Frame");
		std::this_thread::sleep_for(std::chrono::milliseconds(4));
	}
	PROFILE_REPORT();
}


void Simulation::update(){
	root = std::make_unique<QuadTree>(Node(0, 0, Constants::width, Constants::height), 0);
  int ID;

  #pragma omp parallel for
	for(size_t i = 0; i < points.size(); i++){
    //ID = omp_get_thread_num();
    //LOG_INFO("Threads Started for Critical (%d) threads", ID);
		#pragma omp critical
		root->insert(points[i]);
	}

  #pragma omp parallel for
	for(size_t i = 0; i < points.size(); i++){
    //ID = omp_get_thread_num();
    //LOG_INFO("Threads Started for Physics (%d) threads", ID);
		points[i]->setFx(0);
		points[i]->setFy(0);
		
		root->calcforce(*points[i], Constants::delta);
		points[i]->updateVelocity(Constants::dt);

		LOG_DEBUG("Point updated: pos=(%.10f, %.10f), vel=(%.10f, %.10f)",points[i]->getX(), points[i]->getY(), points[i]->getVx(), points[i]->getVy());
	}
  points.erase(std::remove_if(points.begin(), points.end(), [this](const std::shared_ptr<Point>& point) {
    return !point->updatePosition(Constants::dt, Constants::width, Constants::height);
  }),
  points.end());

  LOG_INFO("Number of points after update: %zu", points.size());

  #pragma omp parallel for

	for (size_t i = 0; i < points.size(); i++){
    //ID = omp_get_thread_num();
    //LOG_INFO("Threads Started for Position updates (%d) threads",ID);
		points[i]->updatePosition(Constants::dt, Constants::width, Constants::height);
	}
}

void Simulation::render(){
	renderer->clear();
	renderer->drawQuadTree(*root);
	for (const auto& point: points){
		renderer->drawPoint(*point);
	}
	renderer->present();
}
