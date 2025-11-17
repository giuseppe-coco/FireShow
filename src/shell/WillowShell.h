#pragma once

#include "Shell.h"

class WillowShell : public Shell
{
public:
    WillowShell(ParticleSystem &particleSystem, Firework fire, AudioManager &audioManager);
    void explode() override;
};