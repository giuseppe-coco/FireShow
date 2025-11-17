// FireworkTypes.h
#pragma once

#include <glm/glm.hpp>
#include <string>

enum class FireworkFamily
{
    Peony,
    Chrysanthemum,
    Willow,
    Volcano,
    Ring
};

// Definisce le proprietà di un'esplosione di fuochi d'artificio.
struct Firework
{
    // --- General properties ---
    int id;
    std::string name;
    FireworkFamily family;
    
    // --- Particles' properties ---
    unsigned int particleCount = 100;
    float minLifetime = 0.8;
    float maxLifetime = 1.3;
    float minSpeed = 15.0;
    float maxSpeed = 25.0;
    glm::vec3 startColor = glm::vec3(1.0f, 0.5f, 0.2f); // Arancione
    glm::vec3 endColor = glm::vec3(0.5f, 0.0f, 0.0f);   // Rosso scuro
    float gravityModifier = 1.0;

    // --- Shell's properties ---
    glm::vec3 startShellPosition;
    glm::vec3 startShellVelocity;
    float fuseTime;

    // --- Sound ---
    std::string launchSound = "default_launch";
    std::string explosionSound = "default_explosion";
};

struct FireworkEvent
{
    float triggerTime;  // Il momento in cui l'evento deve scattare
    int fireworkId;
    Firework fire;
    int id;             // Aggiungiamo un ID unico per poter gestire la selezione e la cancellazione nell'UI
};