#include "Timeline.h"
#include <GLFW/glfw3.h>
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

void Timeline::DrawUI(int windowWidth, int windowHeight, const int timelineHeight, const std::map<int, FireworkType> lib)
{
    // Imposta la posizione e la dimensione della prossima finestra di ImGui
    ImGui::SetNextWindowPos(ImVec2(0, windowHeight - timelineHeight)); // Posizione (0,0) in alto a sinistra dello schermo
    ImGui::SetNextWindowSize(ImVec2(windowWidth, timelineHeight));

    // Aggiungi flag per renderla "fissa": senza titolo, non ridimensionabile, non spostabile
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

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

    // --- Logica per aggiungere un nuovo evento ---
    // Dropdown per selezionare il tipo di fuoco da aggiungere
    static int selectedFireworkId = 0; // static per mantenere la selezione tra i frame
    if (ImGui::BeginCombo("Firework Type", lib.count(selectedFireworkId) ? lib.at(selectedFireworkId).name.c_str() : ""))
    {
        for (const auto &pair : lib)
        {
            if (ImGui::Selectable(pair.second.name.c_str(), selectedFireworkId == pair.second.id))
                selectedFireworkId = pair.second.id;
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();

    // --- Gestione Eventi ---
    if (ImGui::Button("Add Firework at Current Time"))
    {
        FireworkEvent newEvent;
        newEvent.triggerTime = currentTime;
        newEvent.fireworkTypeId = selectedFireworkId;
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