#include "StdAfx.h"
#include "Player.h"
#include "Bullet.h"
#include "SpawnPoint.h"
#include "GamePlugin.h"

#include <CryRenderer/IRenderAuxGeom.h>
#include <CryInput/IHardwareMouse.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CryNetwork/Rmi.h>




namespace
{
	static void RegisterPlayerComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerComponent);
}

//----------------------------------------------------------------------------------

void CPlayerComponent::Initialize()
{
	// Mark the entity to be replicated over the network
	m_pEntity->GetNetEntity()->BindToNetwork();
	
	 //ITerrain* terrain = gEnv->p3DEngine->GetITerrain();
	 
	// Register the RemoteReviveOnClient function as a Remote Method Invocation (RMI) that can be executed by the server on clients
	SRmi<RMI_WRAP(&CPlayerComponent::RemoteReviveOnClient)>::Register(this, eRAT_NoAttach, false, eNRT_ReliableOrdered);


	//m_pEntity->LoadGeometry(GetOrMakeEntitySlotId(), "%ENGINE%/EngineAssets/Objects/primitive_sphere.cgf");

	cameraMode = ECameraMode::FreeCam;

	Vec3 debugSpawn = Vec3(512, 512, 50);

	waterLevel = 52;//gEnv->p3DEngine->GetWaterLevel();
	goalTransform = IDENTITY;
		
	//CryLog("f%", waterLevel);
	//CryLog(ToString(waterLevel));
	Vec3 p = debugSpawn;//GetEntity()->GetWorldPos();
	p.z = waterLevel;

	goalTransform.SetTranslation(p);
	curTransform = goalTransform;
	/*
	SEntityPhysicalizeParams physParams;
	physParams.mass = 0;
	physParams.type = PE_RIGID;

	pe_action_add_constraint aac;
	
	m_pEntity->Physicalize(physParams);
	*/

}

//----------------------------------------------------------------------------------

void CPlayerComponent::InitializeLocalPlayer()
{
	
	m_pCameraComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCameraComponent>();
	
	m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();


	
	//BuildBoatAttachments();
	BindInputs();
	

}

//----------------------------------------------------------------------------------

Cry::Entity::EventFlags CPlayerComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::BecomeLocalPlayer |
		Cry::Entity::EEvent::Update |
		Cry::Entity::EEvent::Reset;
}

//----------------------------------------------------------------------------------

void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	
	case Cry::Entity::EEvent::Reset:
	{

	}
	break;
	
	case Cry::Entity::EEvent::BecomeLocalPlayer:
	{
		InitializeLocalPlayer();
	}
	break;

	case Cry::Entity::EEvent::Update:
	{
		const float frameTime = event.fParam[0];

		//UpdateLookDirectionRequest(frameTime);

		if (IsLocalClient())
		{
			// Update the camera component offset
			UpdateCamera(frameTime);
			PanCamera(frameTime);

			

			//if (pRudderEntity);
				//pRudderEntity->GetComponent<CRudderComponent>()->isSubmerged = (pRudderEntity->GetWorldPos().z < tInfo.oceanWaterLevel);
		}
	}
	break;
	}
}

//----------------------------------------------------------------------------------

void CPlayerComponent::OnReadyForGameplayOnServer()
{
	CRY_ASSERT(gEnv->bServer, "This function should only be called on the server!");
	
	const Matrix34 newTransform = CSpawnPointComponent::GetFirstSpawnPointTransform();
	
	Revive(newTransform);
	
	// Invoke the RemoteReviveOnClient function on all remote clients, to ensure that Revive is called across the network
	SRmi<RMI_WRAP(&CPlayerComponent::RemoteReviveOnClient)>::InvokeOnOtherClients(this, RemoteReviveParams{ newTransform.GetTranslation(), Quat(newTransform) });
	
	// Go through all other players, and send the RemoteReviveOnClient on their instances to the new player that is ready for gameplay
	const int channelId = m_pEntity->GetNetEntity()->GetChannelId();
	CGamePlugin::GetInstance()->IterateOverPlayers([this, channelId](CPlayerComponent& player)
	{
		// Don't send the event for the player itself (handled in the RemoteReviveOnClient event above sent to all clients)
		if (player.GetEntityId() == GetEntityId())
			return;

		// Only send the Revive event to players that have already respawned on the server
	//	if (!player.m_isAlive)
			//return;

		// Revive this player on the new player's machine, on the location the existing player was currently at
		const QuatT currentOrientation = QuatT(player.GetEntity()->GetWorldTM());
		SRmi<RMI_WRAP(&CPlayerComponent::RemoteReviveOnClient)>::InvokeOnClient(&player, RemoteReviveParams{ currentOrientation.t, currentOrientation.q }, channelId);
	});
}

