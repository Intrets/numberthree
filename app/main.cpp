#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <filesystem>
#include <chrono>
#include <cassert>
#include <sstream>

#include "MainLoop.h"
#include "Setup.h"

#include <misc/Log.h>

#include <mem/Global.h>


#include <PxPhysicsVersion.h>
#include <PxPhysics.h>
#include <PxFoundation.h>
#include <PxScene.h>
#include <PxMaterial.h>
#include <geometry/PxSphereGeometry.h>
#include <geometry/PxBoxGeometry.h>
#include <geometry/PxPlaneGeometry.h>
#include <PxRigidStatic.h>
#include <common/PxTolerancesScale.h>
#include <common/PxCoreUtilityTypes.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>
#include <foundation/PxMathUtils.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultSimulationFilterShader.h>


//#include<common/>

// TODO: keep runtime option, get value from config/command line argument
bool OPENGL_DEBUG = true;

GLFWwindow* window;

class MyAllocator : public physx::PxDefaultAllocator
{
public:
	MyAllocator() = default;
	virtual ~MyAllocator() = default;
	virtual void* allocate(size_t size, const char* typeName, const char* filename,
		int line) {
		return std::malloc(size);

	};
	virtual void deallocate(void* ptr) {
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

static physx::PxFoundation* foundation;
static physx::PxPhysics* physics;
static physx::PxScene* scene;
static physx::PxCpuDispatcher* cpuDispatcher;
static MyAllocator allocator{};
static MyErrorCallback errorCallback{};

#define PX_CHECK_AND_RETURN_NULL(b, message) if (!b) std::terminate();

//namespace physx
//{
//	PxRigidStatic* PxCreateStatic(PxPhysics& sdk,
//		const PxTransform& transform,
//		PxShape& shape) {
//		PX_CHECK_AND_RETURN_NULL(transform.isValid(), "PxCreateStatic: transform is not valid.");
//
//		PxRigidStatic* s = sdk.createRigidStatic(transform);
//		if (s)
//			s->attachShape(shape);
//		return s;
//	}
//
//	PxRigidStatic* PxCreateStatic(PxPhysics& sdk,
//		const PxTransform& transform,
//		const PxGeometry& geometry,
//		PxMaterial& material,
//		const PxTransform& shapeOffset = PxTransform(PxIdentity)) {
//
//		PX_CHECK_AND_RETURN_NULL(transform.isValid(), "PxCreateStatic: transform is not valid.");
//		PX_CHECK_AND_RETURN_NULL(shapeOffset.isValid(), "PxCreateStatic: shapeOffset is not valid.");
//
//		PxShape* shape = sdk.createShape(geometry, material, true);
//		if (!shape)
//			return NULL;
//
//		shape->setLocalPose(shapeOffset);
//
//		PxRigidStatic* s = PxCreateStatic(sdk, transform, *shape);
//		shape->release();
//		return s;
//	}
//
//
//
//
//	PxRigidStatic* PxCreatePlane(PxPhysics& sdk,
//		const PxPlane& plane,
//		PxMaterial& material) {
//		PX_CHECK_AND_RETURN_NULL(plane.n.isFinite(), "PxCreatePlane: plane normal is not valid.");
//
//		if (!plane.n.isNormalized())
//			return NULL;
//
//		return PxCreateStatic(sdk, PxTransformFromPlaneEquation(plane), PxPlaneGeometry(), material);
//	}
//}

int main(int argc, char* argv[]) {

	//foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator,
	//	errorCallback);
	//if (!foundation) {
	//	assert(0);
	//	std::terminate();
	//}

	//bool recordMemoryAllocations = true;

	//physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
	//	physx::PxTolerancesScale(), recordMemoryAllocations);
	//if (!physics) {
	//	assert(0);
	//	std::terminate();
	//}

	//physx::PxSceneDesc pxSceneDesc(physics->getTolerancesScale());
	//pxSceneDesc.gravity = physx::PxVec3(0.0f, -90.81f, 0.0f);

	//if (!pxSceneDesc.cpuDispatcher) {
	//	cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	//	if (!cpuDispatcher) {
	//		std::cout << "PxCpuDispatcher creation failed!\n";
	//		return EXIT_FAILURE;
	//	}

	//	pxSceneDesc.cpuDispatcher = cpuDispatcher;
	//}

	//if (!pxSceneDesc.filterShader) {
	//	pxSceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	//}

	//scene = physics->createScene(pxSceneDesc);
	//if (!scene) {
	//	std::terminate();
	//}


	//[[maybe_unused]]
	//auto myMaterial = physics->createMaterial(0.5f, 0.5f, 0.5f);
	//std::cout << "refcount: " << myMaterial->getReferenceCount() << "\n";
	////myMaterial->release();
	//assert(myMaterial);


	//[[maybe_unused]]
	//physx::PxRigidStatic* floor = physx::PxCreatePlane(
	//	*physics,
	//	physx::PxPlane(physx::PxVec3(0, 1, 0), 0),
	//	*myMaterial
	//);

	//sizeof(physx::PxVec3);


	std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
	std::filesystem::path resourcesPath;
	if (argc > 1) {
		resourcesPath = std::filesystem::path(argv[1]);
	}
	else {
		std::cout << "No custom path specified, walking up current dir and looking for a resources folder.\n";
		std::cout << "Can take a single nameless argument to set resources folder.\n";

		resourcesPath = std::filesystem::current_path();
		auto rootPath = resourcesPath.root_path();

		// Limit the search in case something goes wrong somehow
		constexpr size_t max_search = 100;
		size_t i = 0;
		for (; i < max_search; i++) {
			if (std::filesystem::exists(resourcesPath / "resources")) {
				resourcesPath /= "resources";
				std::cout << "Found Resources path.\n";
				break;
			}

			resourcesPath = resourcesPath.parent_path();
		}
		if (i == max_search) {
			std::cerr << "Fatal error: could not find Resources folder.";
			return 0;
		}
	}

	std::cout << "Resources path: " << std::filesystem::absolute(resourcesPath) << "\n";

	if (!std::filesystem::exists(resourcesPath)) {
		std::cerr << "Fatal error: resources path does not specify an existing folder.\n";
		return 0;
	}

	if (!initGLFW(window, OPENGL_DEBUG)) {
		std::cerr << "Fatal error: initGLFW failed.\n";
		glfwTerminate();
		return 0;
	}

	initManagers(std::filesystem::absolute(resourcesPath).string());

	mainLoop(window, startTime);

	glfwTerminate();
}
