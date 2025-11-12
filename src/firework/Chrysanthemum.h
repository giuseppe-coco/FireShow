#pragma once

#include "Firework.h"

class Chrysanthemum : public Firework
{
public:
    // Il costruttore chiama semplicemente il costruttore della classe base.
    Chrysanthemum(ParticleSystem &particleSystem);

    // 'override' è una parola chiave di C++11 che dice al compilatore
    // la nostra intenzione di sovrascrivere un metodo virtuale della classe base.
    // Aiuta a prevenire errori di battitura nel nome del metodo.
    void explode() override;
};