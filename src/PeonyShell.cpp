#include "PeonyShell.h"
#include "Utils.h"
#include <iostream>

#include <glm/gtc/random.hpp>

// Il costruttore inizializza la classe base e memorizza il tipo di fuoco.
PeonyShell::PeonyShell(ParticleSystem &particleSystem, const Firework *type)
    : Shell(particleSystem), type(type) {}

void PeonyShell::explode()
{
    if (!type)
        return;
    for (unsigned int i = 0; i < type->particleCount; ++i)
    {
        Particle p;
        p.Position = this->position;
        float speed = sampleGaussian((type->minSpeed + type->maxSpeed) / 2.0, (type->minSpeed - type->maxSpeed) / 6.0);
        p.Velocity = glm::ballRand(1.0f) * speed;
        p.startColor = type->startColor;
        p.endColor = type->endColor;
        p.Life = sampleGaussian((type->minLifetime + type->maxLifetime) / 2.0, (type->minLifetime - type->maxLifetime) / 6.0);
        p.initialLife = p.Life; // Salva la vita iniziale per l'interpolazione

        particleSystem.RespawnParticle(p);
    }
}