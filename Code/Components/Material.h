#pragma once

class CMaterialManager {

public:



	SMaterial* MaterialIron = new SMaterial("Iron", 100);
	//booty
	//lumber
	//alcohol
	//food
	//ammunition
	//fuel
	
};	

struct SMaterial {
	SMaterial(const char* n, int max) :
		name(n),
		maxStack(max)
	{}

	const char* name;
	int amount = 1;
	int maxStack = 100;
};