#pragma once

#include <array>
#include <string_view>

#include <serial/Serializer.h>

#include <PxPhysicsAPI.h>

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
		glm::mat4 transform;
	};

	struct Physics
	{
		physx::PxActor* actor;
	};
}

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
			ALL(transform)
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
