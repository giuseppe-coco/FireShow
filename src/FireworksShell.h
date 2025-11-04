#pragma once

#include "ParticleSystem.h"
#include <glm/glm.hpp>

// Enum per rappresentare lo stato corrente del proiettile
enum class ShellState
{
    RISING,   // Sta salendo
    EXPLODED, // È esploso
    INACTIVE  // È spento e pronto per essere riutilizzato
};

class FireworksShell
{
public:
    // Il costruttore ora prende una reference a un sistema di particelle che userà
    // per generare i suoi effetti. Questo è un esempio di Dependency Injection.
    // Invece di creare un ParticleSystem al suo interno, ne usa uno fornito dall'esterno.
    // Questo è ottimo perché permette a più proiettili di condividere lo stesso sistema di particelle,
    // risparmiando memoria e draw calls.
    FireworksShell(ParticleSystem &particleSystem);

    void Launch(glm::vec3 startPosition, glm::vec3 startVelocity, float fuseTime);
    void Update(float dt);

    ShellState GetState() const { return state; }

private:
    ParticleSystem &particleSystem; // Riferimento al sistema di particelle condiviso

    glm::vec3 position;
    glm::vec3 velocity;
    float fuse;       // Timer per l'esplosione
    float trailTimer; // Timer per emettere le particelle della scia

    ShellState state;

    void emitTrailParticle();
    void explode();
};