#pragma once

#include "Shell.h"

class PeonyShell : public Shell
{
public:
    PeonyShell(ParticleSystem &particleSystem, const Firework & fire, AudioManager &audioManager);
    void explode() override;
};