#pragma once

#include <vector>
#include <glm/glm.hpp>

// Struttura che definisce un singolo evento di lancio sulla timeline
struct FireworkEvent
{
    float triggerTime; // Il momento in cui l'evento deve scattare

    // Parametri per il lancio del proiettile
    glm::vec3 startPosition;
    glm::vec3 startVelocity;
    float fuseTime;

    // Aggiungiamo un ID unico per poter gestire la selezione e la cancellazione nell'UI
    int id;
};

class Timeline
{
public:
    Timeline();

    // Aggiorna lo stato della timeline (es. avanza il tempo se in play)
    // Ritorna un vettore di eventi che devono essere eseguiti IN QUESTO FRAME.
    // Questo design disaccoppia la timeline dal sistema di fuochi d'artificio.
    // La timeline non sa nulla di "FireworksShell", sa solo di "FireworkEvent".
    std::vector<const FireworkEvent *> Update(float dt);

    // Disegna l'interfaccia della timeline usando ImGui
    void DrawUI();

    void Play();
    void Pause();
    void Reset();

private:
    bool isPlaying;
    float currentTime;
    float maxTime; // Durata totale della timeline (es. 180 secondi)

    // L'elenco di tutti gli eventi programmati
    std::vector<FireworkEvent> events;
    int nextEventId; // Per generare ID unici
};