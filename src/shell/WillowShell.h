#pragma once

#include "Shell.h"

class WillowShell : public Shell
{
public:
    WillowShell(ParticleSystem &particleSystem, const Firework *f);
    void explode() override;

private:
    // Ogni PeonyShell ha bisogno di conoscere i parametri della sua esplosione.
    // Li memorizziamo qui.
    const Firework *f;
};