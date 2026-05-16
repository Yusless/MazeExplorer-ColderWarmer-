#include "SlotMachine.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

SlotMachine::SlotMachine()
    : m_active(false),
      m_playerCoins(0),
      m_stake(0),
      m_waitingForStake(true),
      m_stakeInputLen(0),
      m_spinState(0),
      m_currentReel(0),
      m_spinStartTime(0),
      m_spinDuration(1.5f),
      m_spinning(false),
      m_animationTimer(0),
      coins_are_given(false)
{
    std::srand(std::time(nullptr));
    m_reels.resize(3, SlotSymbol::COIN);
    m_targetReels.resize(3, SlotSymbol::COIN);
    m_stakeInput[0] = '\0';
}

void SlotMachine::Start(int playerCoins) {
    m_playerCoins = playerCoins;
    m_active = true;
    m_waitingForStake = true;
    m_spinState = 0;
    m_currentReel = 0;
    m_spinStartTime = 0;
    m_spinning = false;
    m_stake = 0;
    m_stakeInputLen = 0;
    m_stakeInput[0] = '\0';
    for (int i = 0; i < 3; ++i) {
        m_reels[i] = SlotSymbol::COIN;
        m_targetReels[i] = SlotSymbol::COIN;
    }
}

void SlotMachine::Update() {
    if (!m_active) return;

    // 1. Ввод ставки
    if (m_waitingForStake) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= '0' && key <= '9' && m_stakeInputLen < 15) {
                m_stakeInput[m_stakeInputLen++] = (char)key;
                m_stakeInput[m_stakeInputLen] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && m_stakeInputLen > 0) {
            m_stakeInput[--m_stakeInputLen] = '\0';
        }
        if (IsKeyPressed(KEY_ENTER) && m_stakeInputLen > 0) {
            m_stake = atoi(m_stakeInput);
            if (m_stake <= 0 || m_stake > m_playerCoins) {
                m_stakeInputLen = 0;
                m_stakeInput[0] = '\0';
                return;
            }
            if (m_addCoinsCallback) m_addCoinsCallback(-m_stake);
            m_playerCoins -= m_stake;
            m_waitingForStake = false;
            m_spinState = 1;
        }
        return;
    }

    // 2. Ожидание SPACE для запуска
    if (m_spinState == 1) {
        if (IsKeyPressed(KEY_SPACE)) {
            m_spinning = true;
            m_spinStartTime = GetTime();
            m_currentReel = 0;
            GenerateRandomTargets();
            m_spinState = 2;
        }
        return;
    }

    // 3. Вращение
    if (m_spinState == 2) {
        if (GetTime() - m_animationTimer > 0.07f) {
            m_animationTimer = GetTime();
            for (int i = m_currentReel; i < 3; ++i) {
                m_reels[i] = RandomSymbol();
            }
        }
        if (IsKeyPressed(KEY_SPACE)) {
            m_reels[m_currentReel] = m_targetReels[m_currentReel];
            m_currentReel++;
            if (m_currentReel >= 3) {
                m_spinState = 3;
            }
        }
        if (GetTime() - m_spinStartTime > 5.0f && m_currentReel < 3) {
            while (m_currentReel < 3) {
                m_reels[m_currentReel] = m_targetReels[m_currentReel];
                m_currentReel++;
            }
            m_spinState = 3;
        }
        return;
    }

    // 4. Результат
    if (m_spinState == 3) {
        if (!coins_are_given) {
            ApplyResult();
            coins_are_given = true;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            m_spinState = 4;
            coins_are_given = false;
        }
        return;
    }

    // 5. Ожидание выхода
    if (m_spinState == 4) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE)) {
            m_active = false;
        }
    }
}

void SlotMachine::GenerateRandomTargets() {
    for (int i = 0; i < 3; ++i) {
        m_targetReels[i] = RandomSymbol();
    }
}

SlotSymbol SlotMachine::RandomSymbol() {
    int r = rand() % 3;
    if (r == 0) return SlotSymbol::COIN;
    if (r == 1) return SlotSymbol::RED_CROSS;
    return SlotSymbol::RED_CIRCLE;
}

