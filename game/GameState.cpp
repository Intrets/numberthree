#include "GameState.h"

#include <bit>

#include "Game.h"

#include <render/loaders/ModelLoader.h>

#include <PxPhysicsAPI.h>

#include <wglm/gtx/quaternion.hpp>

#include <misc/Timer.h>

void SetupDefaultRigidDynamic(physx::PxRigidDynamic& body, bool kinematic = false) {
	//body.setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);
	//body.setAngularDamping(0.5f);
	//body.setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, kinematic);
}


void game::GameState::runTick() {
	this->tick++;

	const physx::PxReal stepSize = 1.0f / 60.0f;

	Global<misc::Timer>->newTiming("px sim");
	this->scene->simulate(stepSize);

	this->scene->fetchResults(true);
	Global<misc::Timer>->endTiming("px sim");

	Global<misc::Timer>->newTiming("px update");
	physx::PxU32 count;
	[[maybe_unused]]
	auto changes = this->scene->getActiveActors(count);

	for (size_t i = 0; i < count; i++) {
		if (changes[i]->userData == nullptr) {
			continue;
		}
		auto t = static_cast<physx::PxRigidDynamic*>(changes[i])->getGlobalPose();
		glm::vec3 p{ t.p.x, t.p.y, t.p.z };
		glm::quat q{ t.q.x, t.q.y, t.q.z , t.q.w };

		WeakObject obj{
			.index = std::bit_cast<Index<Everything>>(changes[i]->userData),
			.proxy = &everything
		};

		auto ptr = obj.get<Transform>().transform = glm::translate(p) * glm::toMat4(q);
	}
	Global<misc::Timer>->endTiming("px update");

	rand();

}

void game::GameState::addRenderInfo(RenderInfo& renderInfo) {
	this->everything.match([&](Model& model, Transform& transform) {
		renderInfo.addModel(model.model, transform.transform);
		});
}

void game::GameState::init() {
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator,
		errorCallback);
	if (!foundation) {
		assert(0);
		std::terminate();
	}

	bool recordMemoryAllocations = true;

	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
		physx::PxTolerancesScale(), recordMemoryAllocations);
	if (!physics) {
		assert(0);
		std::terminate();
	}

	physx::PxSceneDesc pxSceneDesc(physics->getTolerancesScale());
	auto gravity = 100.0f;
	pxSceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, -gravity);
	pxSceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	pxSceneDesc.bounceThresholdVelocity = 0.2f * gravity;
	auto test = pxSceneDesc.getTolerancesScale();
	rand();

	if (!pxSceneDesc.cpuDispatcher) {
		cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(8);
		if (!cpuDispatcher) {
			std::cout << "PxCpuDispatcher creation failed!\n";
			assert(0);
			std::terminate();
		}

		pxSceneDesc.cpuDispatcher = cpuDispatcher;
	}

	if (!pxSceneDesc.filterShader) {
		pxSceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	}

	scene = physics->createScene(pxSceneDesc);
	if (!scene) {
		assert(0);
		std::terminate();
	}
	this->material = physics->createMaterial(1.5f, 1.5f, 0.5f);


	{
		auto obj = this->everything.make();

		obj.add<Model>(ModelEnum::CUBE);
		obj.add<Transform>(glm::mat4());

		physx::PxRigidDynamic* box = physx::PxCreateDynamic(
			*this->physics,
			physx::PxTransform(physx::PxVec3(0.0f, 1.0f, 1.1f)),
			physx::PxBoxGeometry(physx::PxVec3(1.0f, 1.0f, 1.0f)),
			*this->material,
			1.0f
		);

		box->userData = std::bit_cast<void*>(obj.index);

		SetupDefaultRigidDynamic(*box);
		this->scene->addActor(*box);
	}

	constexpr auto get = [] { return static_cast<float>(rand() % 100 - 50); };

	for (size_t i = 0; i < 10; i++) {
		auto obj = this->everything.make();

		obj.add<Model>(ModelEnum::CUBE);
		obj.add<Transform>(glm::mat4());

		physx::PxRigidDynamic* box = physx::PxCreateDynamic(
			*this->physics,
			physx::PxTransform(physx::PxVec3(get(), get(), 30.0f)),
			physx::PxBoxGeometry(physx::PxVec3(1.0f, 1.0f, 1.0f)),
			*this->material,
			1.0f
		);

		box->userData = std::bit_cast<void*>(obj.index);

		//SetupDefaultRigidDynamic(*box);
		this->scene->addActor(*box);
	}

	{
		auto obj = this->everything.make();

		obj.add<Model>(ModelEnum::CUBE);
		obj.add<Transform>(glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)));

		physx::PxRigidDynamic* box = physx::PxCreateDynamic(
			*this->physics,
			physx::PxTransform(physx::PxVec3(0.0f, 0.0f, 1000.0f)),
			physx::PxBoxGeometry(physx::PxVec3(1.0f, 1.0f, 1.0f)),
			*this->material,
			1.0f
		);

		box->userData = std::bit_cast<void*>(obj.index);

		SetupDefaultRigidDynamic(*box);
		this->scene->addActor(*box);

	}

	{
		auto obj = this->everything.make();

		obj.add<Model>(ModelEnum::PLANE);
		obj.add<Transform>(
			glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) *
			glm::scale(glm::vec3(100.0f))
			);

		auto plane = physx::PxCreatePlane(
			*this->physics,
			physx::PxPlane(physx::PxVec3(0, 0, 1).getNormalized(), 0)
			,
			*material
		);

		obj.add<Physics>(plane);

		this->scene->addActor(*plane);
	}
}
