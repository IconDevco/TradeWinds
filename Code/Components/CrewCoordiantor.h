#pragma once
#include "StdAfx.h"

#include "Components/CrewMember.h"

class CCrewCoordinator {


	int crewSize;
	int maxCrewSize;

	bool hasCaptain;
	//name captain

	//list crew members

	float crewRating;// = crewSize / maxCrewSize;

	void AddCrewMember();
	void MakeCaptain();

	int minimumCrewRequiredForOperation;
	
	void RemoveCrewMember();

	bool MinCrewSizeMet() const { return crewSize >= minimumCrewRequiredForOperation; }
	
	CCrewMember* pCaptain;


};