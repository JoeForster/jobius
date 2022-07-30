#include "PhysicsSystem.h"

#include <string>
#include <format>

#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "DebugTextComponent.h"
#include "AABBComponent.h"
#include "GridWorldComponent.h"

#include "World.h"
#include "SDLRenderManager.h"
#include "Coordinates.h"

void PhysicsSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_TRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	m_ParentWorld->SetSystemSignature<PhysicsSystem>(sysSignature);
	sysSignature.set((size_t)ComponentType::CT_DEBUGTEXT);
	m_ParentWorld->SetSystemDebugSignature<PhysicsSystem>(sysSignature);

	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "PhysicsSystem Init MISSING render manager!");
}


void PhysicsSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	// Hack test motion - TODO simple integrate here use mass properly
	constexpr float gravityAccel = 0.f;//9.8f;
	constexpr float minVelocity = 0.01f;
	constexpr float terminalVelocity = 10.0f;
	static float damping = 1.0f;
	for (EntityID e : mEntities)
	{
		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		auto& rb = m_ParentWorld->GetComponent<RigidBodyComponent>(e);

		if (rb.m_Colliding)
		{
			rb.m_Vel = Vector2f::ZERO;
		}
		else
		{
			if (!rb.m_Kinematic)
			{
				float scaledDamping = powf(damping, deltaSecs);
				rb.m_Vel *= scaledDamping;
				rb.m_Vel.y += gravityAccel * deltaSecs;
			}
			rb.m_Vel.LimitMagnitude(minVelocity, terminalVelocity);
		}

		t.m_Pos.x += rb.m_Vel.x;
		t.m_Pos.y += rb.m_Vel.y;
	
		// HACK TEST until we have a floor: wrap around.
		const auto& bounds = m_ParentWorld->GetGlobalComponent<GridWorldComponent>().m_Bounds;
		if (t.m_Pos.y > bounds.max.y)
		{
			t.m_Pos.y = bounds.min.y;
		}
		else if (t.m_Pos.y < bounds.min.y)
		{
			t.m_Pos.y = bounds.max.y;
		}
	}
}

void PhysicsSystem::Render_Debug()
{
	for (EntityID e : mEntitiesDebug)
	{
		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		auto& rb = m_ParentWorld->GetComponent<RigidBodyComponent>(e);
		auto& dt = m_ParentWorld->GetComponent<DebugTextComponent>(e);
		
		auto& gridWorld = m_ParentWorld->GetGlobalComponent<GridWorldComponent>();
		const Vector2i gridCoords = gridWorld.WorldToGrid(t.m_Pos);

		m_RenderMan->DrawText(
			std::format("{:.2f}({}), {:.2f}({})", rb.m_Vel.x, gridCoords.x, rb.m_Vel.y, gridCoords.y).c_str(),
			dt.m_ResID, WorldToScreen(t.m_Pos));
	}
}
