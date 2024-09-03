#include "../include/render.hpp"

Renderer::Renderer (int width, int height){
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		LOG_ERROR("SDL COULD NOT INITIALISE: SDL ERROR: %s", SDL_GetError());
		throw std::runtime_error("SDL INIT FAILED");
		Logger::close();
	}
	
	window = SDL_CreateWindow("Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	
	if (window == nullptr){
		LOG_ERROR("WINDOW COULD NOT BE CREATED: SDL ERROR: %s", SDL_GetError());
		throw std::runtime_error("WINDOW COULD NOT BE CREATED");
		Logger::close();
	}
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	if (renderer == nullptr){
		LOG_ERROR("RENDERER COULD NOT BE CREATED: SDL ERROR: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		throw std::runtime_error("RENDERER COULD NOT BE CREATED");
		Logger::close();
	}
	
	LOG_INFO("Renderer initialized successfully!");
};

Renderer::~Renderer(){
	if (renderer)
	{
		SDL_DestroyRenderer(renderer);
	}
	
	if (window){
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
	LOG_INFO("Renderer Destroyed!");
};

int Renderer::clear(){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	return 0;
}

int Renderer::present(){
	SDL_RenderPresent(renderer);
	return 0;
}

int Renderer::drawQuadTree(const QuadTree &node)
{
    if (m_showGrid) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_Rect rect = {
            static_cast<int>(node.boundary.x), 
            static_cast<int>(node.boundary.y),
            static_cast<int>(node.boundary.width),
            static_cast<int>(node.boundary.height)
        };
        SDL_RenderDrawRect(renderer, &rect);
    }

    for (const auto& point : node.getPoints()) {
        drawPoint(*point);
    }

    for (const auto& child : node.m_child) {
        if (child) {
						LOG_DEBUG("Drawing child QuadTree");
            drawQuadTree(*child);
        } 
    }
		return 0;
}

int Renderer::drawPoint(const Point& p) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect pointRect = {
        static_cast<int>(p.getX()) - 2,
        static_cast<int>(p.getY()) - 2,
        3,
        3
    };
    SDL_RenderFillRect(renderer, &pointRect);
		return 0;
}

void Renderer::toggleGrid() {
    m_showGrid = !m_showGrid;
}
void Renderer::handleEvent(bool &quit, const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							quit = true;
            case SDLK_SPACE:
                toggleGrid();
                break;
        }
    }
}


