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
                m_fields->lastDashAngle = -m_dashAngle; // завжди мінус
                this->setRotation(m_fields->lastDashAngle);
            } else {
                // якщо wave і був портал трансформації, але не dash — ресет
                double now = m_totalTime;
                if (m_isDart && m_lastActivatedPortal && (now - m_gameModeChangedTime < 0.1)) {
                    m_fields->lastDashAngle = 0.0f;
                    this->setRotation(0.0f);
                } else {
                    this->setRotation(m_fields->lastDashAngle);
                }
            }

            if (!m_isSwing) {
                this->setFlipY(m_isUpsideDown);
            }
        } else {
            PlayerObject::updateRotation(dt);
        }
    }

    void resetObject() {
        PlayerObject::resetObject();   // базовий ресет
        m_fields->lastDashAngle = 0.0f; // обнуляємо кут
        this->setRotation(0.0f);        // скидаємо поворот
    }
};