//----------------------------------------------------------------------------------

bool CPlayerComponent::RemoteReviveOnClient(RemoteReviveParams&& params, INetChannel* pNetChannel)
{
	// Call the Revive function on this client
	Revive(Matrix34::Create(Vec3(1.f), params.rotation, params.position));

	return true;
}

//----------------------------------------------------------------------------------

void CPlayerComponent::SetCameraMode()
{
	bool distVessel = (m_ViewDistance <= 60);
	bool distCruise = (m_ViewDistance > 120);
	bool distMap = (m_ViewDistance > 180);

	allowedToYaw = allowedToPitch = allowedToMouseButtonYaw = allowedToPan = false;
	
	if (IsPilotingVessel()) {
		if (distVessel)
			cameraMode = ECameraMode::Vessel;

		if (distCruise)
			cameraMode = ECameraMode::VesselCruise;

		HideCursor();
	}

	if (distMap) 
		cameraMode = ECameraMode::FreeCam;

	

	switch (cameraMode) 
	{
	case ECameraMode::Vessel:
	{
		CryLog("Vessel");

		allowedToPitch = allowedToYaw = true;
		HideCursor();

	}
	break;

	case ECameraMode::VesselCruise:
	{
		CryLog("Cruise");

		//allowedToMouseButtonYaw = allowedToPitch = allowedToPan = false;
		allowedToYaw = true;
		HideCursor();

	}
	break;

	case ECameraMode::FreeCam:
	{
		CryLog("FreeCam");
		//allowed to pan, mouse yaw

		//not allowed to yaw, pitch
		//allowedToYaw = allowedToPitch = false;
		allowedToMouseButtonYaw = allowedToPan = true;

		AbandonVessel(pPilotedVessel);
		ShowCursor();
	}
	break;

	}

}

//----------------------------------------------------------------------------------

void CPlayerComponent::PanCamera(float frameTime)
{
	Vec3 dir = ZERO;
	Vec3 vesselDir = ZERO;


	if (m_inputFlags & EInputFlag::MoveLeft) {
		dir -= m_lookOrientation.GetColumn0();
		vesselDir.x -= 0.2f;
	}

	if (m_inputFlags & EInputFlag::MoveRight) {
		dir += m_lookOrientation.GetColumn0();
		vesselDir.x += 0.2f;
	}

	if (m_inputFlags & EInputFlag::MoveForward) {
		dir += m_lookOrientation.GetColumn2();
		vesselDir.y += 1;
	}

	if (m_inputFlags & EInputFlag::MoveBack) {
		dir -= m_lookOrientation.GetColumn2();
		vesselDir.y -= 1;
	}
	/*
	else {

		Vec3 r = Vec3(m_mouseDeltaRotation);
		dir += ((m_lookOrientation * r) * frameTime);
		dir.x = -dir.x;

	}


	/*
	//Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
	//Vec3 lookDir = Vec3(ypr.x, ypr.z, 0);

	//dir *= lookDir;

	pe_action_impulse impulseAction;
	impulseAction.impulse = dir * 100;
	if (!dir.IsZero()) {
		if (IPhysicalEntity* pEnt = GetEntity()->GetPhysics())
		{
			CryLog("impulse");
			pEnt->Action(&impulseAction);
		}
		else CryLog("moving, but no physics");

	}

	*/
	

	dir.z = 0;

	m_throttle = vesselDir.y;
	m_turn = vesselDir.x;

	if (IsPilotingVessel()) {

		if (!vesselDir.IsZero()) {
			Throttle();
		}
	}

	if (allowedToPan) {
		goalTransform.AddTranslation(dir * 2);
	}
}

//----------------------------------------------------------------------------------

void CPlayerComponent::ShowCursor()
{
	gEnv->pInput->ShowCursor(true);
}

//----------------------------------------------------------------------------------

void CPlayerComponent::HideCursor()
{
	gEnv->pInput->ShowCursor(false);
}

//----------------------------------------------------------------------------------

