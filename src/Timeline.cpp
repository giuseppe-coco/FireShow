// Timeline.cpp

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

#include "Timeline.h"
#include "Utils.h"

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
    std::map<int,Firework> &lib)
{
    // Imposta la posizione e la dimensione della prossima finestra di ImGui
    ImGui::SetNextWindowPos(ImVec2(0, windowHeight - timelineHeight)); // Posizione (0,0) in alto a sinistra dello schermo
    ImGui::SetNextWindowSize(ImVec2(timelineWidth, timelineHeight));

    // Aggiungi flag per renderla "fissa": non ridimensionabile, non spostabile
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

    // Inizia una nuova finestra di ImGui. Tutto quello che disegneremo fino a ImGui::End()
    // apparirà in questa finestra.
    ImGui::Begin("Controlli Timeline", nullptr, window_flags);

    if (ImGui::Button("Play"))
        Play();
    ImGui::SameLine();
    if (ImGui::Button("Pausa"))
        Pause();
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
        Reset();
    ImGui::SameLine();
    if (ImGui::Button("Export to Cobra")){
        ImGui::OpenPopup("Export result message");
        if (exportToCobra(lib) == 0)
            exportResMess = "Export completed successfully";
        else
            exportResMess = "Export failed";
    }

    drawPopUp();

    ImGui::Text("Tempo Corrente: %.2f s", currentTime);

    // --- Slider della timeline ---
    // L'etichetta "##..." è un trucco di ImGui per avere un ID unico senza mostrare un'etichetta visibile.
    if (ImGui::SliderFloat("##timeline_slider", &currentTime, 0.0f, maxTime, ""))
        // Se l'utente muove lo slider, mettiamo in pausa per evitare salti strani.
        Pause();
    ImGui::Separator();

    mayAddEvent(lib);

    ImGui::Text("%d eventi sulla timeline", (int)events.size());
    // std::cout << "E\n";

    for (int i = 0; i < events.size(); ++i)
    {
        // ImGui::PushID/PopID è importante quando hai widget con la stessa etichetta in un loop.
        // Dà a ogni widget un ID unico.
        ImGui::PushID(events[i].id);

        std::string fireName = "Unknown";
        if (lib.find(events[i].fireworkId) != lib.end())
        {
            fireName = lib[events[i].fireworkId].name;
        }

        ImGui::Text("%s at %.2f s", fireName.c_str(), events[i].triggerTime);
        ImGui::SameLine();
        mayDelEvent(i);
        ImGui::PopID();
    }
    ImGui::End();
}

void Timeline::mayDelEvent(int i)
{
    if (ImGui::Button("Elimina"))
        events.erase(events.begin() + i);
}

void Timeline::mayAddEvent(std::map<int, Firework> &lib)
{
    static int selectedFireworkId = -1;
    if (lib.empty()) return;

    // Inizializzazione sicura: se non abbiamo un ID valido, prendiamo il primo della mappa
    if (lib.find(selectedFireworkId) == lib.end())
        selectedFireworkId = lib.begin()->first;

    // Dropdown
    if (ImGui::BeginCombo("Tipo Fuoco", lib[selectedFireworkId].name.c_str(), ImGuiComboFlags_WidthFitPreview))
    {
        for (auto &elem : lib)
        {
            bool isSelected = (selectedFireworkId == elem.first);
            if (ImGui::Selectable(elem.second.name.c_str(), isSelected))
                selectedFireworkId = elem.first;

            // Imposta il focus iniziale sull'elemento selezionato
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();

    if (ImGui::Button("Aggiungi Fuoco al Tempo Corrente"))
    {
        FireworkEvent newEvent;
        newEvent.triggerTime = currentTime;

        newEvent.fireworkId = selectedFireworkId;

        newEvent.id = nextEventId++;
        newEvent.channel = 1;
        newEvent.cue = newEvent.id;

        events.push_back(newEvent);

        std::cout << "Evento aggiunto: ID evento" << newEvent.id << " (" << lib[newEvent.fireworkId].name << ")" << std::endl;
    }
}

int Timeline::exportToCobra(std::map<int, Firework> &lib){
    std::ofstream file("show.csv");
    if (!file)
    {
        std::cerr << "Errore apertura file\n";
        return 1;
    }

    file << "#Trigger Channel,#Trigger Button,#Confirmation Button,#Return Channel\n";
    file << "1,1,,1\n";
    file << "#Event Time,#Channel,#Cue,#Description\n";

    std::sort(
        events.begin(),
        events.end(), 
        [](const FireworkEvent &e1, const FireworkEvent &e2){
            return e1.triggerTime < e2.triggerTime;
        }
    );
    for (const auto &e : events){
        file << e.triggerTime << "," << e.channel << "," << e.cue << "," << lib[e.fireworkId].name << "\n";
    }

    file.close();
    return 0;
}

void Timeline::drawPopUp(){
    if (ImGui::BeginPopup("Export result message"))
    {
        ImGui::Text(exportResMess.c_str());
        ImGui::EndPopup();
        ImGui::CloseCurrentPopup();
    }
}