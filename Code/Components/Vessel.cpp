#include "StdAfx.h"
#include "Vessel.h"
#include "Player.h"

#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CryNetwork/Rmi.h>

namespace
{
	static void RegisterVesselComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CVessel));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterVesselComponent);
}

void CVessel::Initialize()
{

	m_pEntity->LoadGeometry(0, "Objects/boat.cgf");

	auto* pBulletMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("Materials/bullet");
	m_pEntity->SetMaterial(pBulletMaterial);

	// Now create the physical representation of the entity
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_RIGID;
	physParams.mass = 3500.f;
	m_pEntity->Physicalize(physParams);


}

Cry::Entity::EventFlags CVessel::GetEventMask() const
	{
	return
		Cry::Entity::EEvent::Update,
		Cry::Entity::EEvent::Initialize;
	}

void CVessel::ProcessEvent(const SEntityEvent& event)
	{
		switch (event.event)
		{
			
		case Cry::Entity::EEvent::Initialize: {
			InitializeComponents();
		}
											break;
		case Cry::Entity::EEvent::Update:
		{

			const float frameTime = event.fParam[0];
			
			if (m_pEngineComponent) {
				if (m_pRudderComponent) {
					m_pRudderComponent->turnValue = rudderAngle;
					m_pEngineComponent->SetRudderAngle(m_pRudderComponent->turnValue);
				}

				m_pEngineComponent->SetThrottle(m_throttle);
			}

		}
		break;
		}
	}

void CVessel::OnClick(EntityId playerId) {
	
	
		if (IEntity* pPlayerEntity = gEnv->pEntitySystem->GetEntity(playerId)) {
			CPlayerComponent* pPCp = pPlayerEntity->GetComponent<CPlayerComponent>();
			
			InitializeComponents();

			CryLog("Boat click from freecam");
			if (!pPCp->IsPilotingVessel()) {
				pPCp->PilotVessel(m_pEntity);
				//pPilot = pPCp;
			}

		}

}

void CVessel::InitializeComponents()
{

	//m_pEngineComponent = m_pEntity->CreateComponent<CEngineComponent>();
	//m_pRudderComponent = m_pEntity->CreateComponent<CRudderComponent>();

	//m_pEngineComponent->SetParent(this);
	if(m_pEngineComponent = m_pEntity->GetOrCreateComponent<CEngineComponent>())
		CryLog("Something went wrong when trying to get or create the engine");
	if(m_pRudderComponent = m_pEntity->GetOrCreateComponent<CRudderComponent>())
		CryLog("Something went wrong when trying to get or create the Rudder");




	//if (!m_pRudderComponent || !m_pEngineComponent) {
		//CryLog("Something went wrong when trying to create the engine or the rudder.");
	//}


	//m_pCrewComponent = GetEntity()->GetOrCreateComponent<CCrewCoordinator>();
	//m_pInventoryComponent = GetEntity()->GetOrCreateComponent<CBoatStorageComponent>();

	if (pInteractiveInterface = GetEntity()->GetOrCreateComponent<IInteractiveInterface>()) {
		pInteractiveInterface->selectionFlags = IInteractiveInterface::ESelectionFlags::Vessel;
	}
}



