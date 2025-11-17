#include "WillowShell.h"
#include "Utils.h"

#include <cmath>
#include <iostream>

#include <glm/gtc/random.hpp>

// TODO: diminuire il gl_PointSize per le particelle scia dell'esplosione.
WillowShell::WillowShell(ParticleSystem &particleSystem, const Firework *f, AudioManager &audioManager)
    : Shell(particleSystem, audioManager), f(f) {}

void WillowShell::explode()
{
    if (!f) return;
    audioManager.Play(f->explosionSound);
    for (unsigned int i = 0; i < f->particleCount; ++i)
    {
        glm::vec3 dir = glm::ballRand(1.0f);

        // Vogliamo solo particelle che vadano verso l'alto.
        // Controlliamo semplicemente se la componente y è positiva.
        // dir.y > 0 significa un angolo di elevazione tra 0 e 90 gradi.
        if (dir.y <= 0.1) continue;

        Particle p;
        p.Position = this->position;
        p.gravityModifier = this->f->gravityModifier;
        /*
        (f->minSpeed + f->maxSpeed) / 2.0 è il punto medio tra minSpeed e maxSpeed
        (f->minSpeed + f->maxSpeed) / 6.0 fa sì che il 99% dei punti cadano dentro l'intervallo (f->minSpeed, f->maxSpeed)
        */
        float speed = sampleGaussian((f->minSpeed + f->maxSpeed) / 2.0, (f->minSpeed - f->maxSpeed) / 6.0);
        p.Velocity = dir * speed;
        p.startColor = f->startColor;
        p.endColor = f->endColor;
        p.Life = sampleGaussian((f->minLifetime + f->maxLifetime) / 2.0, (f->minLifetime - f->maxLifetime) / 6.0);
        p.initialLife = p.Life; // Salva la vita iniziale per l'interpolazione
        p.isEmitter = true;
        p.emitInterval = 0.02f; // Emette una scintilla ogni 0.02 secondi
        p.emitTimer = 0.0f;
        p.subLife = 2.0; 
        particleSystem.RespawnParticle(p);        
    }
}
