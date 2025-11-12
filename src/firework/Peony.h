#pragma once

#include "Firework.h"

class Peony : public Firework
{
public:
    Peony(ParticleSystem &particleSystem);
    void explode() override;

};