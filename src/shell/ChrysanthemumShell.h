#pragma once

#include "Shell.h"

class ChrysanthemumShell : public Shell
{
public:
    ChrysanthemumShell(ParticleSystem &particleSystem, Firework fire, AudioManager &audioManager);
    void explode() override;
};