#include "Shell.h"
#include "FireworkTypes.h"
#include "PeonyShell.h"
#include "ChrysanthemumShell.h"
#include "VolcanoShell.h"
#include "RingShell.h"

#include <glm/gtc/random.hpp>
#include <memory>
#include <iostream>
#include "WillowShell.h"

Shell::Shell(ParticleSystem &ps, AudioManager &audioManager)
    : particleSystem(ps), audioManager(audioManager), state(ShellState::INACTIVE) {}

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
    if (state == ShellState::INACTIVE)
        return;

    // Se il proiettile sta ancora salendo
    if (state == ShellState::RISING)
    {
        fuse -= dt; // Consuma la miccia
        position += velocity * dt;
        velocity.y -= 9.81f * 1.0 * dt; // TODO: check gravity value

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
    trailParticle.initialLife = trailParticle.Life;

    // Colori per la scia (es. da arancione brillante a grigio fumo scuro)
    trailParticle.startColor = glm::vec3(1.0f, 0.8f, 0.5f);
    trailParticle.endColor = glm::vec3(0.2f, 0.2f, 0.2f);

    particleSystem.RespawnParticle(trailParticle);
}

std::unique_ptr<Shell> Shell::createShell(const Firework *f, ParticleSystem &ps, AudioManager &audio)
{
    switch (f->family)
    {
    case FireworkFamily::Peony:
        return std::make_unique<PeonyShell>(ps, f, audio);
    case FireworkFamily::Chrysanthemum:
        return std::make_unique<ChrysanthemumShell>(ps, f, audio);
    case FireworkFamily::Willow: 
    { // separated scope
        Shader willowShader("shaders/particle.vert", "shaders/particle.frag");
        willowShader.setInt("firework_type", static_cast<int>(FireworkFamily::Willow));
        ps.shader = willowShader;
        return std::make_unique<WillowShell>(ps, f, audio);
    }
    case FireworkFamily::Volcano:
    {
        Shader volcanoShader("shaders/particle.vert", "shaders/particle.frag");
        volcanoShader.setInt("firework_type", static_cast<int>(FireworkFamily::Volcano));
        ps.shader = volcanoShader;
        return std::make_unique<VolcanoShell>(ps, f, audio);
    }
    case FireworkFamily::Ring:
        return std::make_unique<RingShell>(ps, f, audio);
    default:
        return std::make_unique<PeonyShell>(ps, f, audio);
    }
}