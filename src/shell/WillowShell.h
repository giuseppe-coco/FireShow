#pragma once

#include "Shell.h"

class WillowShell : public Shell
{
public:
    WillowShell(ParticleSystem &particleSystem, const Firework *f, AudioManager &audioManager);
    void explode() override;
private:
    const Firework *f;
};