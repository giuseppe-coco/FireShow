#include "FireworksShell.h"
#include <glm/gtc/random.hpp>

FireworksShell::FireworksShell(ParticleSystem &ps)
    : particleSystem(ps), state(ShellState::INACTIVE) {}

void FireworksShell::Launch(glm::vec3 startPosition, glm::vec3 startVelocity, float fuseTime)
{
    this->position = startPosition;
    this->velocity = startVelocity;
    this->fuse = fuseTime;
    this->trailTimer = 0.0f; // Resetta il timer della scia
    this->state = ShellState::RISING;
}

void FireworksShell::Update(float dt)
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
            state = ShellState::EXPLODED;
        }
    }
    // NOTA: Una volta esploso, il proiettile diventa effettivamente "morto".
    // Potremmo aggiungere uno stato 'EXPLODED' che dura qualche secondo prima
    // di diventare 'INACTIVE', ma per ora lo semplifichiamo.
}

void FireworksShell::emitTrailParticle()
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

    particleSystem.RespawnParticle(trailParticle);
}

void FireworksShell::explode()
{
    // Quando esplode, genera un gran numero di particelle
    // tutte in una volta, usando la posizione corrente del proiettile.
    unsigned int explosionParticles = 500;
    for (unsigned int i = 0; i < explosionParticles; ++i)
    {
        Particle p;
        p.Position = this->position;
        float randomForce = 15.0f + (rand() % 100) / 100.0f * 10.0f; // Forza tra 15 e 25
        p.Velocity = glm::ballRand(1.0f) * randomForce;
        p.Color = glm::vec4(0.5f + (rand() % 100) / 200.0f, 0.5f + (rand() % 100) / 200.0f, 1.0f, 1.0f); // Colori blu/viola
        p.Life = 1.0f + (rand() % 100) / 100.0f;

        particleSystem.RespawnParticle(p);
    }
}