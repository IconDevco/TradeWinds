#pragma once

#include "CryEntitySystem/IEntityComponent.h"
#include "Components/BoatStorageComponent.h"
#include "Components/CrewCoordiantor.h"

#include "Components/Engine.h"
#include "Components/Rudder.h"

#include "DefaultComponents/Geometry/StaticMeshComponent.h"

#include "CryAISystem/Components/IEntityListenerComponent.h"

#include "InteractiveInterface.h"


//Represents a vessel and it's hull, as well as all of the other components that make up a working vessel
//Guns, engines, so on.

class CPlayerComponent;
class CVessel : public IEntityComponent {
public:

	CVessel() = default;
	virtual ~CVessel() = default;

	// IEntityComponent
	virtual void Initialize() override;

	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent & event) override;
	// ~IEntityComponent

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CVessel> & desc)
	{
		
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach });
		desc.SetEditorCategory("Game");
		desc.SetLabel("Vessel");
		desc.SetDescription("Controllable Vessel");
		desc.SetGUID("{4A0B3A39-13C1-4BFD-BB30-A279110D70E7}"_cry_guid);
	}


	// InteractiveInterface
	//template <typename T>
	void OnClick(EntityId playerId);

	void SetThrottle(float f) { m_throttle = f; }
	void SetRudderAngle(float a) { rudderAngle = a; }

	float GetThrottle() { return m_throttle; }
	float GetRudderAngle() { return rudderAngle; }

	//~InteractiveInterface
	void AbandonPilot();
	void InitializeComponents();

	CEngineComponent* GetEngine() { return m_pEngineComponent; }
	CRudderComponent* GetRudder() { return m_pRudderComponent; }

protected:
	
	CEngineComponent* m_pEngineComponent = nullptr;
	CRudderComponent* m_pRudderComponent = nullptr;
	
	float m_throttle;
	float rudderAngle;
	
	//combat coordinator

	//CCrewCoordinator* m_pCrewComponent;
	//CBoatStorageComponent* m_pInventoryComponent;
	

	//is the HNIC boat for the fleet. usually the biggest boat in the center
	//bool isFlagship;

	//bool isPiloted;          

	//how much experience to award the conquering crews 
	//int experienceValue;

	//bool isPartOfFleet;
	//bool isAnchored;

	//void SetFlagship(bool b);

	//void ReleaseAnchor();
	//void RetrieveAnchor();

	//void Capsize();

	//void DockToPort();

	//IEntityFactionComponent
//	IEntityFactionComponent* m_pFactionComponent;
	//IEntityObserverComponent* m_pObserverComponent;
	
	//IEntityObservableComponent* m_pObservableComponent;

	//Hull Mesh
	//Cry::DefaultComponents::CStaticMeshComponent* m_pMeshComponent;

	void Main() {


		//m_pMeshComponent->
		//m_pFactionComponent->SetFactionId(1);
		//gEnv->pAISystem->GetFactionMap().
	}


	//////////////////////////////////////////////////////////////////
	IInteractiveInterface* pInteractiveInterface = nullptr;
	//ESelectionFlags selectionFlags;

	
	
};