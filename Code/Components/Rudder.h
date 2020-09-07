#pragma once
#include "CryEntitySystem/IEntityComponent.h"
#include "DefaultComponents/Geometry/AdvancedAnimationComponent.h"

class CRudderComponent final : public IEntityComponent {

public:
	//CHullComponent() = default;
	virtual ~CRudderComponent() = default;

	// IEntityComponent
	//virtual void Initialize() override;

	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CRudderComponent>& desc)
	{
		desc.SetComponentFlags({ IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::Transform });
		desc.SetGUID("{002A5F4B-BE6C-498A-9347-ACBB2AAA3E9B}"_cry_guid);
	}

	//void Turn();

	float turnValue;

	//-1 to 1
	//void SetTurnValue(float v) { turnValue = v; }
	//float GetTurnValue() { return turnValue; }
	void Main();

	//is the engine underwater
	bool isSubmerged = false;

protected:

	



};

