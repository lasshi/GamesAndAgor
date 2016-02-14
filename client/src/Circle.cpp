#include "Circle.hpp"

#include <iostream>
#include <cmath>
#include "Engine.hpp"

void Circle::init() {}
void Circle::update() {
	if(m_r < m_sr) m_r += 0.5f;
	if(m_r > m_sr) m_r -= 0.5f;
	// if gameobject is not static it should try to move somewhere
	if(!isStatic) {
		m_speed = (1-(log(m_sr/100.0f)/log(1024)))*5.0f;
		// if respawning or something else happened that we are really far away
		// just jump there
		if(abs(m_sx-m_x)>100 || abs(m_sy-m_y)>100) {
			m_x = m_sx;
			m_y = m_sy;
		}
		int dt = (SDL_GetTicks() - (m_st + Engine::connection->getPing()*2))/1000;
		// predict latest position
		m_sx = m_sx + (m_dir.x*3) * dt * m_speed;
		m_sy = m_sy + (m_dir.y*3) * dt * m_speed;
		// update drawable sphere
		m_x = (m_x + ((m_sx-m_x)/10));
		m_y = (m_y + ((m_sy-m_y)/10));
		// dont let m_x and m_y get over uint16_t boundaries
		if(m_x < 0)m_x = 0;
		if(m_y < 0)m_y = 0;
		if(m_x > 32766) m_x = 32766;
		if(m_y > 32766) m_y = 32766;
	}
	m_destRect = {
		int(m_x), int(m_y),        		// make center of circle true center of drawed texture
		int(m_r*2), int(m_r*2) 			// width and height of circle are r*2
	};
}
void Circle::move(int x, int y) {
    m_x += x;
    m_y += y;
}
