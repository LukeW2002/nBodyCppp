#ifndef POINT_HPP
#define POINT_HPP
#include <immintrin.h>

#define mD __m256d

class Point {
	public: 
		Point(double x, double y, double vx, double vy, double mass, double fx, double fy);
	
		bool updatePosition(double dt, double width, double height);
		int updateVelocity(double dt);
	
		double getX() const {return m_x;};
		double getY() const {return m_y;};

		double getVx() const {return m_vx;};
		double getVy() const {return m_vy;};

		double getFx() const {return m_fx;};
		double getFy() const {return m_fy;};

		double getMass() const {return m_mass;};

		void setX(double x) {m_x = x;};
		void setY(double y) {m_y = y;};

		void setVx(double vx) {m_vx = vx;};
		void setVy(double vy) {m_vy = vy;};

		void setFx(double fx) {m_fx = fx;};
		void setFy(double fy) {m_fy = fy;};

		void setMass(double mass) {m_mass = mass;};

	private:
		__m256d data;  // x, y, vx, vy,
		__m256d data2; // Fx, Fy, mass, 0
		double m_x, m_y;
		double m_vx, m_vy;
		double m_fx, m_fy;
		double m_mass;
};
#endif
