#include "Shell.h"
#include "FireworkTypes.h"
#include <glm/gtc/random.hpp>

Shell::Shell(ParticleSystem &ps)
    : particleSystem(ps), state(ShellState::INACTIVE) {}

void Shell::Launch(const FireworkEvent &event, const FireworkType *type)
{
    this->position = event.startPosition;
    this->velocity = event.startVelocity;
    this->fuse = event.fuseTime;
    this->explosionType = type; // Salva il tipo di fuoco per l'esplosione
    this->trailTimer = 0.0f; // resetta il timer della scia
    this->state = ShellState::RISING;
}

void Shell::Update(float dt)
{
    // Se il proiettile non è attivo, non fare nulla.
    if (state == ShellState::INACTIVE)
        return;

    // Se il proiettile sta ancora salendo
    if (state == ShellState::RISING)
    {
        fuse -= dt; // Consuma la miccia

        position += velocity * dt;
        velocity.y -= 9.81f * 0.5f * dt;

        // Logica per emettere la scia di particelle
        trailTimer += dt;
        if (trailTimer > 0.02f) // Emetti una particella di scia ogni 0.02 secondi
        {
            emitTrailParticle();
            trailTimer = 0.0f; // Resetta il timer
        }

        // Controlla se la miccia è finita
        if (fuse <= 0.0f)
        {
            explode();
            state = ShellState::INACTIVE;
        }
    }
}

void Shell::emitTrailParticle()
{
    // Chiediamo al sistema di particelle di "rianimare" una delle sue particelle morte
    // e di trasformarla in una particella di scia.

    // Parametri per la particella di scia
    Particle trailParticle;
    trailParticle.Position = this->position;
    // La velocità è leggermente casuale e rivolta all'indietro/in basso
    trailParticle.Velocity = -this->velocity * 0.1f + glm::ballRand(0.5f);
    trailParticle.Color = glm::vec4(1.0f, 0.8f, 0.5f, 1.0f); // Colore da scintilla
    trailParticle.Life = 0.5f;                               // Vita breve per la scia
    trailParticle.initialLife = 0.5f;
    
    // Colori per la scia (es. da arancione brillante a grigio fumo scuro)
    trailParticle.startColor = glm::vec3(1.0f, 0.8f, 0.5f);
    trailParticle.endColor = glm::vec3(0.2f, 0.2f, 0.2f);

    // La scia dovrebbe essere influenzata dalla gravità normale
    trailParticle.gravityModifier = 1.0f; // <<-- IMPORTANTE: Dagli un valore sensato!

    particleSystem.RespawnParticle(trailParticle);
}

// Create `explosionType->particleCount` `Particle`s
void Shell::explode()
{
    if (!explosionType) return; // Sicurezza: non esplodere se non c'è un tipo definito

    for (unsigned int i = 0; i < explosionType->particleCount; ++i)
    {
        Particle p;
        p.Position = this->position;

        float speed = explosionType->minSpeed + (rand() % 100 / 100.0f) * (explosionType->maxSpeed - explosionType->minSpeed);
        p.Velocity = glm::ballRand(1.0f) * speed;

        p.startColor = explosionType->startColor;
        p.endColor = explosionType->endColor;

        p.Life = explosionType->minLifetime + (rand() % 100 / 100.0f) * (explosionType->maxLifetime - explosionType->minLifetime);
        p.initialLife = p.Life; // Salva la vita iniziale per l'interpolazione

        p.gravityModifier = explosionType->gravityModifier;

        particleSystem.RespawnParticle(p);
    }
}