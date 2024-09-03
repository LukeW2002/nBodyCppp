#include "../include/point.hpp"
#include "../include/debug.hpp"
#include <immintrin.h>
#include <math.h>
#include <cstdio>
#include <mmintrin.h>
#include <algorithm>


Point::Point(double x, double y, double vx, double vy, double mass, double fx, double fy) : m_x(x), m_y(y), m_vx(vx), m_vy(vy), m_mass(mass), m_fx(fx), m_fy(fy){
	LOG_DEBUG("Point created: pos=(%.2f, %.2f), vel=(%.2f, %.2f), mass=%.2f", m_x, m_y, m_vx, m_vy, m_mass);
	data = _mm256_setr_pd(x, y, vx, vy);
	data2 = _mm256_setr_pd(fx, fy, mass, 0);
};

int Point::updateVelocity(double dt){
	__m256d dt_vector = _mm256_set1_pd(dt); 
	__m256d mass_vector = _mm256_permute_pd(_mm256_permute2f128_pd(data2, data2, 0x01), 0b0000); // [fx, fy, mass, 0] - > [mass 0, mass, 0] -> [mass, mass, mass, mass] 
	__m256d force_vector = _mm256_permute2f128_pd(data2, data2, 0x01); // [fx, fy, mass, 0] -> [fx, fy, fx, fy]

	//double old_vx = m_vx, old_vy = m_vy;
	//m_vx += m_fx *dt / m_mass;
	//m_vy += m_fy *dt / m_mass;
	//LOG_DEBUG("Velocity updated with force: (%.5f, %.5f): (%.5f, %.5f) -> (%.5f, %.5f)", m_fx/m_mass, m_fy/m_mass, old_vx, old_vy, m_vx, m_vy);
	return 0;
}

bool Point::updatePosition(double dt, double width, double height){
	double old_x = m_x, old_y = m_y;
	m_x += m_vx * dt;
	m_y += m_vy * dt;
	double new_x = 2 * m_x - old_x + (m_fx / m_mass) * dt * dt;
  double new_y = 2 * m_y - old_y + (m_fy / m_mass) * dt * dt;
    
  m_vx = (new_x - old_x) / (2 * dt);
  m_vy = (new_y - old_y) / (2 * dt);
  if (m_x < 0 || m_x >= width || m_y < 0 || m_y >= height) {
		LOG_DEBUG("Point out of bounds: (%.5f, %.5f)", m_x, m_y);
		return false; // Point should be removed
  }
	LOG_DEBUG("Position updated: (%.5f, %.5f) -> (%.5f, %.5f)", old_x, old_y, m_x, m_y);

	return true;
}
