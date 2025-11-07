#pragma once

#include "Shell.h"

class PeonyShell : public Shell
{
public:
    // Il costruttore chiama semplicemente il costruttore della classe base.
    PeonyShell(ParticleSystem &particleSystem, const FireworkType *type);

    // 'override' è una parola chiave di C++11 che dice al compilatore
    // la nostra intenzione di sovrascrivere un metodo virtuale della classe base.
    // Aiuta a prevenire errori di battitura nel nome del metodo.
    void explode() override;

private:
    // Ogni PeonyShell ha bisogno di conoscere i parametri della sua esplosione.
    // Li memorizziamo qui.
    const FireworkType *type;
};