// VolcanoShell.cpp

#include "VolcanoShell.h"
#include "Utils.h"
#include <iostream>

#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

VolcanoShell::VolcanoShell(ParticleSystem &particleSystem, const Firework *f, AudioManager &audioManager)
    : Shell(particleSystem, audioManager), f(f) {}

void VolcanoShell::Launch(const FireworkEvent &event)
{
    this->position = event.fire.startShellPosition;
    this->state = ShellState::EXPLODING;
}

void VolcanoShell::Update(float dt)
{
    while (this->n_fires < this->f->particleCount && this->emitTimer + dt >= this->emitInterval)
    {
        audioManager.Play(f->launchSound);
        Particle star;
        star.Position = this->position;

        glm::vec4 base_direction(0.0f, 1.0f, 0.0f, 0.0f); // Usiamo vec4 per la moltiplicazione con mat4. w=0 per i vettori direzione.

        // Crea la matrice di rotazione.
        // Ruotiamo attorno all'asse Z per un ventaglio sul piano XY.
        glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

        // Ruota il vettore di base per ottenere la direzione finale.
        glm::vec3 dir = glm::vec3(rotation_matrix * base_direction);
        star.Velocity = dir * this->f->minSpeed; // For this firework this->f->minSpeed == this->f->maxSpeed

        // Le stelle del vulcano sono molto persistenti e lasciano una scia spessa
        star.Life = this->f->minLifetime;
        star.initialLife = star.Life;
        star.startColor = this->f->startColor;
        star.endColor = this->f->endColor;

        // Le stelle del vulcano sono emettitori di scie!
        star.isEmitter = true;
        star.emitInterval = 0.02f; // Scia molto densa
        star.emitTimer = 0.0f;
        star.subLife = 0.8;

        particleSystem.RespawnParticle(star);

        this->angle -= 18.0; 
        this->n_fires++;
        this->emitTimer = 0;
    }

    if (this->emitTimer + dt < this->emitInterval)
        this->emitTimer += dt;
    if (this->n_fires == this->f->particleCount)
        this->state = ShellState::INACTIVE;
}