IEntity* CPlayerComponent::GetEntityFromPointer()
{

	float mouseX, mouseY;
	gEnv->pHardwareMouse->GetHardwareMouseClientPosition(&mouseX, &mouseY);

	// Invert mouse Y
	mouseY = gEnv->pRenderer->GetHeight() - mouseY;

	Vec3 vPos0(0, 0, 0);
	gEnv->pRenderer->UnProjectFromScreen(mouseX, mouseY, 0, &vPos0.x, &vPos0.y, &vPos0.z);

	Vec3 vPos1(0, 0, 0);
	gEnv->pRenderer->UnProjectFromScreen(mouseX, mouseY, 1, &vPos1.x, &vPos1.y, &vPos1.z);

	Vec3 vDir = vPos1 - vPos0;
	vDir.Normalize();

	const unsigned int rayFlags = rwi_stop_at_pierceable | rwi_colltype_any;
	ray_hit hit;

	int hits = gEnv->pPhysicalWorld->RayWorldIntersection(vPos0, vDir * gEnv->p3DEngine->GetMaxViewDistance(), ent_all, rayFlags, &hit, 1);

	if (hits > 0) {
		return gEnv->pEntitySystem->GetEntityFromPhysics(hit.pCollider);
	}
	else
	return nullptr;
}

//----------------------------------------------------------------------------------

void CPlayerComponent::PilotVessel(IEntity* vessel)
{
	pPilotedVessel = vessel;
	//CryLog(ToString(vessel));

	TargetPilotedVessel();


}

//----------------------------------------------------------------------------------

void CPlayerComponent::AbandonVessel(IEntity* vessel)
{
	pPilotedVessel = nullptr;
}

//----------------------------------------------------------------------------------

void CPlayerComponent::TargetPilotedVessel()
{
	m_ViewDistance = 30;
	SetCameraMode();

}

//----------------------------------------------------------------------------------

void CPlayerComponent::SelectVessel(CVessel* vessel)
{

	vessel->OnClick(GetEntityId());
}

//----------------------------------------------------------------------------------

void CPlayerComponent::Throttle()
{
	if (pPilotedVessel) {
		if (CVessel* pEnt = pPilotedVessel->GetComponent<CVessel>()) {

			if (pEnt->GetEngine())
				pEnt->SetThrottle(m_throttle);
			else
				CryLog("Engine broken" + ToString(pEnt->GetEngine()));

			if (pEnt->GetRudder())
				pEnt->SetRudderAngle(m_turn);
			else
				CryLog("Rudder broken" + ToString(pEnt->GetRudder()));

			

		}
	}
}

//----------------------------------------------------------------------------------

void CPlayerComponent::Revive(const Matrix34& transform)
{

	// Set the entity transformation, except if we are in the editor
	// In the editor case we always prefer to spawn where the viewport is
	if (!gEnv->IsEditor())
	{
		//m_pEntity->SetWorldTM(transform);
	}
	


	m_inputFlags.Clear();
	NetMarkAspectsDirty(InputAspect);

	m_mouseDeltaRotation = ZERO;
	m_mouseDeltaSmoothingFilter.Reset();

	m_lookOrientation = IDENTITY;
	m_horizontalAngularVelocity = 0.0f;
	m_averagedHorizontalAngularVelocity.Reset();

	SetCameraMode();

}

//----------------------------------------------------------------------------------

