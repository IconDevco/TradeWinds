#include "StdAfx.h"
#include "Boat.h"
#include "Engine.h"




void CBoatComponent::Initialize()
{
	
	m_pEngineComponent = GetEntity()->GetOrCreateComponentClass<CEngineComponent>();
	m_pHullComponent = GetEntity()->GetOrCreateComponentClass<CHullComponent>();


	IAttachmentManager* pAttatchmentManager = m_pAnimationComponent->GetCharacter()->GetIAttachmentManager();
	IAttachment* pRudderAttatchment = pAttatchmentManager->GetInterfaceByName("Rudder");
	if (pRudderAttatchment) {
		IAttachmentObject* pRudderObject = pRudderAttatchment->GetIAttachmentObject();

		
		STerrainInfo tInfo;
		float waterLevel = tInfo.oceanWaterLevel;

		m_pRudderComponent->isSubmerged = (pRudderObject->GetIRenderNode()->GetPos().z < waterLevel);
	}

}
