#pragma once

#include <glm/glm.hpp>
#include <string>

// Definisce le proprietà di un'esplosione di fuochi d'artificio.
struct FireworkType
{
    int id;
    std::string name = "New Firework";

    // Parametri dell'esplosione
    int particleCount = 100;
    float minLifetime = 0.8f;
    float maxLifetime = 1.3f;
    float minSpeed = 15.0f;
    float maxSpeed = 25.0f;
    glm::vec3 startColor = glm::vec3(1.0f, 0.5f, 0.2f); // Arancione
    glm::vec3 endColor = glm::vec3(0.5f, 0.0f, 0.0f);   // Rosso scuro
    float gravityModifier = 0.5f;                       // Fattore di scala per la gravità
};

// Struttura che definisce un singolo evento di lancio sulla timeline
struct FireworkEvent
{
    float triggerTime; // Il momento in cui l'evento deve scattare

    // Parametri per il lancio del proiettile
    glm::vec3 startPosition;
    glm::vec3 startVelocity;
    float fuseTime;
    int fireworkTypeId; // Riferimento al tipo di fuoco da lanciare

    // Aggiungiamo un ID unico per poter gestire la selezione e la cancellazione nell'UI
    int id;
};