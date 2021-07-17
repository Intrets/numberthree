#include "GameState.h"

#include <bit>

#include "Game.h"

#include <render/loaders/ModelLoader.h>

#include <mem/Global.h>

#include <PxPhysicsAPI.h>

#include <wglm/gtx/quaternion.hpp>
#include <wglm/gtx/euler_angles.hpp>

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
		PROJECTILE = 1 << 2,
		DESTRUCTIBLE = 1 << 3,
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

	if ((filterData0.word0 & FilterGroup::DESTRUCTIBLE) && (filterData1.word0 & FilterGroup::PROJECTILE)) {
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
	}

	//if (filterData0.word0 & FilterGroup::PROJECTILE) {
	//	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
	//}
	if (filterData0.word0 & FilterGroup::PROJECTILE) {
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
	}

	return physx::PxFilterFlag::eDEFAULT;
}

void game::GameState::shootProjectile(glm::vec3 const pos, glm::vec3 const dir, float speed) {
	auto obj = this->everything.make();
	const glm::vec3 scale(0.5f);

	obj.add<Model>(ModelEnum::CUBE);
	obj.add<Transform>().scale = scale * glm::vec3(1.0f, 1.0f, 5.0f);
	obj.add<Explosive>();

	auto box = this->physics->createRigidDynamic(PxTransform(convert<PxVec3>(pos)));

	PxFilterData filterData;
	filterData.word0 = FilterGroup::PROJECTILE;

	//physx::PxRigidDynamic* box = physx::PxCreateDynamic(
	//	*this->physics,
	//	physx::PxTransform(convert<PxVec3>(pos)),
	//	physx::PxBoxGeometry(convert<PxVec3>(scale)),
	//	*this->material,
	//	1.0f
	//);

	//auto b = PxBoxGeometry(convert<PxVec3>(scale));
	static uintptr_t g = 0;

	for (int32_t i = -2; i < 2; i++) {
		auto shape = physics->createShape(
			PxBoxGeometry(convert<PxVec3>(scale)),
			*this->material,
			true
		);
		shape->setSimulationFilterData(filterData);
		shape->userData = (void*)g++;
		shape->setLocalPose(PxTransform(PxVec3(0.0f, 0.0f, 2.0f * i)));

		box->attachShape(*shape);

		shape->release();
	}

	//physx::PxShape* shapes;
	//box->getShapes(&shapes, 1, 0);
	//PxFilterData filterData;

	//filterData.word0 = FilterGroup::PROJECTILE | FilterGroup::DESTRUCTIBLE;
	//filterData.word0 = FilterGroup::PROJECTILE;
	//shapes->setSimulationFilterData(filterData);

	box->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	box->setLinearVelocity(convert<PxVec3>(dir * speed));
	box->userData = std::bit_cast<void*>(obj.index);

	PxRigidBodyExt::updateMassAndInertia(*box, 1.0f);

	this->scene->addActor(*box);
}

void game::GameState::shootTwirlyRocketTest(glm::vec3 const pos, glm::quat quat, float speed) {
	float randomRotation = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * glm::two_pi<float>();
	auto randomQuat = glm::quat(glm::vec3(randomRotation, 0.0f, 0.0f)) * glm::quat(glm::vec3(0.0f, -0.7f, 0.0));
	auto dir = glm::toMat3(quat * randomQuat) * glm::vec3{ 1.0f, 0.0f, 0.0f };

	auto obj = this->everything.make();
	glm::vec3 const scale{ 0.5f, 0.1f, 0.1f };

	obj.add<PhysicsForce>(PhysicsForce{
		.force = glm::vec3(6.0f, 0.0f, 2.0f) ,
		//.force = glm::vec3(0.0f, 0.0f, 0.0f) ,
		.offset = { -0.1f, 0.0f, 0.0f }
		});

	obj.add<Model>(ModelEnum::CUBE);
	auto& transform = obj.add<Transform>();
	transform.scale = scale;
	transform.quat = quat;
	transform.pos = pos;

	physx::PxRigidDynamic* rocket = physx::PxCreateDynamic(
		*this->physics,
		physx::PxTransform(convert<PxVec3>(pos + dir)) * PxTransform(convert<PxQuat>(quat * randomQuat)),
		physx::PxBoxGeometry(convert<PxVec3>(scale)),
		*this->material,
		1.0f
	);
	rocket->setAngularDamping(10.0f);

	PxFilterData filterData;
	filterData.word0 = FilterGroup::PROJECTILE;
	PxShape* shape;
	rocket->getShapes(&shape, 1);
	shape->setQueryFilterData(filterData);

	rocket->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	rocket->setLinearVelocity(convert<PxVec3>(dir * speed));
	rocket->userData = std::bit_cast<void*>(obj.index);

	obj.add<Physics>(rocket);
	this->scene->addActor(*rocket);
}

