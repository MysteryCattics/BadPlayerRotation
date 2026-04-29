#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(NoRotatePlayer, PlayerObject) {
    struct Fields {
        float lastDashAngle = 0.0f;
        int lastGamemode = -1;  // Отслеживаем последний гейммо́д
    };

    void updateRotation(float dt) {
        // Проверяем, включён ли мод
        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            PlayerObject::updateRotation(dt);
            return;
        }

        // Определяем текущий гейммо́д
        int currentGamemode = -1;
        if (m_isShip) currentGamemode = 0;
        else if (m_isBird) currentGamemode = 1;      // Wave
        else if (m_isBall) currentGamemode = 2;
        else if (m_isDart) currentGamemode = 3;      // Spider
        else if (m_isRobot) currentGamemode = 4;
        else if (m_isSwing) currentGamemode = 5;
        else currentGamemode = 6;                    // Cube (дефолтный режим)

        // Если гейммо́д изменился - сбрасываем угол
        if (m_fields->lastGamemode != currentGamemode && m_fields->lastGamemode != -1) {
            m_fields->lastDashAngle = 0.0f;
            this->setRotation(0.0f);
        }

        // Определяем, нужно ли переопределять поворот
        bool shouldOverride = m_isShip || m_isBird || m_isDart || m_isSwing;

        if (shouldOverride) {
            // Сохраняем угол дэша
            if (m_isDashing) {
                m_fields->lastDashAngle = -m_dashAngle;
            }
            
            // Применяем угол
            this->setRotation(m_fields->lastDashAngle);
            
            // Специальная обработка для Swing
            if (m_isSwing) {
                this->setFlipY(false);
            }
        } else {
            // Для Cube, Ball, Robot - используем оригинальную логику
            PlayerObject::updateRotation(dt);
        }

        // Запоминаем текущий гейммо́д для следующего кадра
        m_fields->lastGamemode = currentGamemode;
    }

    void resetObject() {
        PlayerObject::resetObject();
        m_fields->lastDashAngle = 0.0f;
        m_fields->lastGamemode = -1;
        this->setRotation(0.0f);
    }
};
