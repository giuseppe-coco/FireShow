#include "ChrysanthemumShell.h"
#include "Utils.h"
#include <iostream>

#include <glm/gtc/random.hpp>

// Il costruttore inizializza la classe base e memorizza il tipo di fuoco.
ChrysanthemumShell::ChrysanthemumShell(ParticleSystem &particleSystem, Firework fire, AudioManager &audioManager)
    : Shell(fire, particleSystem, audioManager) {}

void ChrysanthemumShell::explode()
{
    // if (!fire) return;
    audioManager.Play(fire.explosionSound);
    for (unsigned int i = 0; i < fire.particleCount; ++i)
    {
        Particle p;
        p.Position = this->position;
        /*  (fire.minSpeed + fire.maxSpeed) / 2.0 è il punto medio tra minSpeed e maxSpeed
            (fire.minSpeed + fire.maxSpeed) / 6.0 fa sì che il 99% dei punti cadano dentro l'intervallo (fire.minSpeed, fire.maxSpeed)
        */
        float speed = sampleGaussian((fire.minSpeed + fire.maxSpeed) / 2.0, (fire.minSpeed - fire.maxSpeed) / 6.0);
        p.Velocity = glm::ballRand(1.0f) * speed;
        p.startColor = fire.startColor;
        p.endColor = fire.endColor;
        p.Life = sampleGaussian((fire.minLifetime + fire.maxLifetime) / 2.0, (fire.minLifetime - fire.maxLifetime) / 6.0);
        p.initialLife = p.Life; // Salva la vita iniziale per l'interpolazione
        p.isEmitter = true;
        p.emitInterval = 0.02f; // Emette una scintilla ogni 0.02 secondi
        p.emitTimer = 0.0f;
        p.subLife = 0.4;
        
        particleSystem.RespawnParticle(p);
    }
}