#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "Types.h"

class Firework;

class Timeline
{
public:
    Timeline();

    // Aggiorna lo stato della timeline (es. avanza il tempo se in play)
    // Ritorna un vettore di eventi che devono essere eseguiti IN QUESTO FRAME.
    // Questo design disaccoppia la timeline dal sistema di fuochi d'artificio.
    // La timeline non sa nulla di "Firework", sa solo di "FireworkEvent".
    std::vector<const FireworkEvent *> Update(float dt);

    void DrawUI(int windowWidth, int windowHeight, const int timelineHeight, std::vector<std::unique_ptr<Firework>>& lib);

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

    void mayAddEvent(std::vector<std::unique_ptr<Firework>>& lib);
    void mayDelEvent(int idx);
};