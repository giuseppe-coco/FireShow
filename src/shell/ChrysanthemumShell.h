#pragma once

#include "Shell.h"

class ChrysanthemumShell : public Shell
{
public:
    ChrysanthemumShell(ParticleSystem &particleSystem, const Firework &fire, AudioManager &audioManager);
    void explode() override;
};