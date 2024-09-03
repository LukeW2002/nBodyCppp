#include "../include/quadtree.hpp"
#include "../include/constants.hpp"
#include "../include/debug.hpp"
#include <math.h>
#include <ostream>

QuadTree::QuadTree(const Node& boundary, int depth) : boundary(boundary), depth(depth), divided(false), m_totalmass(0), m_centreMassX(0), m_centreMassY(0){
	m_points.reserve(Constants::MAX_POINTS) ;
	for (auto& child : m_child){
		child = nullptr;
	}
}

bool QuadTree::subdivide(){
	double x = boundary.x, y = boundary.y, w = boundary.width, h = boundary.height;
	m_child[0] = std::make_unique<QuadTree>(Node(x, y, w/2, h/2), depth + 1);
	m_child[1] = std::make_unique<QuadTree>(Node(x + w/2, y, w/2, h/2), depth + 1);
	m_child[2] = std::make_unique<QuadTree>(Node(x, y + h/2, w/2, h/2), depth + 1);
	m_child[3] = std::make_unique<QuadTree>(Node(x + w/2, y + h / 2, w/2, h/2), depth + 1);
	divided = true;
	    for (const auto& point : m_points) {
        for (auto& child : m_child) {
            if (child->insert(point)) {
                break;
            }
        }
    }
    m_points.clear();

	return divided;
}


void QuadTree::updateMassDistribution(const std::shared_ptr<Point>& p) {
    double newMass = m_totalmass + p->getMass();
    m_centreMassX = (m_centreMassX * m_totalmass + p->getX() * p->getMass()) / newMass;
    m_centreMassY = (m_centreMassY * m_totalmass + p->getY() * p->getMass()) / newMass;
    m_totalmass = newMass;
}

bool QuadTree::insert(std::shared_ptr<Point> p){
	if(!boundary.contains(*p)){
		return false;
	}

	if (m_points.size() < Constants::quadTreeDepth && !divided){
		m_points.push_back(p);
		updateMassDistribution(p);
		return true;
	}

	if (!divided) {
		subdivide();
	}

  for (auto& child : m_child) {
  	if (child->insert(p)) {
  		updateMassDistribution(p);
  		return true;
  		}
  	}
	return  false;
}


int QuadTree::clear(){
	
	return 0;
}
 

double QuadTree::calcforce(Point &p, double theta) const{
	if (m_points.empty() && m_child[0] == nullptr){
		return 0; 
	}
	double dx = m_centreMassX - p.getX();
	double dy = m_centreMassY - p.getY();
	double softening = 0.1;

	double distance = sqrt(dx*dx + dy*dy + softening*softening); 

	// The node is a leaf. The point we are looking at is not in the node then we calculate the the attraction that single point has to the other point

	if (m_points.size() == 1 &&m_points[0].get() != &p){
		double f = Constants::G*p.getMass() * m_points[0]->getMass() / (distance * distance * distance);
		//restricting access to make variables all private might become a problem with SIMD
		p.setFx(p.getFx() + f*dx);
		p.setFy(p.getFy() + f*dy);
	}
	else if((boundary.width / distance < theta) || m_child[0] == nullptr ){ // If point is far we approximate
		if (distance > 0 && m_points.size() > 0){
			double f = Constants::G*p.getMass() * m_totalmass / (distance * distance * distance);
			p.setFx(p.getFx() + f*dx);
			p.setFy(p.getFy() + f*dy);

		}
	} else {
			if (m_child[0]) {
				for (auto& child : m_child){
					child->calcforce(p, theta); // we need to go deeper]
				}
		}
	}
	return 0;
}
