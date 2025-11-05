#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

// La texture del terreno
uniform sampler2D groundTexture;

void main()
{
    // Campioniamo la texture usando le coordinate ricevute
    vec4 texColor = texture(groundTexture, TexCoords);

    // Applichiamo un semplice "fog" o oscuramento per dare un'atmosfera notturna.
    // In questo modo la texture, anche se chiara, apparirà scura nella scena.
    FragColor = texColor * vec4(0.2, 0.2, 0.2, 1.0);
}