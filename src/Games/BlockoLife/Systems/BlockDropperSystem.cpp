#include "BlockDropperSystem.h"

#include "SDLRenderManager.h"
#include "World.h"
#include "WorldCoords.h"
#include "BlockoLifeWorldBuilder.h"

#include "KBInputComponent.h"
#include "TransformComponent.h"
#include "PadInputComponent.h"
#include "DebugTextComponent.h"
#include "GridWorldComponent.h"
#include "GridTransformComponent.h"

#include "Components/BlockDropperComponent.h"


void BlockDropperSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);

	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "BlockDropperSystem Init MISSING render manager!");

	EntitySignature renderSignature;
	renderSignature.set((size_t)ComponentType::CT_GRIDTRANSFORM);
	renderSignature.set((size_t)ComponentType::CT_BL_DROPPER);
	m_ParentWorld->SetSystemSignatures<BlockDropperSystem>(renderSignature);
}

void BlockDropperSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	for (EntityID e : GetEntities())
	{
		auto& kbInput = m_ParentWorld->GetComponent<KBInputComponent>(e);
		auto& padInput = m_ParentWorld->GetComponent<PadInputComponent>(e);
		auto& dropTransform = m_ParentWorld->GetComponent<GridTransformComponent>(e);
		auto& dropper = m_ParentWorld->GetComponent<BlockDropperComponent>(e);

		// Move cursor
		Vector2i delta = Vector2i::ZERO;

		if (kbInput.WasJustPressed(KB_KEY::KEY_UP)) delta.y -= 1;
		if (kbInput.WasJustPressed(KB_KEY::KEY_DOWN)) delta.y += 1;
		if (kbInput.WasJustPressed(KB_KEY::KEY_LEFT)) delta.x -= 1;
		if (kbInput.WasJustPressed(KB_KEY::KEY_RIGHT)) delta.x += 1;

		if (delta == Vector2i::ZERO)
		{
			if (padInput.WasJustPressed(GAMEPAD_BTN::BTN_DPAD_UP)) delta.y -= 1;
			if (padInput.WasJustPressed(GAMEPAD_BTN::BTN_DPAD_DOWN)) delta.y += 1;
			if (padInput.WasJustPressed(GAMEPAD_BTN::BTN_DPAD_LEFT)) delta.x -= 1;
			if (padInput.WasJustPressed(GAMEPAD_BTN::BTN_DPAD_RIGHT)) delta.x += 1;
		}

		dropTransform.m_Pos += delta;

		// Drop a block if button clicked
		// TODO_SPAWNING needs refactor - probably an EVENT SYSTEM? Submit spawn requests from anywhere to one place,
		// all of them processed and checked against each other by one system like GameOfLifeSystem
		if (kbInput.IsPressed(KB_KEY::KEY_SPACE) || padInput.IsPressed(GAMEPAD_BTN::BTN_FACE_A))
		{
			bool spaceOccupied = false;

			EntitySignature querySignature;
			querySignature.set((size_t)ComponentType::CT_GRIDTRANSFORM);
			querySignature.set((size_t)ComponentType::CT_BL_SPECIES);
			EntityQuery q (querySignature);
			std::set<EntityID> creatures;

			m_ParentWorld->ExecuteQuery(q, creatures);
			for (EntityID e : creatures)
			{
				auto& checkTransform = m_ParentWorld->GetComponent<GridTransformComponent>(e);
				auto& species = m_ParentWorld->GetComponent<SpeciesComponent>(e);
				if (dropTransform.m_Pos == checkTransform.m_Pos)
				{
					assert(species.m_Species < Species::SPECIES_COUNT);
					spaceOccupied = true;
				}
			}

			if (!spaceOccupied)
			{
				BlockoLifeWorldBuilder::BuildEntity(*m_ParentWorld, dropper.m_Species, dropTransform.m_Pos);
			}
		}
	}
}

void BlockDropperSystem::Render_Main()
{
	auto& gridWorld = m_ParentWorld->GetGlobalComponent<GridWorldComponent>();

	for (EntityID e : GetEntities())
	{
		auto& transform = m_ParentWorld->GetComponent<GridTransformComponent>(e);

		// TODO support for thicker selection box, or transparent graphic?
		Vector2i screenPos = WorldCoords::GridToScreen(*m_ParentWorld, transform.m_Pos);
		const int width = gridWorld.m_GridSize;
		const Vector2i topLeft { screenPos.x, screenPos.y };
		const Vector2i bottomRight { screenPos.x+gridWorld.m_GridSize, screenPos.y+gridWorld.m_GridSize };
		const Rect2i drawBox { topLeft, bottomRight };

		m_RenderMan->DrawRect(drawBox, BLUE);
	}
}
