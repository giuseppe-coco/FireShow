// Editor.h
#pragma once

#include <vector>

#include "FireworkTypes.h"

class Editor
{
private:
    int nextFireworkTypeId;
    std::vector<Firework> fireworksLibrary;
    Firework *selectedType = nullptr; // tipo di fuoco attualmente selezionato nell'editor
    void createHardcodedFireworks();

public: 
    Editor();
    std::vector<Firework>& getFireworksLibrary() { return fireworksLibrary; }
    void DrawUI(int windowWidth, int windowHeight, const int editorHeight);
};