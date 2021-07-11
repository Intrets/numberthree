#include "ui/ConstructMovementHandlers.h"

#include <ui/State.h>
#include <ui/Constructer.h>
#include <ui/Invisible.h>

#include <misc/Option.h>

#include <mem/ReferenceManager.h>

#include <PxPhysicsAPI.h>

#include "player/PlayerInfo.h"
#include "Game.h"

namespace game
{
	using namespace ui;

	UniqueReference<ui::Base, ui::Base> constructMovementHandlers() {
		// wasd movement in world
		{
			UniqueReference<Base, Invisible> movement = Global::getManager().makeUniqueRef<Invisible>();

			movement.get()->addGlobalBind({ CONTROL::KEY::MOUSE_POS_CHANGED, CONTROL::STATE::PRESSED }, [&](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					auto d = glm::vec2(uiInfo.uiState.getCursorMovement()) / 500.0f;
					userData.look += glm::vec3(0.0f, d.y, -d.x);
					userData.look.y = glm::clamp(userData.look.y, 0.1f, glm::pi<float>() - 0.1f);
					return BIND::RESULT::CONTINUE;
				});

			constexpr float targetSpeed = 25.0f;
			constexpr float acceleration = 400.0f;

			constexpr auto calculateForce = [](glm::vec3 dir, glm::vec3 v) {
				dir = glm::normalize(dir);
				auto c = glm::dot(dir, v);

				if (c + acceleration / 60.0f > targetSpeed) {
					return dir * std::clamp(targetSpeed - c, 0.0f, acceleration / 60.0f) * 60.0f;
				}
				else {
					return dir * acceleration;
				}
			};

			movement.get()->addGlobalBind({ CONTROL::KEY::LEFT, CONTROL::STATE::PRESSED | CONTROL::STATE::DOWN }, [&](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (userData.player.isQualified()) {
						glm::vec4 dir4{ -1.0f, 0.0f, 0.0f, 1.0f };
						auto M = glm::yawPitchRoll(-userData.look.x, -userData.look.y, -userData.look.z);
						glm::vec3 dir3 = dir4 * M;
						dir3.z = 0.0f;

						auto body = userData.player->get<game::Physics>().getAs<physx::PxRigidDynamic>();
						const auto v = convert<glm::vec3>(body->getLinearVelocity());

						userData.player->get<game::Physics>().applyAcceleration(calculateForce(dir3, v));
					}
					return BIND::RESULT::CONTINUE;
				});

			movement.get()->addGlobalBind({ CONTROL::KEY::RIGHT, CONTROL::STATE::PRESSED | CONTROL::STATE::DOWN }, [&](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (userData.player.isQualified()) {
						glm::vec4 dir4{ 1.0f, 0.0f, 0.0f, 1.0f };
						auto M = glm::yawPitchRoll(-userData.look.x, -userData.look.y, -userData.look.z);
						glm::vec3 dir3 = dir4 * M;
						dir3.z = 0.0f;

						auto body = userData.player->get<game::Physics>().getAs<physx::PxRigidDynamic>();
						const auto v = convert<glm::vec3>(body->getLinearVelocity());

						userData.player->get<game::Physics>().applyAcceleration(calculateForce(dir3, v));
					}
					return BIND::RESULT::CONTINUE;
				});

			movement.get()->addGlobalBind({ CONTROL::KEY::DOWN, CONTROL::STATE::PRESSED | CONTROL::STATE::DOWN }, [&](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (userData.player.isQualified()) {
						glm::vec4 dir4{ 0.0f, 0.0f, 1.0f, 1.0f };
						auto M = glm::yawPitchRoll(-userData.look.x, -userData.look.y, -userData.look.z);
						glm::vec3 dir3 = dir4 * M;
						dir3.z = 0.0f;

						auto body = userData.player->get<game::Physics>().getAs<physx::PxRigidDynamic>();
						const auto v = convert<glm::vec3>(body->getLinearVelocity());

						userData.player->get<game::Physics>().applyAcceleration(calculateForce(dir3, v));
					}
					return BIND::RESULT::CONTINUE;
				});

			movement.get()->addGlobalBind({ CONTROL::KEY::UP, CONTROL::STATE::PRESSED | CONTROL::STATE::DOWN }, [&](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (userData.player.isQualified()) {
						glm::vec4 dir4{ 0.0f, 0.0f, -1.0f, 1.0f };
						auto M = glm::yawPitchRoll(-userData.look.x, -userData.look.y, -userData.look.z);
						glm::vec3 dir3 = dir4 * M;
						dir3.z = 0.0f;

						auto body = userData.player->get<game::Physics>().getAs<physx::PxRigidDynamic>();
						const auto v = convert<glm::vec3>(body->getLinearVelocity());

						userData.player->get<game::Physics>().applyAcceleration(calculateForce(dir3, v));
					}
					return BIND::RESULT::CONTINUE;
				});

			movement.get()->addGlobalBind({ CONTROL::KEY::JUMP, CONTROL::STATE::PRESSED | CONTROL::STATE::DOWN }, [&](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (userData.player.isQualified()) {
						if (userData.player->get<game::Player>().onGround) {
							userData.player->get<game::Player>().onGround = false;

							glm::vec3 force{ 0.0f, 0.0f, 1.0f };

							userData.player->get<game::Physics>().applyAcceleration(force * 2000.0f);
						}

					}
					return BIND::RESULT::CONTINUE;
				});

			movement.get()->addGlobalBind({ CONTROL::KEY::SCROLL_UP }, [&](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					using viewport = misc::Option<misc::OPTION::CL_VIEWPORTSCALE, float>;
					viewport::setVal(viewport::getVal() / 1.1f);
					return BIND::RESULT::CONTINUE;
				});

			movement.get()->addGlobalBind({ CONTROL::KEY::SCROLL_DOWN }, [&](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					using viewport = misc::Option<misc::OPTION::CL_VIEWPORTSCALE, float>;
					viewport::setVal(viewport::getVal() * 1.1f);
					return BIND::RESULT::CONTINUE;
				});

			return movement;
		}
	}
}