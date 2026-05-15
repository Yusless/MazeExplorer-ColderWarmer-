#ifndef SLOT_MACHINE_H
#define SLOT_MACHINE_H

#include <raylib.h>
#include <functional>
#include <vector>
#include <string>

enum class SlotSymbol { COIN, RED_CROSS, RED_CIRCLE };

class SlotMachine {
public:
    SlotMachine();
    void Start(int playerCoins);                         // запуск мини-игры
    void Update();                                       // обновление каждый кадр
    void Draw();                                         // отрисовка 2D-интерфейса
    bool IsActive() const { return m_active; }
    void Close() { m_active = false; }
    void SetAddCoinsCallback(std::function<void(int)> callback) { m_addCoinsCallback = callback; }

private:
    bool m_active;
    int m_playerCoins;                                   // копия баланса (синхронизируется через callback)
    int m_stake;                                         // текущая ставка
    bool m_waitingForStake;
    char m_stakeInput[16];
    int m_stakeInputLen;
    int m_spinState; // 0=ожидание ставки, 1=спин разрешён, 2=вращаем барабаны, 3=ждём остановку, 4=показ результата
    int m_currentReel;                                   // какой барабан останавливаем (0,1,2)
    float m_spinStartTime;
    float m_spinDuration;
    std::vector<SlotSymbol> m_reels;                     // текущие отображаемые символы (для анимации)
    std::vector<SlotSymbol> m_targetReels;               // финальные символы (после остановки)
    bool m_spinning;
    float m_animationTimer;
    
    std::function<void(int)> m_addCoinsCallback;
    
    void GenerateRandomTargets();
    SlotSymbol RandomSymbol();
    void ApplyResult();
};

#endif