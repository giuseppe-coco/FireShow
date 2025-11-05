#version 330 core
layout (location = 0) in vec3 aPos;

// Uniforms
uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;

void main()
{
    TexCoords = aPos;
    
    // TRUCCO 1: Rimuoviamo la traslazione dalla matrice di vista.
    // La matrice di vista è composta da rotazione + traslazione. Togliendo la traslazione,
    // il cubo dello skybox seguirà la rotazione della camera ma non si muoverà con essa.
    // Questo fa sì che la camera sembri sempre al centro dello skybox, dandogli
    // una dimensione infinita.
    mat4 viewNoTranslation = mat4(mat3(view)); 
    
    vec4 pos = projection * viewNoTranslation * vec4(aPos, 1.0);
    
    // TRUCCO 2: Impostiamo la profondità (z) al valore massimo (1.0).
    // gl_Position è un vec4(x, y, z, w). La divisione z/w dà la profondità, che va da -1 a 1.
    // Impostando z a w, la profondità normalizzata (z/w) sarà 1.0. Questo forza lo skybox
    // ad essere sempre disegnato dietro a qualsiasi altro oggetto nella scena.
    gl_Position = pos.xyww;
}