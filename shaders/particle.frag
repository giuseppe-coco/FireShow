#version 330 core
in vec4 vs_Color;
out vec4 FragColor;

void main()
{
    // Il colore del frammento è semplicemente il colore che arriva dal vertex shader.
    // Per avere particelle rotonde e non quadrate, usiamo discard.
    // gl_PointCoord è una variabile speciale disponibile quando si renderizzano GL_POINTS.
    // Le sue coordinate (x,y) vanno da 0.0 a 1.0 all'interno del punto.
    // La funzione distance calcola la distanza dal centro del punto (0.5, 0.5).
    // Se la distanza è maggiore di 0.5 (cioè siamo fuori dal cerchio inscritto), scartiamo il frammento.
    if(distance(gl_PointCoord, vec2(0.5, 0.5)) > 0.5)
        discard;

    FragColor = vs_Color;
}