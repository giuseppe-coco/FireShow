#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

// Per lo skybox, usiamo una "cubemap", una texture speciale composta da 6 facce.
// Per la nostra versione 2D, fingeremo usando una texture 2D, ma il concetto è simile.
// Per semplicità, inizieremo con una texture 2D per uno "skydome".
uniform sampler2D skyTexture;

void main()
{
    // Per ora, un campionamento semplice. Un vero skybox userebbe TexCoords come
    // una direzione 3D per campionare una cubemap.
    // Con una texture 2D, dobbiamo "mappare" le coordinate 3D in 2D.
    // Una semplice mappatura sferica può essere:
    vec2 uv = vec2(atan(TexCoords.x, TexCoords.z) / (2.0 * 3.14159) + 0.5, 
                   asin(TexCoords.y) / 3.14159 + 0.5);
    FragColor = texture(skyTexture, uv);
}