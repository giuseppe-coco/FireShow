#include "Timeline.h"
#include "Utils.h" 
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include "../vendors/imgui/imgui.h"

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
                triggeredEvents.push_back(&event);
        }

        if (currentTime > maxTime)
        {
            currentTime = maxTime;
            Pause();
        }
    }
    return triggeredEvents;
}

void Timeline::DrawUI(
    const int timelineWidth,
    const int windowHeight,
    const int timelineHeight,
    std::vector<Firework> &lib)
{
    // Imposta la posizione e la dimensione della prossima finestra di ImGui
    ImGui::SetNextWindowPos(ImVec2(0, windowHeight - timelineHeight)); // Posizione (0,0) in alto a sinistra dello schermo
    ImGui::SetNextWindowSize(ImVec2(timelineWidth, timelineHeight));

    // Aggiungi flag per renderla "fissa": non ridimensionabile, non spostabile
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

    // Inizia una nuova finestra di ImGui. Tutto quello che disegneremo fino a ImGui::End()
    // apparirà in questa finestra.
    ImGui::Begin("Timeline Controls", nullptr, window_flags);

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
        // Se l'utente muove lo slider, mettiamo in pausa per evitare salti strani.
        Pause();
    ImGui::Separator();

    mayAddEvent(lib);

    ImGui::Text("%d events on timeline", (int)events.size());

    // Elenca tutti gli eventi
    for (int i = 0; i < events.size(); ++i)
    {
        // ImGui::PushID/PopID è importante quando hai widget con la stessa etichetta in un loop.
        // Dà a ogni widget un ID unico.
        ImGui::PushID(events[i].id);
        ImGui::Text("%s at %.2f s", events[i].fire.name.c_str(), events[i].triggerTime);
        ImGui::SameLine();
        mayDelEvent(i); 
        ImGui::PopID();
    }
    ImGui::End(); // Conclude la finestra
}

void Timeline::mayDelEvent(int i)
{
    if (ImGui::Button("Delete"))
    {
        events.erase(events.begin() + i);
        i--; // Decrementa l'indice perché il vettore si è accorciato
    }
}

void Timeline::mayAddEvent(std::vector<Firework>& lib)
{
    // Dropdown per selezionare il tipo di fuoco da aggiungere
    static Firework* selectedFirework; // static per mantenere la selezione tra i frame
    
    // Se selectedFirework non è ancora stato impostato (è la prima volta o
    // l'elemento che puntava è stato cancellato), lo impostiamo al primo
    // elemento della libreria come default sicuro.
    if (selectedFirework == nullptr)
        selectedFirework = &lib[0];

    auto it = std::find_if(
        lib.begin(), lib.end(),
        [&](const Firework &f)
        { return selectedFirework && f.id == selectedFirework->id; }
    );

    if (ImGui::BeginCombo(
            "Firework Type",
            it != lib.end() ? (*it).name.c_str() : "",
            ImGuiComboFlags_WidthFitPreview))
    {
        for (auto& elem : lib)
        {
            if (ImGui::Selectable(elem.name.c_str(), selectedFirework->id == elem.id))
                selectedFirework = &elem;
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
 
    if (ImGui::Button("Add Firework at Current Time"))
    {
        FireworkEvent newEvent;
        newEvent.triggerTime = currentTime;
        newEvent.fire = *selectedFirework;
        newEvent.id = nextEventId++;
        events.push_back(newEvent);
    }
}