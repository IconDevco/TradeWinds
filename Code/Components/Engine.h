#pragma once
#include "CryEntitySystem/IEntityComponent.h"
#include "DefaultComponents/Geometry/AdvancedAnimationComponent.h"


//class CVessel;
class CEngineComponent final : public IEntityComponent {
public:


	CEngineComponent() = default;
	virtual ~CEngineComponent() = default;

	// IEntityComponent
	virtual void Initialize() override;

	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CEngineComponent>& desc)
	{
		desc.SetComponentFlags({ IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::Transform });
		desc.SetGUID("{4A0B3A39-13C1-4BFD-BB30-A279110D70E7}"_cry_guid);
	}

public:
	void Accelerate(float fTime);
	
	//is supposed to be 0 - 1
	void Throttle();

	void SetThrottle(float f) { m_throttle = f; }
	void SetRudderAngle(float a) { rudderAngle = a; }
	//void SetParent(CVessel* vessel) { pParentVessel = vessel; }
protected:
	
	float m_throttle;
	float horsepower = 10;
	float rudderAngle = 0;

	//Vec3 AttachmentPosition = Vec3(0);

	//CVessel* pParentVessel = nullptr;

};
