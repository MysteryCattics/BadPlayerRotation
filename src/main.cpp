#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(NoRotatePlayer, PlayerObject) {
    struct Fields {
        float lastDashAngle = 0.0f;
    };

    void updateRotation(float dt) {
        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            PlayerObject::updateRotation(dt);
            return;
        }

        if (m_isShip || m_isBird || m_isDart || m_isSwing) {
            if (m_isDashing) {
                m_fields->lastDashAngle = -m_dashAngle; // всегда минус
                this->setRotation(m_fields->lastDashAngle);
            } else {
                this->setRotation(m_fields->lastDashAngle);
            }

            if (!m_isSwing) {
                this->setFlipY(m_isUpsideDown);
            }
        } else {
            PlayerObject::updateRotation(dt);
        }
    }

    void resetObject() {
        PlayerObject::resetObject();
        m_fields->lastDashAngle = 0.0f;
        this->setRotation(0.0f);
    }
};
