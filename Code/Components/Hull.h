#pragma once
#include "CryEntitySystem/IEntityComponent.h"
#include "DefaultComponents/Geometry/AdvancedAnimationComponent.h"
class CHullComponent final : public IEntityComponent {

public:
	//CHullComponent() = default;
	virtual ~CHullComponent() = default;

	// IEntityComponent
	virtual void Initialize() override;

	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CHullComponent>& desc)
	{
		desc.SetGUID("{002A5F4B-BE6C-498A-9347-ACBB2AAA3E9B}"_cry_guid);
	}

	void Revive();

protected:

};

