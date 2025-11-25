#version 330 core

out vec4 FragColor;

uniform vec4 shadowColor;

void main()
{
    // Sombra simples com cor semi-transparente
    FragColor = shadowColor;
}
