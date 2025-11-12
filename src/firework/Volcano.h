#pragma once
#include "Firework.h"

class Volcano : public Firework
{
public:
    Volcano(ParticleSystem &particleSystem);
    void explode() override;
    ~Volcano();
};
