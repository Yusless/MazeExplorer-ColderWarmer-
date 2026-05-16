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
    void Start(int playerCoins);
    void Update();
    void Draw();
    bool IsActive() const { return m_active; }
    void Close() { m_active = false; }
    void SetAddCoinsCallback(std::function<void(int)> callback) { m_addCoinsCallback = callback; }
    void SetWinSound(Sound sound) { m_winSound = sound; }
    void SetLoseSound(Sound sound) { m_loseSound = sound; }

private:
    bool m_active;
    int m_playerCoins;
    int m_stake;
    bool m_waitingForStake;
    char m_stakeInput[16];
    int m_stakeInputLen;
    int m_spinState;
    int m_currentReel;
    float m_spinStartTime;
    float m_spinDuration;
    std::vector<SlotSymbol> m_reels;
    std::vector<SlotSymbol> m_targetReels;
    bool m_spinning;
    float m_animationTimer;
    bool coins_are_given;
    
    std::function<void(int)> m_addCoinsCallback;
    Sound m_winSound;
    Sound m_loseSound;
    
    void GenerateRandomTargets();
    SlotSymbol RandomSymbol();
    void ApplyResult();
};

#endif