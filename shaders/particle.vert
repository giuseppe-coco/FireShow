#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

// Uniforms per le trasformazioni spaziali
uniform mat4 projection;
uniform mat4 view;
uniform int isWillow;

// Passiamo il colore al fragment shader
out vec4 vs_Color;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    vs_Color = aColor;
    
    // gl_PointSize è una variabile speciale del vertex shader.
    // Controlla la dimensione in pixel dei punti renderizzati con GL_POINTS.
    // La facciamo dipendere dalla distanza per dare un minimo di prospettiva
    // e la riduciamo per le particelle che stanno per "morire".
    float distance = length(gl_Position.xyz);

    if (isWillow == 1)
    {
        gl_PointSize = distance;
        //gl_PointSize *= vs_Color.a;
    }
    else
    {
        gl_PointSize = 50.0 / distance;
        gl_PointSize *= (vs_Color.a * 3.0); // Riduciamo la dimensione man mano che il colore sfuma
    }
}