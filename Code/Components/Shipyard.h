#pragma once

#include "Vessel.h"

class CShipyard {

	int numVesselsForSale;

	struct VesselForSale {

		CVessel* vessel;
		int value;

	};

};