// Editor.cpp

#include "Editor.h"
#include <GLFW/glfw3.h>
#include "../vendors/imgui/imgui.h"

Editor::Editor() 
{
    // Creiamo un tipo di default
    FireworkType defaultType;
    defaultType.id = nextFireworkTypeId++;
    defaultType.name = "Default Peony";
    fireworksLibrary[defaultType.id] = defaultType;
}

void Editor::DrawUI(
    const int editorWidth,
    const int windowHeight,
    const int editorHeight
)
{
    // Imposta la posizione e la dimensione della prossima finestra di ImGui
    ImGui::SetNextWindowPos(ImVec2(editorWidth, windowHeight - editorHeight)); // Posizione (0,0) in alto a sinistra dello schermo
    ImGui::SetNextWindowSize(ImVec2(editorWidth, editorHeight));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

    ImGui::Begin("Firework Editor", nullptr, window_flags);
    // --- Lista dei tipi di fuochi (la nostra fireworksLibraryreria) ---
    if (ImGui::Button("Add New Type"))
    {
        FireworkType newType;
        newType.id = nextFireworkTypeId++;
        newType.name = "New Firework " + std::to_string(newType.id);
        fireworksLibrary[newType.id] = newType;
        selectedType = &fireworksLibrary[newType.id]; // Seleziona il nuovo tipo
    }
    ImGui::Separator();
    ImGui::Text("Library:");
    for (auto &pair : fireworksLibrary)
    {
        // Seleziona un tipo dalla lista
        if (ImGui::Selectable(pair.second.name.c_str(), selectedType && selectedType->id == pair.second.id))
            selectedType = &pair.second;
    }
    ImGui::Separator();
    // --- Editor delle proprietà del tipo selezionato ---
    if (selectedType)
    {
        ImGui::Text("Editing: %s", selectedType->name.c_str());
        char nameBuffer[128];
        strncpy(nameBuffer, selectedType->name.c_str(), sizeof(nameBuffer));
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
            selectedType->name = nameBuffer;

        ImGui::DragInt("Particle Count", &selectedType->particleCount, 10, 10, 10000);
        ImGui::DragFloatRange2("Lifetime", &selectedType->minLifetime, &selectedType->maxLifetime, 0.1f, 0.1f, 10.0f);
        ImGui::DragFloatRange2("Speed", &selectedType->minSpeed, &selectedType->maxSpeed, 0.5f, 0.0f, 100.0f);
        ImGui::ColorEdit3("Start Color", (float *)&selectedType->startColor);
        ImGui::ColorEdit3("End Color", (float *)&selectedType->endColor);
        // ImGui::SliderFloat("Gravity Modifier", &selectedType->gravityModifier, 0.0f, 2.0f);
    }
    ImGui::End();
}