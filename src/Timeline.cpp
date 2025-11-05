#include "Timeline.h"
#include "../vendors/imgui/imgui.h" // Includiamo l'header di ImGui

Timeline::Timeline()
    : isPlaying(false), currentTime(0.0f), maxTime(180.0f), nextEventId(0) {}

void Timeline::Play() { isPlaying = true; }
void Timeline::Pause() { isPlaying = false; }
void Timeline::Reset()
{
    isPlaying = false;
    currentTime = 0.0f;
}

std::vector<const FireworkEvent *> Timeline::Update(float dt)
{
    std::vector<const FireworkEvent *> triggeredEvents;

    if (isPlaying)
    {
        float previousTime = currentTime;
        currentTime += dt;

        // Controlla se abbiamo superato il trigger time di qualche evento
        for (const auto &event : events)
        {
            // Un evento scatta se il suo tempo di trigger era nel passato
            // ma ora è nel presente.
            if (event.triggerTime > previousTime && event.triggerTime <= currentTime)
            {
                triggeredEvents.push_back(&event);
            }
        }

        if (currentTime > maxTime)
        {
            currentTime = maxTime;
            Pause();
        }
    }
    return triggeredEvents;
}

void Timeline::DrawUI()
{
    // Inizia una nuova finestra di ImGui. Tutto quello che disegneremo fino a ImGui::End()
    // apparirà in questa finestra.
    ImGui::Begin("Timeline Controls");

    // --- Controlli di riproduzione ---
    if (ImGui::Button("Play"))
        Play();
    ImGui::SameLine(); // Mette il prossimo widget sulla stessa riga
    if (ImGui::Button("Pause"))
        Pause();
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
        Reset();

    ImGui::Text("Current Time: %.2f s", currentTime);

    // --- Slider della timeline ---
    // L'etichetta "##..." è un trucco di ImGui per avere un ID unico senza mostrare un'etichetta visibile.
    if (ImGui::SliderFloat("##timeline_slider", &currentTime, 0.0f, maxTime, ""))
    {
        // Se l'utente muove lo slider, mettiamo in pausa per evitare salti strani.
        Pause();
    }

    ImGui::Separator(); // Una linea di separazione

    // --- Gestione Eventi ---
    if (ImGui::Button("Add Firework at Current Time"))
    {
        FireworkEvent newEvent;
        newEvent.triggerTime = currentTime;
        // Valori di default per il nuovo fuoco d'artificio
        newEvent.startPosition = glm::vec3((rand() % 40) - 20, 0.0f, (rand() % 20) - 10);
        newEvent.startVelocity = glm::vec3(0.0f, 10.0f + (rand() % 10), 0.0f);
        newEvent.fuseTime = 2.0f + (rand() % 100) / 100.0f;
        newEvent.id = nextEventId++;

        events.push_back(newEvent);
    }

    ImGui::Text("%d events on timeline", (int)events.size());

    // Elenca tutti gli eventi
    for (int i = 0; i < events.size(); ++i)
    {
        // ImGui::PushID/PopID è importante quando hai widget con la stessa etichetta in un loop.
        // Dà a ogni widget un ID unico.
        ImGui::PushID(events[i].id);
        ImGui::Text("Event at %.2f s", events[i].triggerTime);
        ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            events.erase(events.begin() + i);
            i--; // Decrementa l'indice perché il vettore si è accorciato
        }
        ImGui::PopID();
    }

    ImGui::End(); // Conclude la finestra
}