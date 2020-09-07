#pragma once

#include "CryEntitySystem/IEntityComponent.h"



class CFleetCoordinator : public IEntityComponent {


	int fleetPower;

	int fleetSize;
	int maxFleetSize;

	float maxFleetSpeed;

	//creates a formation 
	void OrganizeFormation(); 

	void SortBySize();


	int numPresentTiers;

	void CreateFormationRingFromSize(float power);





	struct TierFormation {

		int tier = 0;

		//number of vessels in this formation of this tier
		int numVessels = 0;
		float averagePower = 0;

		const size_t GetNumVessels() const { return numVessels; }

		std::list<Vec3> points;
	
		void CreatePoints();

	};



};