#pragma once

#include <cstdint>
#include <array>

#include <PxPhysicsAPI.h>

#include <mem/Everything.h>
#include <mem/Global.h>

#include <misc/Misc.h>
#include <misc/Log.h>

#include "Renderer.h"

using namespace physx;

struct FilterCallbackHandler : public physx::PxSimulationEventCallback
{
	// Inherited via PxSimulationEventCallback
	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {
	}
	virtual void onWake(PxActor** actors, PxU32 count) override {
	}
	virtual void onSleep(PxActor** actors, PxU32 count) override {
	}
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override {
		WeakObject obj{
			.index = std::bit_cast<Index<Everything>>(pairHeader.actors[0]->userData),
			.proxy = *Global<Everything*>
		};

		if (obj.isNotNull() && obj.has<game::Player>()) {
			obj.get<game::Player>().onGround = true;
		}

		std::cout << "contact or something\n";
	}
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override {
	}
	virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override {
	}
};

class MyAllocator : public physx::PxDefaultAllocator
{
public:
	MyAllocator() = default;
	virtual ~MyAllocator() = default;
	virtual void* allocate(size_t size, const char* typeName, const char* filename,
		int line) override {
		return std::malloc(size);

	};
	virtual void deallocate(void* ptr) override {
		std::free(ptr);
	};
};

class MyErrorCallback : public physx::PxErrorCallback
{
public:
	MyErrorCallback() = default;
	~MyErrorCallback() = default;

	virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) {
		Global<misc::Log>->putLine(std::format("PhysX error in file {}:{} -- {}\n", file, line, message));
	};
};

namespace game
{
	struct GameState
	{
		physx::PxFoundation* foundation;
		physx::PxPhysics* physics;
		physx::PxScene* scene;
		physx::PxCpuDispatcher* cpuDispatcher;
		physx::PxMaterial* material;
		physx::PxMaterial* playerMaterial;
		physx::PxDefaultAllocator allocator{};
		physx::PxDefaultErrorCallback errorCallback{};

		FilterCallbackHandler handler{};

		void shootProjectile(glm::vec3 dir, float speed);

		QualifiedObject player;

		Everything everything;
		int32_t tick = 0;

		void runTick();

		void addRenderInfo(RenderInfo& renderInfo);

		void init();

		GameState() = default;
		~GameState() = default;

		DEFAULT_MOVE(GameState);
		NO_COPY(GameState);
	};
}