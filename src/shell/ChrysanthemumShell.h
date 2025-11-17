#pragma once

#include "Shell.h"

class ChrysanthemumShell : public Shell
{
public:
    ChrysanthemumShell(ParticleSystem &particleSystem, const Firework *f, AudioManager &audioManager);
    void explode() override;
private:
    const Firework *f;
};