#include "Shell.h"
#include "FireworkTypes.h"
#include "PeonyShell.h"
#include "ChrysanthemumShell.h"

#include <glm/gtc/random.hpp>
#include <memory>
#include <iostream>

Shell::Shell(ParticleSystem &ps)
    : particleSystem(ps), state(ShellState::INACTIVE) {}

void Shell::Launch(const FireworkEvent &event)
{
    this->position = event.fire.startShellPosition;
    this->velocity = event.fire.startShellVelocity;
    this->fuse = event.fire.fuseTime;
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

// Chiediamo al sistema di particelle di "rianimare" una delle sue particelle morte
// e di trasformarla in una particella di scia.
void Shell::emitTrailParticle()
{
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

    particleSystem.RespawnParticle(trailParticle);
}

std::unique_ptr<Shell> Shell::createShell(const Firework* f, ParticleSystem &ps)
{
    switch (f->family)
    {
    case FireworkFamily::Peony:
        return std::make_unique<PeonyShell>(ps, f);
    case FireworkFamily::Chrysanthemum:
        return std::make_unique<ChrysanthemumShell>(ps, f);
    // case FireworkFamily::Willow:
    //     return std::make_unique<WillowShell>(ps, f);
    default:
        // Ritorna un tipo di default o nullptr se il tipo non è riconosciuto
        return std::make_unique<PeonyShell>(ps, f);
    }
}