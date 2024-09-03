#ifndef SIMULATION_HPP
#define SIMULATION_HPP
#include <vector>
#include <memory>
#include <omp.h>
#include "../include/quadtree.hpp"
#include "../include/render.hpp"

class Simulation {
    public:
        Simulation( int width, int height, int numPoints);
        void run();
    private:
        void update();
        void render();
        const double fixedDt = 1.0 / 60.0;  // 60 updates per second
        double accumulator = 0.0;

        std::unique_ptr<QuadTree> root;
        std::vector<std::shared_ptr<Point>> points;
        std::unique_ptr<Renderer> renderer;
};
#endif
