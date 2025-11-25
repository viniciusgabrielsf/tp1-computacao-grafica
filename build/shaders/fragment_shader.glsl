#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

// Fog (névoa)
uniform bool enableFog;
uniform float fogDensity;
uniform vec3 fogColor;

void main()
{
    // Modelo de iluminação Phong (componentes: ambiente, difusa, especular)

    // 1. Luz ambiente
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // 2. Luz difusa
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 3. Luz especular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Cor final com iluminação
    vec3 result = (ambient + diffuse + specular) * objectColor;

    // Aplica fog (névoa) se habilitado
    if (enableFog) {
        float distance = length(viewPos - FragPos);
        float fogFactor = exp(-fogDensity * distance);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        result = mix(fogColor, result, fogFactor);
    }

    FragColor = vec4(result, 1.0);
}
