#pragma once
#include "CryEntitySystem/IEntityComponent.h"
#include "DefaultComponents/Geometry/AdvancedAnimationComponent.h"

//
//Class for a player mounted machine gun repeater turret
//
class CMountedMachineGunComponent final : public IEntityComponent {

public:
	//CEngineComponent() = default;
	virtual ~CMountedMachineGunComponent() = default;

	// IEntityComponent
	virtual void Initialize() override;

	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CMountedMachineGunComponent>& desc)
	{
		//desc.SetGUID("{4A0B3A39-13C1-4BFD-BB30-A279110D70E7}"_cry_guid);
	}

public:
	
	void Aim();
	void Fire();

	float shootingSpeed;

	
protected:
	
};
