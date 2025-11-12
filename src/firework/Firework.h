// Firework.h
#pragma once

#include "ParticleSystem.h"
#include "Types.h"
#include "Editor.h"
#include "Timeline.h"

#include <memory>
#include <glm/glm.hpp>

// Enum per rappresentare lo stato corrente del proiettile
enum class ShellState
{
    RISING,   // Sta salendo
    INACTIVE  // È spento e pronto per essere riutilizzato
};

class Firework
{
    friend class Editor;
    friend class Timeline;

protected:
    // ******************** Attributes *****************************
    // --- General properties ---
    int id;
    std::string name;
    FireworkFamily family;
    ParticleSystem &particleSystem; // Riferimento al sistema di particelle condiviso

    // --- Particles' properties ---
    unsigned int particleCount = 100;
    float minLifetime = 0.8;
    float maxLifetime = 1.3;
    float minSpeed = 15.0;
    float maxSpeed = 25.0;
    glm::vec3 startColor = glm::vec3(1.0f, 0.5f, 0.2f); // Arancione
    glm::vec3 endColor = glm::vec3(0.5f, 0.0f, 0.0f);   // Rosso scuro
    float gravityModifier = 1.0;

    // --- Firework's properties ---
    glm::vec3 position;
    glm::vec3 velocity;
    float fuse;       // Timer per l'esplosione
    float trailTimer; // Timer per emettere le particelle della scia
    ShellState state;

    // ********************** Inheritable Methods ******************************
    void emitTrailParticle();
    
    // Metodo per l'esplosione.
    // È 'virtual puro' ( = 0; ). Questo significa che la classe Firework non fornisce
    // un'implementazione. Obbliga ogni classe figlia (come PeonyShell) a
    // fornire la propria implementazione specifica.
    // Questo rende Firework una "classe astratta": non puoi crearne un'istanza diretta.
    virtual void explode() = 0;

public:
    // Il costruttore prende una reference a un sistema di particelle che userà
    // per generare i suoi effetti. Questo è un esempio di Dependency Injection.
    // Invece di creare un ParticleSystem al suo interno, ne usa uno fornito dall'esterno.
    // Questo è ottimo perché permette a più proiettili di condividere lo stesso sistema di particelle,
    // risparmiando memoria e draw calls.
    Firework(ParticleSystem &particleSystem);

    // Virtual destructor. FONDAMENTALE quando si usa l'ereditarietà con polimorfismo.
    // Assicura che se si cancella un puntatore alla classe base che punta a un oggetto
    // della classe derivata, venga chiamato il distruttore corretto.
    virtual ~Firework() = default;
    void Launch(const FireworkEvent &event);
    void Update(float dt);
    ShellState GetState() const { return state; }
};