#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(NoRotatePlayer, PlayerObject) {
    float lastDashAngle = 0.0f; // поле класса

    void updateRotation(float dt) {
        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            PlayerObject::updateRotation(dt);
            return;
        }

        if (m_isShip || m_isBird || m_isDart || m_isSwing) {
            if (m_isDashing) {
                lastDashAngle = m_dashAngle;
                this->setRotation(-m_dashAngle);
            } else {
                this->setRotation(-lastDashAngle);
            }

            if (!m_isSwing) {
                this->setFlipY(m_isUpsideDown);
            }
        } else {
            PlayerObject::updateRotation(dt);
        }
    }

    // правильный метод сброса
    void resetObject() {
        PlayerObject::resetObject(); // базовый сброс
        lastDashAngle = 0.0f;        // обнуляем угол
        this->setRotation(0.0f);     // сброс вращения
    }
};
