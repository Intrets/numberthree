#pragma once

#include <array>
#include <string_view>

#include <serial/Serializer.h>

#include <mem/Everything.h>

#include <misc/Misc.h>

#include <wglm/gtx/quaternion.hpp>
#include <wglm/gtx/transform.hpp>

#include <PxPhysicsAPI.h>

using namespace physx;

template<class From>
struct Convert<physx::PxQuat, From>
{
	using To = physx::PxQuat;

	static inline To run(From const from) {
		if constexpr (std::is_same_v<To, From>) {
			return from;
		}
		else {
			return { from.x, from.y, from.z, from.w };
		}
	}
};

template<class From>
struct Convert<physx::PxVec3, From>
{
	using To = physx::PxVec3;

	static inline To run(From const from) {
		if constexpr (std::is_same_v<To, From>) {
			return from;
		}
		else {
			return { from.x, from.y, from.z };
		}
	}
};

namespace game
{
	struct GameState;
}

struct Look
{
	float yaw{};
	float pitch{};
	float roll{};

	void changePitch(float d);
	void changeYaw(float d);
	void changeRoll(float d);

	glm::vec3 getDir();
	glm::quat getQuat();
	glm::mat4 getTransform();
	glm::mat3 getTransform3();
};

struct UserData
{
	Look look{};

	QualifiedObject player;

	game::GameState& gameState;
};

enum class ModelEnum
{
	PLANE,
	CUBE,
	MAX
};

using namespace std::string_view_literals;

constexpr std::string_view modelNames[] = {
	"2x2Plane.obj"sv,
	"Cube.obj"sv,
};

namespace game
{
	struct Model
	{
		ModelEnum model;
	};

	struct Transform
	{
		glm::vec3 pos{ 0.0f, 0.0f, 0.0f };
		glm::quat quat{};
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::mat4 getTransform() {
			return glm::translate(this->pos) * glm::toMat4(this->quat) * glm::scale(this->scale);
		};
	};

	struct Physics
	{
		physx::PxActor* actor;

		template<class T>
		T* getAs() {
			return static_cast<physx::PxRigidDynamic*>(this->actor);
		}

		void applyAcceleration(glm::vec3 force) {
			static_cast<physx::PxRigidDynamic*>(this->actor)->addForce(
				physx::PxVec3(force.x, force.y, force.z),
				physx::PxForceMode::eACCELERATION
			);
		}

		void applyLocalAcceleration(glm::vec3 force, glm::vec3 offset) {
			PxRigidBodyExt::addLocalForceAtLocalPos(
				*static_cast<PxRigidDynamic*>(this->actor),
				convert<PxVec3>(force),
				convert<PxVec3>(offset),
				PxForceMode::eFORCE
			);
		}
	};

	struct Player
	{
		bool onGround = false;
	};

	struct Explosive
	{

	};

	struct PhysicsForce
	{
		glm::vec3 force;
		glm::vec3 offset;
	};
}

template<>
struct serial::Serializable<game::PhysicsForce>
{
	inline const static std::string_view typeName = "PhysicsForce";

	ALL_DEF(game::PhysicsForce) {
		return serializer.runAll<Selector>(
			ALL(force)
			);
	}
};

template<>
struct serial::Serializable<game::Explosive>
{
	inline const static std::string_view typeName = "Explosive";

	ALL_DEF(game::Explosive) {
		return true;
	}
};

template<>
struct serial::Serializable<game::Player>
{
	inline const static std::string_view typeName = "Player";

	ALL_DEF(game::Player) {
		return serializer.runAll<Selector>(
			ALL(onGround)
			);
	}
};

template<>
struct serial::Serializable<ModelEnum>
{
	inline const static std::string_view typeName = "ModelEnum";

	READ_DEF(ModelEnum) {
		int32_t val;
		if (!serializer.read(val)) return false;
		obj = static_cast<ModelEnum>(val);
		return true;
	}

	WRITE_DEF(ModelEnum) {
		return serializer.write(static_cast<int32_t>(obj));
	}

	PRINT_DEF(ModelEnum) {
		return serializer.print(static_cast<int32_t>(obj));
	}
};

template<>
struct serial::Serializable<game::Model>
{
	inline const static std::string_view typeName = "Model";

	ALL_DEF(game::Model) {
		return serializer.runAll<Selector>(
			ALL(model)
			);
	};
};

template<>
struct serial::Serializable<game::Transform>
{
	inline const static std::string_view typeName = "Transform";

	ALL_DEF(game::Transform) {
		return serializer.runAll<Selector>(
			ALL(pos),
			ALL(quat)
			);
	};
};

template<>
struct serial::Serializable<game::Physics>
{
	inline const static std::string_view typeName = "Physics";

	ALL_DEF(game::Physics) {
		return false;
	};
};
