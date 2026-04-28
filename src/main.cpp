#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(NoRotatePlayer, PlayerObject) {
    void updateRotation(float dt) {
        // Игнорируем стандартное вращение
        // Сохраняем только flipX/flipY и углы при dash-orb
        if (m_isShip || m_isBird || m_isDart || m_isSwing) {
            // Принудительно сбрасываем rotation
            this->setRotation(0.0f);

            // Проверяем dash-orb флаг
            if (m_isDashing) {
                // Поворачиваем к орбу (например, на 90°)
                this->setRotation(m_dashAngle);
            }

            // Переворот при смене направления
            if (m_isUpsideDown) {
                this->setScaleY(-fabs(this->getScaleY()));
            } else {
                this->setScaleY(fabs(this->getScaleY()));
            }
        } else {
            // Для остальных режимов оставляем стандартное поведение
            PlayerObject::updateRotation(dt);
        }
    }
};
