#include "ChrysanthemumShell.h"
#include "Utils.h"
#include <iostream>

#include <glm/gtc/random.hpp>

// Il costruttore inizializza la classe base e memorizza il tipo di fuoco.
ChrysanthemumShell::ChrysanthemumShell(ParticleSystem &particleSystem, const Firework *f)
    : Shell(particleSystem), f(f) {}

void ChrysanthemumShell::explode()
{
    std::cout << "[DEBUG] ChrysanthemumShell::explode() called." << std::endl;
    if (!f)
    {
        std::cerr << "[FATAL ERROR] ChrysanthemumShell::explode() called, but 'f' is nullptr!" << std::endl;
        return;
    }

    std::cout << "[DEBUG]   > Accessing firework: " << f->name
              << " | At address: " << f << std::endl;
    if (!f)
        return;
        
    for (unsigned int i = 0; i < f->particleCount; ++i)
    {
        Particle p;
        p.Position = this->position;
        /*  (f->minSpeed + f->maxSpeed) / 2.0 è il punto medio tra minSpeed e maxSpeed
            (f->minSpeed + f->maxSpeed) / 6.0 fa sì che il 99% dei punti cadano dentro l'intervallo (f->minSpeed, f->maxSpeed)
        */
        float speed = sampleGaussian((f->minSpeed + f->maxSpeed) / 2.0, (f->minSpeed - f->maxSpeed) / 6.0);
        p.Velocity = glm::ballRand(1.0f) * speed;
        p.startColor = f->startColor;
        p.endColor = f->endColor;
        p.Life = sampleGaussian((f->minLifetime + f->maxLifetime) / 2.0, (f->minLifetime - f->maxLifetime) / 6.0);
        p.initialLife = p.Life; // Salva la vita iniziale per l'interpolazione
        p.isEmitter = true;
        p.emitInterval = 0.02f; // Emette una scintilla ogni 0.02 secondi
        p.emitTimer = 0.0f;
        p.subLife = 0.4;
        
        particleSystem.RespawnParticle(p);
    }
}