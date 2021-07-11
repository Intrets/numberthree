#include "GameState.h"

#include <bit>

#include "Game.h"

#include <render/loaders/ModelLoader.h>

#include <mem/Global.h>

#include <PxPhysicsAPI.h>

#include <wglm/gtx/quaternion.hpp>

#include <misc/Timer.h>

void SetupDefaultRigidDynamic(physx::PxRigidDynamic& body, bool kinematic = false) {
	//body.setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);
	//body.setAngularDamping(0.5f);
	//body.setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, kinematic);
}


struct FilterGroup
{
	enum : physx::PxU32
	{
		PLAYER = 1 << 0,
		WORLD = 1 << 1,
	};
};

physx::PxFilterFlags SampleSubmarineFilterShader(
	physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize) {
	// let triggers through
	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1)) {
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

	if (filterData0.word0 & FilterGroup::PLAYER) {
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
	}

	return physx::PxFilterFlag::eDEFAULT;
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

		WeakObject obj{
			.index = std::bit_cast<Index<Everything>>(changes[i]->userData),
			.proxy = &everything
		};

		const auto t = static_cast<physx::PxRigidDynamic*>(changes[i])->getGlobalPose();
		obj.get<Transform>().quat = { t.q.x, t.q.y, t.q.z , t.q.w };
		obj.get<Transform>().pos = { t.p.x, t.p.y, t.p.z };
	}
	Global<misc::Timer>->endTiming("px update");

	rand();

}

void game::GameState::addRenderInfo(RenderInfo& renderInfo) {
	this->everything.match([&](Model& model, Transform& transform) {
		renderInfo.addModel(model.model, transform.getTransform());
		});

	if (this->player.isQualified()) {
		//renderInfo.cameraInfo.V = this->player->get<Transform>().transform;
	}
}

void game::GameState::init() {
	Global<Everything*>.provide(new (Everything*)(&this->everything));

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
		//pxSceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		pxSceneDesc.filterShader = SampleSubmarineFilterShader;
		pxSceneDesc.simulationEventCallback = &this->handler;
	}

	scene = physics->createScene(pxSceneDesc);
	if (!scene) {
		assert(0);
		std::terminate();
	}
	this->material = physics->createMaterial(0.0f, 0.0f, 0.5f);
	this->playerMaterial = physics->createMaterial(0.0f, 0.0f, 0.0f);

	// player object
	{
		auto obj = this->everything.make();

		this->player = obj;

		obj.add<Transform>();
		obj.add<Model>(ModelEnum::CUBE);
		obj.add<Player>();

		physx::PxRigidDynamic* box = physx::PxCreateDynamic(
			*this->physics,
			physx::PxTransform(physx::PxVec3(0.0f, 0.0f, 3.0f)),
			physx::PxBoxGeometry(physx::PxVec3(1.0f, 1.0f, 3.0f)),
			*this->playerMaterial,
			1.0f
		);
		physx::PxShape* shapes;
		box->getShapes(&shapes, 1, 0);
		physx::PxFilterData filterData;

		filterData.word0 = FilterGroup::PLAYER;
		shapes->setSimulationFilterData(filterData);

		obj.add<Physics>(box);

		box->setMassSpaceInertiaTensor(physx::PxVec3(0.0f, 0.0f, 0.0f));
		box->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);

		box->userData = std::bit_cast<void*>(obj.index);
		this->scene->addActor(*box);
	}

	{
		auto obj = this->everything.make();

		obj.add<Model>(ModelEnum::CUBE);
		obj.add<Transform>();

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
		obj.add<Transform>();

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
		obj.add<Transform>(
			Transform{
				.pos = glm::vec3(0.0f, 0.0f, 0.0f)
			});

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
			Transform{
				.pos = glm::vec3(0.0f, 0.0f, 0.0f),
				.quat = glm::quat(),
				.scale = glm::vec3(100.0f)
			}
		);

		auto plane = physx::PxCreatePlane(
			*this->physics,
			physx::PxPlane(physx::PxVec3(0, 0, 1).getNormalized(), 0)
			,
			*material
		);

		//obj.add<Physics>(plane);

		this->scene->addActor(*plane);
	}
}