void CPlayerComponent::BindInputs()
{
	m_pInputComponent->RegisterAction("player", "moveleft", [this](int activationMode, float value) { 
HandleInputFlagChange(EInputFlag::MoveLeft, (EActionActivationMode)activationMode); 

 });
	m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, EKeyId::eKI_A);

	m_pInputComponent->RegisterAction("player", "moveright", [this](int activationMode, float value) {
 HandleInputFlagChange(EInputFlag::MoveRight, (EActionActivationMode)activationMode); 

 });
	m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, EKeyId::eKI_D);

	m_pInputComponent->RegisterAction("player", "moveforward", [this](int activationMode, float value) { 
		HandleInputFlagChange(EInputFlag::MoveForward, (EActionActivationMode)activationMode);  

});
	m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, EKeyId::eKI_W);

	m_pInputComponent->RegisterAction("player", "moveback", [this](int activationMode, float value) { 
		HandleInputFlagChange(EInputFlag::MoveBack, (EActionActivationMode)activationMode);  

	});
	m_pInputComponent->BindAction("player", "moveback", eAID_KeyboardMouse, EKeyId::eKI_S);


	m_pInputComponent->RegisterAction("player", "mouse_rotateyaw", [this](int activationMode, float value) { m_mouseDeltaRotation.x -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotateyaw", eAID_KeyboardMouse, EKeyId::eKI_MouseX);

	m_pInputComponent->RegisterAction("player", "mouse_rotatepitch", [this](int activationMode, float value) { m_mouseDeltaRotation.y -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotatepitch", eAID_KeyboardMouse, EKeyId::eKI_MouseY);

	m_pInputComponent->RegisterAction("player", "mouse_scrolldown", [this](int activationMode, float value) { scrollValue += 1; SetCameraMode(); });
	m_pInputComponent->BindAction("player", "mouse_scrolldown", eAID_KeyboardMouse, EKeyId::eKI_MouseWheelDown);

	m_pInputComponent->RegisterAction("player", "mouse_scrollup", [this](int activationMode, float value) {scrollValue -= 1; SetCameraMode(); });
	m_pInputComponent->BindAction("player", "mouse_scrollup", eAID_KeyboardMouse, EKeyId::eKI_MouseWheelUp);

	m_pInputComponent->RegisterAction("player", "action", [this](int activationMode, float value)
	{


	});

	m_pInputComponent->BindAction("player", "action", eAID_KeyboardMouse, EKeyId::eKI_F);
	
	m_pInputComponent->RegisterAction("player", "pancam", [this](int activationMode, float value) 
	{
		
		SetCameraMode();
		
		if (allowedToMouseButtonYaw) {
			if (activationMode == eAAM_OnPress) {
				isMouseYawing = true;
				HideCursor();
			}

			if (activationMode == eAAM_OnRelease) {
				isMouseYawing = false;
				ShowCursor();
			}

			
			
		}
	});
	m_pInputComponent->BindAction("player", "pancam", eAID_KeyboardMouse, EKeyId::eKI_Mouse2);

	
	/*
	IActionMapManager* pActionMapManager = gEnv->pGameFramework->GetIActionMapManager();
	IActionMap* pActionMap = pActionMapManager->GetActionMap("player");
	const SActionInput* actionInput;
	IActionMapAction* pAction = pActionMap->GetAction(ActionId("action"));
	actionInput = pActionMapManager->GetActionInput(pActionMap->GetName(),pAction->GetActionId(), eAID_KeyboardMouse,0);
	*/
	
	
	m_pInputComponent->RegisterAction("player", "select", [this](int activationMode, float value)
	{
		// Only fire on press, not release
		if (activationMode == eAAM_OnPress)
		{
			CryLog("Clicked");
			if (IEntity* pEntity = GetEntityFromPointer()) {
				
				//CryLog(pEntity->GetName());
				if (auto* pVessel = pEntity->GetComponent<CVessel>()) {
					
					SelectVessel(pVessel);

				}
			}
		}
	});

	m_pInputComponent->BindAction("player", "select", eAID_KeyboardMouse, EKeyId::eKI_Mouse1);
	// Register the shoot action
	//m_pInputComponent->RegisterAction("player", "shoot", [this](int activationMode, float value));
	//m_pInputComponent->BindAction("player", "shoot", eAID_KeyboardMouse, EKeyId::eKI_Space);

	CryLog("Inputs bound");

}

//----------------------------------------------------------------------------------

void CPlayerComponent::UpdateCamera(float frameTime)
{
	if (isMouseYawing) {
		allowedToYaw = true;//allowedToPitch  = false;

	}

	// Start with updating look orientation from the latest input
	Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));

	if (!m_mouseDeltaRotation.IsZero())
	{
		const float rotationSpeed = 0.002f;
		if (allowedToYaw) {
			ypr.x += m_mouseDeltaRotation.x * rotationSpeed;
					
		}

	

		const float rotationLimitsMinPitch = -1.2;
		const float rotationLimitsMaxPitch = 0.4f;
		

		if (allowedToPitch) {
			// TODO: Perform soft clamp here instead of hard wall, should reduce rot speed in this direction when close to limit.
			ypr.y = CLAMP(ypr.y + m_mouseDeltaRotation.y * rotationSpeed, rotationLimitsMinPitch, rotationLimitsMaxPitch);
			
		}
		else
		{
			//auto r = Matrix33::CreateRotationXYZ(Ang3(ypr.x, ypr.y, DEG2RAD(-45)));

			ypr.y = DEG2RAD(-45);
			//Vec3 d = pPilotedVessel->GetWorldPos() - m_pCameraComponent->GetTransformMatrix().GetTranslation();
			//ypr.y = d.y;
		}

		//	ypr.y = CLAMP(ypr.y + m_mouseDeltaRotation.y * rotationSpeed, rotationLimitsMinPitch, rotationLimitsMaxPitch);
			// Look direction needs to be synced to server to calculate the movement in
			// the right direction.
		ypr.z = 0;

		m_lookOrientation = Quat(CCamera::CreateOrientationYPR(ypr));
		NetMarkAspectsDirty(InputAspect);

		// Reset every frame
		m_mouseDeltaRotation = ZERO;


	}

	// Start with changing view rotation to the requested mouse look orientation
	Matrix34 localTransform = IDENTITY;

	localTransform.SetRotation33(Matrix33(m_pEntity->GetWorldRotation().GetInverted()) * CCamera::CreateOrientationYPR(ypr));

	//target the vessel
	if (IsPilotingVessel()) {
		Vec3 p = pPilotedVessel->GetWorldPos();
		p.z += 4;
			// p.z = waterLevel;
		goalTransform.SetTranslation(p);

	}

	float fd = 32;
	fd += scrollValue;
	m_ViewDistance = fd;

	// Offset the player along the forward axis (normally back)
	// Also offset upwards

	localTransform.SetTranslation(-localTransform.GetColumn1() * m_ViewDistance);

	m_pCameraComponent->SetTransformMatrix(localTransform);
	//m_pAudioListenerComponent->SetOffset(localTransform.GetTranslation());

	curTransform.SetRotation33(Matrix33(m_lookOrientation));
	if (IsPilotingVessel()) 
	{
		//curTransform = Lerp(curTransform, goalTransform, frameTime);
	}
	else 
	{
		
	}

	Vec3 curVec = curTransform.GetTranslation();
	curVec = Lerp(curTransform.GetTranslation(), goalTransform.GetTranslation(), frameTime);
	curTransform.SetTranslation(curVec);

	GetEntity()->SetWorldTM(curTransform);

}

//----------------------------------------------------------------------------------

/*
void CPlayerComponent::BuildBoatAttachments()
{
	if (IAttachmentManager* pAttachmentManager = GetEntity()->GetCharacter(0)->GetIAttachmentManager()) 
	{
		SEntitySpawnParams spawnParams;

		spawnParams.pParent = GetEntity();
		spawnParams.vScale = Vec3(1);
		spawnParams.qRotation = IDENTITY;

		spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();

		IEntityClass* pRudderClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("CRudderComponent");
		IEntityClass* pEngineClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("CEngineComponent");


		//existence insurance
		if (!pRudderClass || !pEngineClass) {
			CryLog("No class found");
			return;
		}

		if (IAttachment* pRudderAttachment = pAttachmentManager->GetInterfaceByName("Rudder"))
		{
			if (pRudderEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams)) {
				CEntityAttachment* pEntAttach = new CEntityAttachment();

				m_pRudderComponent = pRudderEntity->GetOrCreateComponentClass<CRudderComponent>();

				pEntAttach->SetEntityId(pRudderEntity->GetId());
				pRudderAttachment->AddBinding(pEntAttach);

				CryLog("Initialized Rudder");
			}
		}

		if (IAttachment* pEngineAttachment = pAttachmentManager->GetInterfaceByName("Engine"))
		{
			if (pEngineEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams)) {
				CEntityAttachment* pEntAttach = new CEntityAttachment();

				m_pEngineComponent = pEngineEntity->GetOrCreateComponentClass<CEngineComponent>();

				pEntAttach->SetEntityId(pEngineEntity->GetId());
				pEngineAttachment->AddBinding(pEntAttach);


				CryLog("Initialized Engine");
			}
		}
	}
}
*/

//----------------------------------------------------------------------------------

void CPlayerComponent::HandleInputFlagChange(const CEnumFlags<EInputFlag> flags, const CEnumFlags<EActionActivationMode> activationMode, const EInputFlagType type)
{
	switch (type)
	{
	case EInputFlagType::Hold:
	{
		if (activationMode == eAAM_OnRelease)
		{
			m_inputFlags &= ~flags;
		}
		else
		{
			m_inputFlags |= flags;
		}
	}
	break;
	case EInputFlagType::Toggle:
	{
		if (activationMode == eAAM_OnRelease)
		{
			// Toggle the bit(s)
			m_inputFlags ^= flags;
		}
	}
	break;
	}

	// Input is replicated from the client to the server.
	if (IsLocalClient())
	{
		NetMarkAspectsDirty(InputAspect);
	}
}