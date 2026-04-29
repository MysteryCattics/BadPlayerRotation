#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(NoRotatePlayer, PlayerObject) {
    struct Fields {
        float lastDashAngle = 0.0f;
        int lastGamemode = -1;
        float groundAngle = 0.0f;
        bool isOnSlope = false;
        GameObject* currentSlope = nullptr;  // Добавляем указатель на текущий наклон
    };

    // Этот метод вызывается при столкновении с наклонной поверхностью
    void collidedWithSlope(float dt, GameObject* object, bool skipPre) {
        // Сохраняем информацию о наклоне
        m_fields->currentSlope = object;
        m_fields->isOnSlope = true;
        m_fields->groundAngle = getSlopeAngle(object);
        
        // Сразу выравниваем игрока к поверхности
        this->setRotation(m_fields->groundAngle);
        
        // Вызываем оригинальный метод, чтобы не нарушать игровую логику
        PlayerObject::collidedWithSlope(dt, object, skipPre);
    }

    void updateRotation(float dt) {
        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            PlayerObject::updateRotation(dt);
            return;
        }

        // Определяем текущий гейммо́д
        int currentGamemode = getCurrentGamemode();

        // Если гейммо́д изменился - сбрасываем угол
        if (m_fields->lastGamemode != currentGamemode && m_fields->lastGamemode != -1) {
            m_fields->lastDashAngle = 0.0f;
            m_fields->groundAngle = 0.0f;
            m_fields->isOnSlope = false;
            m_fields->currentSlope = nullptr;
            this->setRotation(0.0f);
        }

        // Определяем, нужно ли переопределять поворот
        bool shouldOverride = m_isShip || m_isBird || m_isDart || m_isSwing;

        if (shouldOverride) {
            // Если на наклонной поверхности - выравниваем
            if (m_fields->isOnSlope && !m_isDashing) {
                this->setRotation(m_fields->groundAngle);
            }
            // Сохраняем угол дэша
            else if (m_isDashing) {
                m_fields->lastDashAngle = -m_dashAngle;
                this->setRotation(m_fields->lastDashAngle);
            }
            // Обычное состояние
            else {
                this->setRotation(m_fields->lastDashAngle);
            }
            
            if (m_isSwing) {
                this->setFlipY(false);
            }
        } else {
            PlayerObject::updateRotation(dt);
            
            // Для обычных режимов тоже выравниваем к наклону
            if (m_fields->isOnSlope) {
                this->setRotation(m_fields->groundAngle);
            }
        }

        m_fields->lastGamemode = currentGamemode;
    }
    
    float getSlopeAngle(GameObject* obj) {
        if (!obj) return 0.0f;
        
        int id = obj->m_objectID;
        
        // Углы наклонов для основных ID
        switch(id) {
            case 4: return 45.0f;
            case 5: return -45.0f;
            case 6: return 30.0f;
            case 7: return -30.0f;
            case 8: return 60.0f;
            case 9: return -60.0f;
            case 10: return 20.0f;
            case 11: return -20.0f;
            case 12: return 15.0f;
            case 13: return -15.0f;
            case 14: return 10.0f;
            case 15: return -10.0f;
            default: return 0.0f;
        }
    }
    
    int getCurrentGamemode() {
        if (m_isShip) return 0;
        else if (m_isBird) return 1;
        else if (m_isBall) return 2;
        else if (m_isDart) return 3;
        else if (m_isRobot) return 4;
        else if (m_isSwing) return 5;
        else return 6;
    }

    void resetObject() {
        PlayerObject::resetObject();
        m_fields->lastDashAngle = 0.0f;
        m_fields->lastGamemode = -1;
        m_fields->groundAngle = 0.0f;
        m_fields->isOnSlope = false;
        m_fields->currentSlope = nullptr;
        this->setRotation(0.0f);
    }
};
