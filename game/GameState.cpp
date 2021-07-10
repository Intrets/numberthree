#include "GameState.h"

#include "Game.h"

#include <render/loaders/ModelLoader.h>

#include <PxPhysicsAPI.h>

void SetupDefaultRigidDynamic(physx::PxRigidDynamic& body, bool kinematic = false) {
	body.setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);
	body.setAngularDamping(0.5f);
	body.setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, kinematic);
}


void game::GameState::runTick() {
	this->tick++;

	const physx::PxReal stepSize = 1.0f / 60.0f;

	this->scene->simulate(stepSize);

	this->scene->fetchResults(true);

	physx::PxU32 count;
	[[maybe_unused]]
	auto changes = this->scene->getActiveActors(count);

	for (size_t i = 0; i < count; i++) {
		auto t = static_cast<physx::PxRigidDynamic*>(changes[i])->getGlobalPose();
		glm::vec3 p;
		p.x = t.p.x;
		p.y = t.p.y;
		p.z = t.p.z;

		static_cast<Transform*>(changes[i]->userData)->transform = glm::translate(p);


	}

	//physx::PxActorTypeFlags test1;
	//physx::PxActorTypeFlags test2;
	//test1.set(physx::PxActorTypeFlag::eRIGID_DYNAMIC);
	//test2.set(physx::PxActorTypeFlag::eRIGID_STATIC);
	//auto test = test1 | test2;
	//std::vector<physx::PxActor*> buff;
	//buff.resize(100);
	//this->scene->getActors(
	//	test,
	//	buff.data(),
	//	static_cast<physx::PxU32>(buff.size()),
	//	0
	//);

	//for (size_t i = 0; i < 2; i++) {
	//	[[maybe_unused]]
	//	auto p = static_cast<physx::PxRigidActor*>(buff[i])->getGlobalPose().p;
	//	rand();

	//}

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
	pxSceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, -90.81f);
	pxSceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;

	if (!pxSceneDesc.cpuDispatcher) {
		cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
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
	this->material = physics->createMaterial(0.5f, 0.5f, 0.5f);


	{
		auto obj = this->everything.make();

		obj.add<Model>(ModelEnum::CUBE);
		obj.add<Transform>(glm::mat4());
	}

	{
		auto obj = this->everything.make();

		obj.add<Model>(ModelEnum::CUBE);
		obj.add<Transform>(glm::translate(glm::vec3(10.0f, 0.0f, 0.0f)));

		//auto box = physx::PxBoxGeometry(physx::PxVec3(1.0f, 1.0f, 1.0f));
		//physx::PxTransform transform{ physx::PxVec3(0.0f, 0.0f, 3.0f) };
		//auto box_actor = physx::PxCreateDynamic(
		//	*this->physics,
		//	transform,
		//	box,
		//	*this->material,
		//	1.0f
		//);

		//obj.add<Physics>(box_actor);

		//this->scene->addActor(*box_actor);

		//rand();

		physx::PxRigidDynamic* box = physx::PxCreateDynamic(*this->physics, physx::PxTransform(physx::PxIdentity), physx::PxBoxGeometry(physx::PxVec3(1.0f, 1.0f, 1.0f)), *this->material, 1.0f);

		box->userData = &obj.get<Transform>();

		SetupDefaultRigidDynamic(*box);
		this->scene->addActor(*box);

	}

	{
		auto obj = this->everything.make();

		obj.add<Model>(ModelEnum::PLANE);
		obj.add<Transform>(
			glm::translate(glm::vec3(0.0f, 0.0f, 5.0f)) *
			glm::scale(glm::vec3(10.0f))
			);

		auto plane = physx::PxCreatePlane(
			*this->physics,
			physx::PxPlane(physx::PxVec3(0, 0, 1).getNormalized(), 10),
			*material
		);

		obj.add<Physics>(plane);

		this->scene->addActor(*plane);
	}
}
