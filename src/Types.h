// FireworkTypes.h
#pragma once

#include <glm/glm.hpp>
#include <string>

class Firework;

enum class FireworkFamily
{
    Peony,
    Chrysanthemum,
    Willow,
    Volcano
};

struct FireworkEvent
{
    float triggerTime;  // Il momento in cui l'evento deve scattare
    Firework *fire;      // Oggetto da lanciare
    int id;             // Aggiungiamo un ID unico per poter gestire la selezione e la cancellazione nell'UI
};