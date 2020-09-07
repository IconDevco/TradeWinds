#pragma once

#include "DefaultComponents/Cameras/CameraComponent.h"
#include "DefaultComponents/Cameras/CameraManager.h"
#include "DefaultComponents/Cameras/ICameraManager.h"



class CPlayerCamera {


	enum { 
		maxAllowed = 32, 
		minAllowed = 2
	};

	uint32 mUnits[maxAllowed];

	





	Cry::DefaultComponents::CCameraComponent* camera;

	void Main() {

	
	}

};