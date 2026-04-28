#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(NoRotatePlayer, PlayerObject) {
    struct Fields {
        float lastDashAngle = 0.0f; // все кастомные поля сюда
    };

    void updateRotation(float dt) {
        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            PlayerObject::updateRotation(dt);
            return;
        }

        auto& f = m_fields; // доступ к твоим полям

        if (m_isShip || m_isBird || m_isDart || m_isSwing) {
            if (m_isDashing) {
                f.lastDashAngle = m_dashAngle;
                this->setRotation(m_dashAngle);
            } else {
                this->setRotation(f.lastDashAngle);
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
