#pragma once

#include "CryEntitySystem/IEntityComponent.h"

//this component drives the boat when it is not being pioneered by a player

class CCaptainComponent : IEntityComponent {

	
	Vec3 desiredPos;

	//how close a vessel can get to the desired position before it's "close enough"
	float comfortableDistance;


	void SetWaypoint(Vec3 p);

	void DriveToPoint();
	





};