#pragma once
#include "StdAfx.h"
#include "CryEntitySystem/IEntityComponent.h"

class IInteractiveInterface : public IEntityComponent{


public:

	IInteractiveInterface() = default;
	virtual ~IInteractiveInterface() {};


	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<IInteractiveInterface>& desc)
	{
		desc.SetGUID("{3C1A944D-B619-4F52-9D75-51BF1F190000}"_cry_guid);
	}

	

public:
	enum class ESelectionFlags {
		Vessel,
		Port,
		Fleet

	};

	ESelectionFlags selectionFlags;

public:
	//template <typename T>
	
	//virtual void OnClick(EntityId playerId, uint32 cameraMode);
	//void SetInteractiveEntity(IEntity* e) { m_pInteractiveEntity = e; }

	//virtual void OnClick(CPlayerComponent* localPlayer);

protected:
	
	

};