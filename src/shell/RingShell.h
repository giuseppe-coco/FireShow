// RingShell.h

#pragma once

#include "Shell.h"
#include "FireworkTypes.h"

class RingShell : public Shell
{
public:
    RingShell(ParticleSystem &particleSystem, const Firework *f);
    void explode() override;
    void Update(float dt) override;

private:
    const Firework *f;
    float emitTimer = 0.0, emitInterval = 0.2;
    unsigned int n_fires = 0;

    Particle makeParticle(float radius, unsigned int i);
};