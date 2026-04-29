#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(NoRotatePlayer, PlayerObject) {
    struct Fields {
        float lastDashAngle = 0.0f;
        bool wasSpecialMode = false;
    };

    void updateRotation(float dt) {
        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            PlayerObject::updateRotation(dt);
            return;
        }

        bool isSpecialMode = m_isShip || m_isBird || m_isDart || m_isSwing;

        if (isSpecialMode) {
            if (!m_fields->wasSpecialMode) {
                m_fields->lastDashAngle = 0.0f;
                this->setRotation(0.0f);
            }

            if (m_isDashing) {
                m_fields->lastDashAngle = -m_dashAngle;
            }

            this->setRotation(m_fields->lastDashAngle);

            if (m_isSwing) {
                this->setFlipY(false);
            }
        } else {
            PlayerObject::updateRotation(dt);
        }

        m_fields->wasSpecialMode = isSpecialMode;
    }

    void resetObject() {
        PlayerObject::resetObject();
        m_fields->lastDashAngle = 0.0f;
        m_fields->wasSpecialMode = false;
        this->setRotation(0.0f);
    }
};
