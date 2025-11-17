// Shell.h
#pragma once

#include "ParticleSystem.h"
#include "FireworkTypes.h"
#include "AudioManager.h"

#include <memory>
#include <glm/glm.hpp>

// Enum per rappresentare lo stato corrente del proiettile
enum class ShellState
{
    RISING,   // Sta salendo
    INACTIVE,  // È spento e pronto per essere riutilizzato
    EXPLODING
};

class Shell
{
public:
    // Il costruttore prende una reference a un sistema di particelle che userà
    // per generare i suoi effetti. Questo è un esempio di Dependency Injection.
    // Invece di creare un ParticleSystem al suo interno, ne usa uno fornito dall'esterno.
    // Questo è ottimo perché permette a più proiettili di condividere lo stesso sistema di particelle,
    // risparmiando memoria e draw calls.
    Shell(Firework fire, ParticleSystem &particleSystem, AudioManager &audioManager);

    ShellState GetState() const { return state; }
    static std::unique_ptr<Shell> createShell(ParticleSystem &ps, Firework fire, AudioManager &audio);
    virtual void Launch();
    virtual void Update(float dt);

    // Virtual destructor. FONDAMENTALE quando si usa l'ereditarietà con polimorfismo.
    // Assicura che se si cancella un puntatore alla classe base che punta a un oggetto
    // della classe derivata, venga chiamato il distruttore corretto.
    virtual ~Shell() = default;

protected:
    Firework fire;
    ParticleSystem &particleSystem; // Riferimento al sistema di particelle condiviso
    AudioManager &audioManager;
    glm::vec3 position;
    glm::vec3 velocity;
    float fuse;       // Timer per l'esplosione
    float trailTimer; // Timer per emettere le particelle della scia
    ShellState state;

    void emitTrailParticle();
    
    // Metodo per l'esplosione.
    // È 'virtual puro' ( = 0; ). Questo significa che la classe Shell non fornisce
    // un'implementazione. Obbliga ogni classe figlia (come PeonyShell) a
    // fornire la propria implementazione specifica.
    // Questo rende Shell una "classe astratta": non puoi crearne un'istanza diretta.
    virtual void explode() = 0;
};