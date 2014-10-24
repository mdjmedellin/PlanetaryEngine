#ifndef BULLET_H
#define BULLET_H
#pragma once

#include "Actor.h"

namespace gh
{
	class ActorData;

	class Bullet : public Actor
	{
	public:
		Bullet( unsigned int teamIndex );
		Bullet( ActorData* actorInformation );
		~Bullet();

		virtual void draw();
		virtual bool isReadyToDie() const;
		virtual void getActorData(  ActorData* out_actorData  );
		void setVelocity( float shipOrientation );
		bool diedBeforeTime();

	private:
		double timeOfCreation;
	};

}

#endif