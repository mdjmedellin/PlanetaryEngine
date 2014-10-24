#pragma once
#ifndef EXPLOSION_FX_H
#define EXPLOSION_FX_H

#include <vector>
#include "Vector3.hpp"

struct Sphere
{
	Vector3 center;
	Vector3 velocity;
	float radius;
};

namespace gh
{
	class ExplosionFX
	{
	public:
		ExplosionFX( float lifespan );
		void update();
		void render();

	private:
		float m_lifespan;
		std::vector< Sphere >m_particles;
	};
}

#endif