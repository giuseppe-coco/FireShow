#include "Peony.h"
#include "Utils.h"
#include <iostream>

#include <glm/gtc/random.hpp>

// Il costruttore inizializza la classe base e memorizza il tipo di fuoco.
Peony::Peony(ParticleSystem &particleSystem)
    : Firework(particleSystem) {}

void Peony::explode()
{
    for (unsigned int i = 0; i < particleCount; ++i)
    {
        Particle p;
        p.Position = this->position;
        /*  (minSpeed + maxSpeed) / 2.0 è il punto medio tra minSpeed e maxSpeed
            (minSpeed + maxSpeed) / 6.0 fa sì che il 99% dei punti cadano dentro l'intervallo (minSpeed, maxSpeed)
        */
        float speed = sampleGaussian((minSpeed + maxSpeed) / 2.0, (minSpeed - maxSpeed) / 6.0);
        p.Velocity = glm::ballRand(1.0f) * speed;
        p.startColor = startColor;
        p.endColor = endColor;
        p.Life = sampleGaussian((minLifetime + maxLifetime) / 2.0, (minLifetime - maxLifetime) / 6.0);
        p.initialLife = p.Life; // Salva la vita iniziale per l'interpolazione

        particleSystem.RespawnParticle(p);
    }
}