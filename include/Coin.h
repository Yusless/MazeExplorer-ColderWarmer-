#ifndef COIN_H
#define COIN_H

#include <raylib.h>

class Coin {
public:
    Coin(Vector3 position);
    void Draw() const;
    BoundingBox GetBoundingBox() const;
    bool IsCollected() const { return collected; }
    void Collect() { collected = true; }
    Vector3 GetPosition() const { return position; }

private:
    Vector3 position;
    bool collected;
    float radius;
};

#endif