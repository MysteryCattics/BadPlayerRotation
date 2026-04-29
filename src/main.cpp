#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(NoRotatePlayer, PlayerObject) {
    struct Fields {
        float lastDashAngle = 0.0f;
        int lastGamemode = -1;
        float groundAngle = 0.0f;
        bool isOnSlope = false;
        float timeSinceSlope = 0.0f;
    };

    void update(float dt) {
        PlayerObject::update(dt);
        
        if (!Mod::get()->getSettingValue<bool>("enabled")) return;
        
        // Проверяем, находится ли игрок на поверхности
        checkGroundCollision();
        
        // Если на наклоне и прошло меньше 0.1 секунды после касания
        if (m_fields->isOnSlope) {
            m_fields->timeSinceSlope += dt;
            if (m_fields->timeSinceSlope > 0.1f) {
                m_fields->isOnSlope = false;
            }
        }
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
            m_fields->timeSinceSlope = 0.0f;
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
    
    void checkGroundCollision() {
        auto playLayer = PlayLayer::get();
        if (!playLayer) {
            m_fields->isOnSlope = false;
            return;
        }
        
        // Получаем позицию игрока
        CCPoint playerPos = this->getPosition();
        float playerHeight = this->getScaledContentSize().height;
        
        // Проверяем точки под игроком
        std::vector<CCPoint> checkPoints = {
            ccp(playerPos.x, playerPos.y - playerHeight / 2),                      // Центр под игроком
            ccp(playerPos.x - 10, playerPos.y - playerHeight / 2),                 // Левая точка
            ccp(playerPos.x + 10, playerPos.y - playerHeight / 2),                 // Правая точка
            ccp(playerPos.x, playerPos.y - playerHeight / 2 - 5)                   // Чуть ниже центра
        };
        
        bool foundSlope = false;
        float closestAngle = 0.0f;
        
        for (auto& checkPos : checkPoints) {
            auto objects = playLayer->getObjectsAt(checkPos, 15.0f);
            
            for (auto obj : objects) {
                if (obj && isSlopeObject(obj)) {
                    float angle = getSlopeAngle(obj);
                    float distance = getDistanceToSlope(obj, checkPos);
                    
                    if (distance < 8.0f) {
                        foundSlope = true;
                        closestAngle = angle;
                        break;
                    }
                }
            }
            if (foundSlope) break;
        }
        
        if (foundSlope) {
            if (!m_fields->isOnSlope) {
                // Только что коснулись наклона
                m_fields->timeSinceSlope = 0.0f;
            }
            m_fields->isOnSlope = true;
            m_fields->groundAngle = closestAngle;
        } else {
            // Не на наклоне, но даем таймер на выход
            if (m_fields->timeSinceSlope > 0.05f) {
                m_fields->isOnSlope = false;
            }
        }
    }
    
    bool isSlopeObject(GameObject* obj) {
        if (!obj) return false;
        
        // Основные ID наклонных объектов
        static std::vector<int> slopeIDs = {
            4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
            93, 94, 95, 96, 97, 98, 99, 100,
            361, 362, 363, 364, 365, 366, 367, 368
        };
        
        int objectID = obj->m_objectID;
        return std::find(slopeIDs.begin(), slopeIDs.end(), objectID) != slopeIDs.end();
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
            case 93: return 45.0f;
            case 94: return -45.0f;
            case 95: return 30.0f;
            case 96: return -30.0f;
            case 97: return 60.0f;
            case 98: return -60.0f;
            case 99: return 20.0f;
            case 100: return -20.0f;
            default: return 0.0f;
        }
    }
    
    float getDistanceToSlope(GameObject* obj, CCPoint playerPos) {
        if (!obj) return 999.0f;
        
        CCPoint objPos = obj->getPosition();
        float angle = getSlopeAngle(obj);
        float radAngle = angle * M_PI / 180.0f;
        
        // Получаем размер объекта
        CCSize objSize = obj->getScaledContentSize();
        
        // Вычисляем нормаль к поверхности
        CCPoint normal = ccp(-sin(radAngle), cos(radAngle));
        
        // Смещение относительно центра объекта
        CCPoint localPos = ccpSub(playerPos, objPos);
        
        // Расстояние до плоскости наклона
        float distance = fabs(localPos.x * normal.x + localPos.y * normal.y);
        
        return distance;
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
        m_fields->timeSinceSlope = 0.0f;
        this->setRotation(0.0f);
    }
};
