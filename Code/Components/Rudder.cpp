#include "StdAfx.h"
#include "Rudder.h"

Cry::Entity::EventFlags CRudderComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::Reset |
		Cry::Entity::EEvent::Update;
}

void CRudderComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::Reset:
	{
		
	}
	break;

	case Cry::Entity::EEvent::Update:
	{

		//STerrainInfo tInfo;
		//float waterLevel = tInfo.oceanWaterLevel;

		//isSubmerged = (GetIRenderNode()->GetPos().z < waterLevel);
	}
	break;
	}


}

