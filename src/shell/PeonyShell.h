#pragma once

#include "Shell.h"

class PeonyShell : public Shell
{
public:
    PeonyShell(ParticleSystem &particleSystem, const Firework *f, AudioManager &audioManager);
    void explode() override;

private:
    const Firework *f;
};