// RingShell.h

#pragma once

#include "Shell.h"
#include "FireworkTypes.h"

class RingShell : public Shell
{
public:
    RingShell(ParticleSystem &particleSystem, Firework fire, AudioManager &audioManager);
    void Launch() override;
    void explode() override;
    void Update(float dt) override;

private:
    float emitTimer = 0.0, emitInterval = 0.2;
    unsigned int n_fires = 0;

    Particle makeParticle(float radius, unsigned int i);
};