void game::GameState::runTick() {
	Global<render::DebugRenderInfo>->world.addPoint({});


	this->tick++;

	this->everything.match([](PhysicsForce& physicsForce, Physics& physics, Transform& transform) {
		physics.applyLocalAcceleration(
			physicsForce.force,
			physicsForce.offset
		);
		});

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
		obj.get<Transform>().quat = convert<glm::quat>(t.q);
		obj.get<Transform>().pos = convert<glm::vec3>(t.p);
	}
	Global<misc::Timer>->endTiming("px update");

	this->everything.collectRemoved();
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
	this->material = physics->createMaterial(0.0f, 0.7f, 0.3f);
	this->playerMaterial = physics->createMaterial(0.0f, 0.0f, -1.0f);

	// player object
	{
		auto obj = this->everything.make();

		this->player = obj;

		obj.add<Transform>();
		obj.add<Player>();

		physx::PxRigidDynamic* box = physx::PxCreateDynamic(
			*this->physics,
			physx::PxTransform(physx::PxVec3(-70.0f, 0.0f, 3.0f)),
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
		this->shootProjectile({ 30.0f, 30.0f, 5.0f }, { 1.0f, 0.0f, 0.0f }, 10.0f);
		this->shootProjectile({ 50.0f, 30.0f, 5.0f }, { -1.0f, 0.0f, 0.0f }, 10.0f);
	}


	 //big box
	{
		auto obj = this->everything.make();

		obj.add<Model>(ModelEnum::CUBE);
		obj.add<Transform>().scale = glm::vec3(10.0f, 10.0f, 50.0f);

		physx::PxRigidDynamic* box = physx::PxCreateDynamic(
			*this->physics,
			physx::PxTransform(physx::PxVec3(-10.0f, 10.0f, 50.1f)),
			physx::PxBoxGeometry(physx::PxVec3(10.0f, 10.0f, 50.0f)),
			*this->material,
			1.0f
		);

		physx::PxShape* shapes;
		box->getShapes(&shapes, 1, 0);
		physx::PxFilterData filterData;

		filterData.word0 = FilterGroup::DESTRUCTIBLE;
		shapes->setSimulationFilterData(filterData);

		box->userData = std::bit_cast<void*>(obj.index);

		this->scene->addActor(*box);
	}

	// small box under big box
	{
		auto obj = this->everything.make();

		obj.add<Model>(ModelEnum::CUBE);
		obj.add<Transform>().scale = glm::vec3(1.0f, 1.0f, 1.0f);

		physx::PxRigidDynamic* box = physx::PxCreateDynamic(
			*this->physics,
			physx::PxTransform(physx::PxVec3(5.0f, 25.0f, 1.1f)),
			physx::PxBoxGeometry(physx::PxVec3(1.0f, 1.0f, 1.0f)),
			*this->material,
			1.0f
		);

		box->userData = std::bit_cast<void*>(obj.index);

		SetupDefaultRigidDynamic(*box);
		this->scene->addActor(*box);
	}

	constexpr auto get = [] { return static_cast<float>(rand() % 100 - 50); };

	for (size_t i = 0; i < 0; i++) {
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

		auto plane = physx::PxCreatePlane(
			*this->physics,
			physx::PxPlane(physx::PxVec3(0, 0, 1).getNormalized(), 0),
			*material
		);

		auto p = convert<glm::vec3>(plane->getGlobalPose().p);
		auto q = convert<glm::quat>(plane->getGlobalPose().q);

		obj.add<Transform>(
			Transform{
				.pos = p,
				.quat = q,
				.scale = glm::vec3(200.0f)
			}
		);

		this->scene->addActor(*plane);
	}

	//// angled plane
	//{
	//	auto obj = this->everything.make();

	//	obj.add<Model>(ModelEnum::PLANE);

	//	auto plane = physx::PxCreatePlane(
	//		*this->physics,
	//		physx::PxPlane(physx::PxVec3(0, 1, 1).getNormalized(), 0),
	//		*material
	//	);

	//	PxShape* planeShape;
	//	plane->getShapes(&planeShape, 1, 0);

	//	auto p = convert<glm::vec3>(plane->getGlobalPose().p);
	//	auto q = convert<glm::quat>(plane->getGlobalPose().q);

	//	obj.add<Transform>(
	//		Transform{
	//			.pos = p,
	//			.quat = q,
	//			.scale = glm::vec3(100.0f)
	//		}
	//	);


	//	this->scene->addActor(*plane);
	//}
}
