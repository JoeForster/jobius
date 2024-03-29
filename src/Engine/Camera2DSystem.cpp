#include "Camera2DSystem.h"

#include <format>
#include <string>

#include "KBInputComponent.h"
#include "Camera2DComponent.h"

#include "World.h"
#include "SDLRenderManager.h"

void Camera2DSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	// TODO pad control support
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_KBINPUT);
	//sysSignature.set((size_t)ComponentType::CT_PADINPUT);
	m_ParentWorld->SetSystemSignatures<Camera2DSystem>(sysSignature);


	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "Camera2DSystem Init MISSING render manager!");
}


void Camera2DSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	for (EntityID e : GetEntities())
	{
		auto& kbInput = m_ParentWorld->GetComponent<KBInputComponent>(e);
		//auto& padInput = m_ParentWorld->GetComponent<PadInputComponent>(e);

		constexpr float CAMERA_PAN_VEL = 400.0f;
		constexpr float CAMERA_ZOOM_VEL = 1.0f;
		
		auto& cameraComp = m_ParentWorld->GetGlobalComponent<Camera2DComponent>();
		
		if (kbInput.IsPressed(KB_KEY::KEY_RETURN))
		{
			cameraComp.m_CameraPos = Vector2f::ZERO;
			cameraComp.m_CameraZoom = 1.0f;
		}
		else
		{
			Vector2f desiredVel = Vector2f::ZERO;

			if (kbInput.IsPressed(KB_KEY::KEY_W)) desiredVel.y -= 1.0f;
			if (kbInput.IsPressed(KB_KEY::KEY_S)) desiredVel.y += 1.0f;
			if (kbInput.IsPressed(KB_KEY::KEY_A)) desiredVel.x -= 1.0f;
			if (kbInput.IsPressed(KB_KEY::KEY_D)) desiredVel.x += 1.0f;

			desiredVel *= CAMERA_PAN_VEL;
			desiredVel *= deltaSecs;

			cameraComp.m_CameraPos += desiredVel;

			float zoomDelta = 0.0f;

			if (kbInput.IsPressed(KB_KEY::KEY_Q)) zoomDelta += CAMERA_ZOOM_VEL;
			if (kbInput.IsPressed(KB_KEY::KEY_E)) zoomDelta -= CAMERA_ZOOM_VEL;

			cameraComp.m_CameraZoom += zoomDelta * deltaSecs;
		}

		static Vector2f prevPos = Vector2f::ZERO;
		if (prevPos != cameraComp.m_CameraPos)
		{
			//printf("Camera pos: (%f %f)\n", cameraComp.m_CameraPos.x, cameraComp.m_CameraPos.y);
			prevPos = cameraComp.m_CameraPos;
		}
	}
}

void Camera2DSystem::Render_Main()
{
	auto& cameraComp = m_ParentWorld->GetGlobalComponent<Camera2DComponent>();

	//m_RenderMan->SetOffset(cameraComp.m_CameraPos);
	m_RenderMan->SetZoom(cameraComp.m_CameraZoom);
}
