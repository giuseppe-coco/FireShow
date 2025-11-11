#pragma once

#include "Shell.h"

class ChrysanthemumShell : public Shell
{
public:
    // Il costruttore chiama semplicemente il costruttore della classe base.
    ChrysanthemumShell(ParticleSystem &particleSystem, const Firework *f);

    // 'override' è una parola chiave di C++11 che dice al compilatore
    // la nostra intenzione di sovrascrivere un metodo virtuale della classe base.
    // Aiuta a prevenire errori di battitura nel nome del metodo.
    void explode() override;

private:
    // Ogni ChrysanthemumShell ha bisogno di conoscere i parametri della sua esplosione.
    // Li memorizziamo qui.
    const Firework *f;
};