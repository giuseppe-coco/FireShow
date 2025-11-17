// RingShell.cpp

#include "RingShell.h"
#include "Utils.h"
#include <iostream>

#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

RingShell::RingShell(ParticleSystem &particleSystem, Firework fire, AudioManager &audioManager)
    : Shell(fire, particleSystem, audioManager) {}

void RingShell::Launch()
{
    this->position = fire.startShellPosition;
    this->velocity = fire.startShellVelocity;
    this->fuse = fire.fuseTime;
    this->trailTimer = 0.0f; // resetta il timer della scia
    this->state = ShellState::RISING;
}

void RingShell::Update(float dt)
{
    if (state == ShellState::INACTIVE) return;
    if (state == ShellState::RISING)
    {
        fuse -= dt;
        position += velocity * dt;
        velocity.y -= 9.81f * 1.0 * dt; // TODO: check gravity value
        if (fuse <= 0.0f)
        {
            explode();
            state = ShellState::INACTIVE;
        }
    }
}

void RingShell::explode()
{
    // if (!f) return;
    audioManager.Play(fire.explosionSound);
    float radius = 2.0;
    for (unsigned int i = 0; i < fire.particleCount; ++i)
    {
        Particle p;
        p = makeParticle(radius, i);        
        particleSystem.RespawnParticle(p);
    }
}

Particle RingShell::makeParticle(float radius, unsigned int i)
{
    Particle p;
    float theta = 2.0 * glm::pi<float>() * ((float)i / (float)fire.particleCount);
    p.Position.x = this->position.x + radius * cos(theta);
    p.Position.y = this->position.y + radius * sin(theta);
    p.Position.z = this->position.z;
    p.Velocity = glm::normalize((p.Position - this->position)) * fire.minSpeed;
    p.startColor = fire.startColor;
    p.endColor = fire.endColor;
    p.Life = fire.minLifetime;
    p.initialLife = p.Life;
    return p;
}
