#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <SDL2/SDL.h>
#include "quadtree.hpp"
#include "point.hpp"
#include "debug.hpp"

class Renderer{
	public:
		Renderer (int width, int height);

		~Renderer();
		int clear();
		int present();
		int drawQuadTree(const QuadTree& node);
		int drawPoint (const Point& p);
		void toggleGrid();
		void handleEvent(bool &quit, const SDL_Event& event);

	private:
		bool m_showGrid = false;
		SDL_Window* window;
		SDL_Renderer* renderer;
};
#endif
