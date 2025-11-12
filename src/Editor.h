// Editor.h
#pragma once

#include <vector>
#include <memory>

#include "firework/Firework.h"

class Editor
{
private:
    int nextFireworkTypeId;
    std::vector<std::unique_ptr<Firework>> fireworksLibrary;
    Firework *selectedType = nullptr; // tipo di fuoco attualmente selezionato nell'editor
    void createHardcodedFireworks(ParticleSystem &ps);

public:
    Editor(ParticleSystem &ps);
    std::vector<std::unique_ptr<Firework>> &getFireworksLibrary() { return fireworksLibrary; }
    void DrawUI(int windowWidth, int windowHeight, const int editorHeight);
};