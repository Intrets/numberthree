#include "ui/ConstructMovementHandlers.h"

#include <ui/State.h>
#include <ui/Constructer.h>
#include <ui/Invisible.h>

#include <misc/Option.h>

#include <mem/ReferenceManager.h>

#include <PxPhysicsAPI.h>

#include "player/PlayerInfo.h"
#include "Game.h"
#include "GameState.h"

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
					userData.look.changePitch(-d.y);
					userData.look.changeYaw(-d.x);
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

			for (const auto [key, dir3] : {
				std::tuple{ CONTROL::KEY::RIGHT, glm::vec3(0.0f, -1.0f, 0.0f) },
				std::tuple{ CONTROL::KEY::LEFT, glm::vec3(0.0f, 1.0f, 0.0f) },
				std::tuple{ CONTROL::KEY::DOWN, glm::vec3(-1.0f, 0.0f, 0.0f) },
				std::tuple{ CONTROL::KEY::UP, glm::vec3(1.0f, 0.0f, 0.0f) }
				}) {

				movement.get()->addGlobalBind({ key, CONTROL::STATE::PRESSED | CONTROL::STATE::DOWN }, [=](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
					{
						if (userData.player.isQualified()) {
							auto dir = userData.look.getTransform3() * dir3;
							dir.z = 0.0f;

							auto body = userData.player->get<game::Physics>().getAs<physx::PxRigidDynamic>();
							const auto v = convert<glm::vec3>(body->getLinearVelocity());

							userData.player->get<game::Physics>().applyAcceleration(calculateForce(dir, v));
						}
						return BIND::RESULT::CONTINUE;
					});
			}

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

			movement.get()->addGlobalBind({ CONTROL::KEY::ACTION0, CONTROL::STATE::DOWN }, [&](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					static int i = 0;
					if (i++ % 1 == 0) {
						if (userData.player.isQualified()) {
							auto dir = userData.look.getDir();

							glm::vec3 headHeight{ 0.0f, 0.0f, 1.5f };

							userData.gameState.shootTwirlyRocketTest(
								userData.player->get<Transform>().pos + headHeight + dir * 2.0f,
								userData.look.getQuat(),
								10.0f
							);
							userData.gameState.shootTwirlyRocketTest(
								userData.player->get<Transform>().pos + headHeight + dir * 2.0f,
								userData.look.getQuat(),
								10.0f
							);
						}
					}
					return BIND::RESULT::CONTINUE;
				});

			return movement;
		}
	}
}