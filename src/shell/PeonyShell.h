#pragma once

#include "Shell.h"

class PeonyShell : public Shell
{
public:
    PeonyShell(ParticleSystem &particleSystem, Firework fire, AudioManager &audioManager);
    void explode() override;
};