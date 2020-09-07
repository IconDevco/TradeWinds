#pragma once
#include "vessel/Vessel.h"
#include <array>
#include <numeric>

#include <CryEntitySystem/IEntityComponent.h>
#include <CryMath/Cry_Camera.h>

#include <ICryMannequin.h>
#include <CrySchematyc/Utils/EnumFlags.h>

#include <DefaultComponents/Cameras/CameraComponent.h>
#include "DefaultComponents/Geometry/AdvancedAnimationComponent.h"
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Audio/ListenerComponent.h>
#include "DefaultComponents/Physics/CharacterControllerComponent.h"
#include <CryAISystem/Components/IEntityNavigationComponent.h>



#include "CryInput/IInput.h"
#include "CryAction/IActionMapManager.h"


////////////////////////////////////////////////////////
// Represents a player participating in gameplay
////////////////////////////////////////////////////////
class CPlayerComponent final : public IEntityComponent
{

	enum class EInputFlagType
	{
		Hold = 0,
		Toggle
	};

	enum class EInputFlag : uint8
	{
		MoveLeft = 1 << 0,
		MoveRight = 1 << 1,
		MoveForward = 1 << 2,
		MoveBack = 1 << 3
	};

	static constexpr EEntityAspects InputAspect = eEA_GameClientD;

	template<typename T, size_t SAMPLES_COUNT>
	class MovingAverage
	{
		static_assert(SAMPLES_COUNT > 0, "SAMPLES_COUNT shall be larger than zero!");

	public:

		MovingAverage()
			: m_values()
			, m_cursor(SAMPLES_COUNT)
			, m_accumulator()
		{
		}

		MovingAverage& Push(const T& value)
		{
			if (m_cursor == SAMPLES_COUNT)
			{
				m_values.fill(value);
				m_cursor = 0;
				m_accumulator = std::accumulate(m_values.begin(), m_values.end(), T(0));
			}
			else
			{
				m_accumulator -= m_values[m_cursor];
				m_values[m_cursor] = value;
				m_accumulator += m_values[m_cursor];
				m_cursor = (m_cursor + 1) % SAMPLES_COUNT;
			}

			return *this;
		}

		T Get() const
		{
			return m_accumulator / T(SAMPLES_COUNT);
		}

		void Reset()
		{
			m_cursor = SAMPLES_COUNT;
		}

	private:

		std::array<T, SAMPLES_COUNT> m_values;
		size_t m_cursor;

		T m_accumulator;
	};



public:
	CPlayerComponent() = default;
	virtual ~CPlayerComponent() = default;

	// IEntityComponent
	virtual void Initialize() override;

	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
	{
		desc.SetGUID("{63F4C0C6-32AF-4ACB-8FB0-57D45DD14725}"_cry_guid);
	}

	void OnReadyForGameplayOnServer();
	bool IsLocalClient() const { return (m_pEntity->GetFlags() & ENTITY_FLAG_LOCAL_PLAYER) != 0; }

	void Revive(const Matrix34& transform);

protected:
	void HandleInputFlagChange(CEnumFlags<EInputFlag> flags, CEnumFlags<EActionActivationMode> activationMode, EInputFlagType type = EInputFlagType::Hold);

	void BindInputs();

	//void BuildBoatAttachments();
	void UpdateCamera(float frameTime);
	//void UpdateLookDirectionRequest(float frameTime);

	// Called when this entity becomes the local player, to create client specific setup such as the Camera
	void InitializeLocalPlayer();

	// Start remote method declarations
protected:

	// Parameters to be passed to the RemoteReviveOnClient function
	struct RemoteReviveParams
	{
		// Called once on the server to serialize data to the other clients
		// Then called once on the other side to deserialize
		void SerializeWith(TSerialize ser)
		{
			// Serialize the position with the 'wrld' compression policy
			ser.Value("pos", position, 'wrld');
			// Serialize the rotation with the 'ori0' compression policy
			ser.Value("rot", rotation, 'ori0');
		}

		Vec3 position;
		Quat rotation;
	};
	// Remote method intended to be called on all remote clients when a player spawns on the server
	bool RemoteReviveOnClient(RemoteReviveParams&& params, INetChannel* pNetChannel);



protected:


	float waterLevel = 0;
	Cry::DefaultComponents::CCameraComponent* m_pCameraComponent = nullptr;
	Cry::DefaultComponents::CInputComponent* m_pInputComponent = nullptr;


protected:
	CEnumFlags<EInputFlag> m_inputFlags;
	Vec2 m_mouseDeltaRotation;
	float scrollValue;
	MovingAverage<Vec2, 10> m_mouseDeltaSmoothingFilter;

	Quat m_lookOrientation; //!< Should translate to head orientation in the future
	float m_horizontalAngularVelocity = 0;
	MovingAverage<float, 10> m_averagedHorizontalAngularVelocity;


	////////////////////// MODE /////////////////////////////
public:

	enum class ECameraMode {
		//This represents the selection mode

		//concept of a camera that can pan and zoom within the bounds of a certain area
		//usually outside areas that require selection interfaces... Port stuff
		Port,
		Shipyard,

		//the only camera that should leave godspace. put yourself into your biggest boats cockpit. this sounds hard
		FlagshipCabin,

		//pan and zoom mode for selecting vessels as part of the fleet. concept requires selection interface
		Fleet,

		//is commanding a vessel. camera pan snaps back to target. normal zoom allowance, no selection interface
		Vessel,

		//cruising cameras are zoom only camera. Has map like zoom, but only follows a vessel in question
		//has an allowed range of pan, but pan snaps back to target
		VesselCruise,
		FleetCruise,

		//concept of a free camera. Pan and zoom not bound to any object
		//only zoom in so far, world has clouds, and things outside of the range of any fleet are not updated or hidden.
		//selection interface allows ports and vessels. 
		FreeCam = 0,

		//this camera makes sure the player's first engaged fleet and the opposing fleet are in view,
		//with focus on the player's flagship
		CombatCamera

		//TODO:
		//create new port status for if a vessel of a faction is docked

	};
	ECameraMode cameraMode;

	void SetCameraMode();

	////////////////////// CAMERA /////////////////////////////

	void PanCamera(float frameTime);

	bool isCursorShown;
	void ShowCursor();
	void HideCursor();

	bool allowedToPan;
	bool allowedToMouseButtonYaw;
	bool allowedToPitch;
	bool allowedToYaw;

	//bool allowedToMousePan;

	//Is holding down pan mouse button
	bool isMouseYawing;

	Matrix34 curTransform;
	Matrix34 goalTransform;

	//Goal
	float m_ViewDistance = 30;


	float m_ScrollSpeed;
	float m_PanSpeed;


	////////////////////// VESSEL /////////////////////////////
public:
	void PilotVessel(IEntity* vessel);
	void AbandonVessel(IEntity* vessel);
	void TargetPilotedVessel();

	bool IsPilotingVessel() const { return (pPilotedVessel != nullptr); }

protected:

	IEntity* pPilotedVessel;
	
	void Throttle();

	float m_throttle;
	float m_turn;

	//bool IsPilotingVessel() { return (pPilotedVessel != nullptr); }

	////////////////////// SELECTION /////////////////////////////
public:

	IEntity* GetEntityFromPointer();
	void SelectVessel(CVessel* vessel);


	void BeginDragSelect(Vec3 begin) {
		AABB boxAABB;
		boxAABB.Add(begin);
	
	//	gEnv->pPhysicalWorld->GetEntitiesInBox(;
	}
	void EndDragSelect() {


	}
	//DynArray<IEntity> selectedEntities;




};
