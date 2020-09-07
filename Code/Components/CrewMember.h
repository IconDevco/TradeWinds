#pragma once
class CCrewMember {


	//determines the efficiency of a crewmate
	int workRating; //1-6

	//qualified if over 4
	bool isQualifiedCaptain;
	bool isCaptain;

	//1-10, 10 gauranteed to join opposing vessel on defeat
	int deserterRating; 

	int experience; //2000xp = 1 rating


}; 