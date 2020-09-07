#include "StdAfx.h"
#include "Engine.h"
//#include "Vessel.h"



void CEngineComponent::Initialize()
{
	//const int geometrySlot = 1;
	//m_pEntity->LoadGeometry(geometrySlot, "%ENGINE%/EngineAssets/Objects/primitive_sphere.cgf");

	//pe_action_add_constraint
	m_throttle = 0;
	horsepower = 10;
	rudderAngle = 0;
	
	
}

void CEngineComponent::Accelerate(float fTime)
{
	Vec3 v = m_pEntity->GetForwardDir();
	//float rudderAngle = 

	Vec3 impulseDir = Vec3(0);

	impulseDir = Vec3(
		v.y + rudderAngle,
		v.y,
		0
	);

	float f = Lerp<float>(f, m_throttle, fTime);
	float throttlePower = horsepower * f;

	pe_action_impulse impulse;

	impulse.impulse = impulseDir * throttlePower;
	impulse.point = GetEntity()->GetWorldPos();
	if (IPhysicalEntity* pEnt = GetEntity()->GetPhysics()) {
		pEnt->Action(&impulse);
	}
}

void CEngineComponent::Throttle()
{
	//m_throttle = pParentVessel->GetThrottle();
	//rudderAngle = pParentVessel->GetRudderAngle();

}

Cry::Entity::EventFlags CEngineComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::Update;
}

void CEngineComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{


	case Cry::Entity::EEvent::Update:
	{
		const float frameTime = event.fParam[0];
		Accelerate(frameTime);
	}
	break;
	}
}