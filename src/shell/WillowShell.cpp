#include "WillowShell.h"
#include "Utils.h"

#include <cmath>
#include <iostream>

#include <glm/gtc/random.hpp>

// TODO: diminuire il gl_PointSize per le particelle scia dell'esplosione.
WillowShell::WillowShell(ParticleSystem &particleSystem, Firework fire, AudioManager &audioManager)
    : Shell(fire, particleSystem, audioManager) {}

void WillowShell::explode()
{
    // if (!f) return;
    audioManager.Play(fire.explosionSound);
    for (unsigned int i = 0; i < fire.particleCount; ++i)
    {
        glm::vec3 dir = glm::ballRand(1.0f);

        // Vogliamo solo particelle che vadano verso l'alto.
        // Controlliamo semplicemente se la componente y è positiva.
        // dir.y > 0 significa un angolo di elevazione tra 0 e 90 gradi.
        if (dir.y <= 0.1) continue;

        Particle p;
        p.Position = this->position;
        p.gravityModifier = this->fire.gravityModifier;
        /*
        (fire.minSpeed + fire.maxSpeed) / 2.0 è il punto medio tra minSpeed e maxSpeed
        (fire.minSpeed + fire.maxSpeed) / 6.0 fa sì che il 99% dei punti cadano dentro l'intervallo (fire.minSpeed, fire.maxSpeed)
        */
        float speed = sampleGaussian((fire.minSpeed + fire.maxSpeed) / 2.0, (fire.minSpeed - fire.maxSpeed) / 6.0);
        p.Velocity = dir * speed;
        p.startColor = fire.startColor;
        p.endColor = fire.endColor;
        p.Life = sampleGaussian((fire.minLifetime + fire.maxLifetime) / 2.0, (fire.minLifetime - fire.maxLifetime) / 6.0);
        p.initialLife = p.Life; // Salva la vita iniziale per l'interpolazione
        p.isEmitter = true;
        p.emitInterval = 0.02f; // Emette una scintilla ogni 0.02 secondi
        p.emitTimer = 0.0f;
        p.subLife = 2.0; 
        particleSystem.RespawnParticle(p);        
    }
}
