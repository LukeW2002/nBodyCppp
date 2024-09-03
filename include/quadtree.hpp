#ifndef QUADTREE_HPP
#define QUADTREE_HPP
#include <vector>
#include <memory>
#include <omp.h>
#include "point.hpp"
#include <array>

class Node{
	public:
		int x, y, width, height;
		Node(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {};
		bool contains(const Point& point) const {
			return (point.getX() >= x - width && point.getX() < x + width &&
					    point.getY() >= y - height && point.getY() < y + height);
		}
};

class QuadTree{
	public:
		QuadTree(const Node& boundary, int depth);
		bool insert(std::shared_ptr<Point> p);
		int clear();
		bool subdivide();
//		bool inBounds(const Point &p) const;
		double calcforce(Point &p, double theta) const;

		void updateMassDistribution(const std::shared_ptr<Point> &p);

		double getTotalMass() const {return m_totalmass;};
		double getCentreMassX() const {return m_centreMassX;};
		double getCentreMassY() const {return m_centreMassY;};
		const std::vector<std::shared_ptr<Point>>& getPoints() const {return m_points;};
		Node boundary;
		std::array<std::unique_ptr<QuadTree>, 4> m_child;

	private:
		int depth;
		bool divided;
		std::vector<std::shared_ptr<Point>> m_points;
		double m_totalmass;
		double m_centreMassX, m_centreMassY;
};

#endif
