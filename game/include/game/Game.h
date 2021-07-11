#pragma once

#include <array>
#include <string_view>

#include <serial/Serializer.h>

#include <mem/Everything.h>

#include <wglm/gtx/quaternion.hpp>
#include <wglm/gtx/transform.hpp>

#include <PxPhysicsAPI.h>

namespace game
{
	struct GameState;
}

struct UserData
{
	glm::vec3 look = { 0.0f, glm::half_pi<float>(), 0.0f };

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

//constexpr std::array<std::string_view, static_cast<size_t>(ModelEnum::MAX)> modelNames{
//	"2x2Plane.obj"sv,
//	"Cube.obj"sv,
//};

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
	};

	struct Player
	{
		bool onGround = false;
	};
}

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
