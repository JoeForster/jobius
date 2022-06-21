#include "PhysicsSystem.h"

#include <sstream>

#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "AABBComponent.h"
#include "World.h"
#include "SDLRenderManager.h"

void PhysicsSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_TRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	m_ParentWorld->SetSystemSignature<PhysicsSystem>(sysSignature);

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
	static float damping = 0.7f;
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
		const auto& bounds = m_ParentWorld->GetBounds();
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

void PhysicsSystem::Render()
{
	System::Render();

	for (EntityID e : mEntities)
	{
		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		auto& rb = m_ParentWorld->GetComponent<RigidBodyComponent>(e);
		
		// TODO_DEBUG_DRAW here for now, but need a separate pass for debug.
		const ResourceID font = m_RenderMan->GetDefaultFont();
		
		// TODO tidy this, there has to be a better way of "find and update reference if it wasn't already in there"
		std::ostringstream ss;
		ss.precision(2);
		ss << rb.m_Vel.x << ", " << rb.m_Vel.y;

		EntityResMap::iterator existing = m_DebugText.find(e);
		if (existing == m_DebugText.end())
		{
			ResourceID text = SDLRenderManager::ResourceID_Invalid;
			if (m_RenderMan->PrepareText(ss.str().c_str(), font, text))
			{
				m_RenderMan->DrawText(text, t.m_Pos.x, t.m_Pos.y);
				m_DebugText[e] = text;
			}
		}
		else
		{
			ResourceID text = existing->second;
			if (m_RenderMan->PrepareText(ss.str().c_str(), font, text))
			{
				m_RenderMan->DrawText(text, t.m_Pos.x, t.m_Pos.y);
			}
		}
	}
}