// ========== ИЗМЕНЁННАЯ ФУНКЦИЯ С ПРОИГРЫВАНИЕМ ЗВУКОВ ==========
void SlotMachine::ApplyResult() {
    bool allCoins = (m_reels[0] == SlotSymbol::COIN && m_reels[1] == SlotSymbol::COIN && m_reels[2] == SlotSymbol::COIN);
    bool allRedCircles = (m_reels[0] == SlotSymbol::RED_CIRCLE && m_reels[1] == SlotSymbol::RED_CIRCLE && m_reels[2] == SlotSymbol::RED_CIRCLE);
    bool allRedCrosses = (m_reels[0] == SlotSymbol::RED_CROSS && m_reels[1] == SlotSymbol::RED_CROSS && m_reels[2] == SlotSymbol::RED_CROSS);

    if (allCoins || allRedCircles || allRedCrosses) {
        int win = m_stake * 2;
        if (m_addCoinsCallback) m_addCoinsCallback(win);
        PlaySound(m_winSound);
    } else {
        PlaySound(m_loseSound);
    }
}
// =============================================================

void SlotMachine::Draw() {
    if (!m_active) return;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));

    int cx = GetScreenWidth() / 2;
    int cy = GetScreenHeight() / 2;

    if (m_waitingForStake) {
        DrawText("ENTER YOUR BET", cx - 120, cy - 100, 40, YELLOW);
        DrawText(TextFormat("Your coins: %d", m_playerCoins), cx - 100, cy - 40, 25, WHITE);
        DrawRectangle(cx - 100, cy, 200, 40, LIGHTGRAY);
        DrawText(m_stakeInput, cx - 90, cy + 10, 30, BLACK);
        DrawText("Press ENTER to confirm", cx - 130, cy + 70, 20, GRAY);
        if (m_stake > m_playerCoins && m_stakeInputLen == 0) {
            DrawText("Not enough coins!", cx - 100, cy + 120, 20, RED);
        }
    } else if (m_spinState == 1) {
        DrawText("Press SPACE to spin the reels", cx - 150, cy, 25, WHITE);
    } else if (m_spinState == 2 || m_spinState == 3) {
        int reelW = 100, reelH = 150;
        int startX = cx - 150;
        int y = cy - 75;
        for (int i = 0; i < 3; ++i) {
            DrawRectangle(startX + i * reelW, y, reelW, reelH, DARKGRAY);
            DrawRectangleLines(startX + i * reelW, y, reelW, reelH, GOLD);
            int centerX = startX + i * reelW + reelW/2;
            int centerY = y + reelH/2;
            int radius = 30;
            if (m_reels[i] == SlotSymbol::COIN) {
                DrawCircle(centerX, centerY, radius, YELLOW);
                DrawCircle(centerX - 8, centerY - 8, 6, GOLD);
            } else if (m_reels[i] == SlotSymbol::RED_CROSS) {
                DrawCircle(centerX, centerY, radius, RED);
                DrawLine(centerX - radius/2, centerY - radius/2, centerX + radius/2, centerY + radius/2, WHITE);
                DrawLine(centerX - radius/2, centerY + radius/2, centerX + radius/2, centerY - radius/2, WHITE);
            } else if (m_reels[i] == SlotSymbol::RED_CIRCLE) {
                DrawCircle(centerX, centerY, radius, RED);
                DrawCircleLines(centerX, centerY, radius-3, WHITE);
            }
        }
        if (m_spinState == 2) {
            DrawText("Press SPACE to stop each reel", cx - 150, cy + 100, 20, WHITE);
        } else {
            bool allCoins = (m_reels[0] == SlotSymbol::COIN && m_reels[1] == SlotSymbol::COIN && m_reels[2] == SlotSymbol::COIN);
            bool allRedCircles = (m_reels[0] == SlotSymbol::RED_CIRCLE && m_reels[1] == SlotSymbol::RED_CIRCLE && m_reels[2] == SlotSymbol::RED_CIRCLE);
            bool allRedCrosses = (m_reels[0] == SlotSymbol::RED_CROSS && m_reels[1] == SlotSymbol::RED_CROSS && m_reels[2] == SlotSymbol::RED_CROSS);
            
            DrawText("RESULT", cx - 40, cy - 120, 40, YELLOW);
            if (allCoins || allRedCircles || allRedCrosses) {
                DrawText(TextFormat("YOU WIN %d!", m_stake * 2), cx - 100, cy + 120, 30, GREEN);
            } else {
                DrawText("YOU LOSE!", cx - 80, cy + 120, 30, RED);
            }
            DrawText("Press any key to continue...", cx - 130, cy + 180, 20, WHITE);
        }
    } else if (m_spinState == 4) {
        DrawText("Press any key to continue...", cx - 130, cy, 25, WHITE);
    }
}