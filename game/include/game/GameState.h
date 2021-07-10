#pragma once

#include <cstdint>
#include <array>

#include <PxPhysicsAPI.h>

#include <mem/Everything.h>
#include <mem/Global.h>

#include <misc/Misc.h>
#include <misc/Log.h>

#include "Renderer.h"


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
		physx::PxDefaultAllocator allocator{};
		physx::PxDefaultErrorCallback errorCallback{};
		//MyAllocator allocator{};
		//MyErrorCallback errorCallback{};

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