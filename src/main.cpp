#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(NoRotatePlayer, PlayerObject) {
    void updateRotation(float dt) {
        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            PlayerObject::updateRotation(dt);
            return;
        }

        if (m_isShip || m_isBird || m_isDart || m_isSwing) {
            static float lastDashAngle = 0.0f;

            if (m_isDashing) {
                lastDashAngle = m_dashAngle;
                this->setRotation(-m_dashAngle);
            } else {
                this->setRotation(-lastDashAngle);
            }

            // переворот НЕ трогаем — оставляем стандартный
        } else {
            PlayerObject::updateRotation(dt);
        }
    }
};
