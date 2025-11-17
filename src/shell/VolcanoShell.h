// VolcanoShell.h

#pragma once

#include "Shell.h"
#include "FireworkTypes.h"

class VolcanoShell : public Shell
{
public:
    VolcanoShell(ParticleSystem &particleSystem, const Firework *f, AudioManager &audioManager);
    void explode() override {}
    void Launch(const FireworkEvent &event) override;
    void Update(float dt) override;

private:
    const Firework *f;
    float angle = 45.0, emitTimer = 0.0, emitInterval = 0.2;
    unsigned int n_fires = 0;
};