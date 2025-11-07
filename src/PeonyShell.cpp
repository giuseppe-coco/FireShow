#include "PeonyShell.h"
#include <glm/gtc/random.hpp>

// Il costruttore inizializza la classe base e memorizza il tipo di fuoco.
PeonyShell::PeonyShell(ParticleSystem &particleSystem, const FireworkType *type)
    : Shell(particleSystem), type(type) {}

void PeonyShell::explode()
{
    // Sicurezza: non fare nulla se il tipo non è valido.
    if (!type)
        return;

    for (unsigned int i = 0; i < type->particleCount; ++i)
    {
        Particle p;
        p.Position = this->position;
        float speed = type->minSpeed + (rand() % 100 / 100.0f) * (type->maxSpeed - type->minSpeed);
        p.Velocity = glm::ballRand(1.0f) * speed;
        p.startColor = type->startColor;
        p.endColor = type->endColor;
        p.Life = type->minLifetime + (rand() % 100 / 100.0f) * (type->maxLifetime - type->minLifetime);
        p.initialLife = p.Life; // Salva la vita iniziale per l'interpolazione
        p.gravityModifier = type->gravityModifier;

        particleSystem.RespawnParticle(p);
    }
}