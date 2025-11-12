#include "Willow.h"
#include "Utils.h"

#include <cmath>
#include <iostream>

#include <glm/gtc/random.hpp>

// TODO: diminuire il gl_PointSize per le particelle scia dell'esplosione.
Willow::Willow(ParticleSystem &particleSystem)
    : Firework(particleSystem) {}

void Willow::explode()
{
    for (unsigned int i = 0; i < particleCount; ++i)
    {
        glm::vec3 dir = glm::ballRand(1.0f);

        // Vogliamo solo particelle che vadano verso l'alto.
        // Controlliamo semplicemente se la componente y è positiva.
        // dir.y > 0 significa un angolo di elevazione tra 0 e 90 gradi.
        if (dir.y <= 0.1) continue;

        Particle p;
        p.Position = this->position;
        p.gravityModifier = this->gravityModifier;
        /*
        (minSpeed + maxSpeed) / 2.0 è il punto medio tra minSpeed e maxSpeed
        (minSpeed + maxSpeed) / 6.0 fa sì che il 99% dei punti cadano dentro l'intervallo (minSpeed, maxSpeed)
        */
        float speed = sampleGaussian((minSpeed + maxSpeed) / 2.0, (minSpeed - maxSpeed) / 6.0);
        p.Velocity = dir * speed;
        p.startColor = startColor;
        p.endColor = endColor;
        p.Life = sampleGaussian((minLifetime + maxLifetime) / 2.0, (minLifetime - maxLifetime) / 6.0);
        p.initialLife = p.Life; // Salva la vita iniziale per l'interpolazione
        p.isEmitter = true;
        p.emitInterval = 0.02f; // Emette una scintilla ogni 0.02 secondi
        p.emitTimer = 0.0f;
        p.subLife = 2.0; 
        particleSystem.RespawnParticle(p);        
    }
}