#pragma once

#include "Firework.h"

class Willow : public Firework
{
public:
    Willow(ParticleSystem &particleSystem);
    void explode() override;
};