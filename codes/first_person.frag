#version 330 core

in vec3 fragPos;
in vec3 normal;

out vec4 fragColor;

/////////////////////////////////

struct Terrain
{
    vec3 col;
};

struct DirLight
{
    vec3 dir;
    vec3 col;
};

struct Camera
{
    vec3 pos;
};

/////////////////////////////////

uniform DirLight dLight;
uniform Terrain terrain;
uniform Camera cam;

/////////////////////////////////

void main()
{
    //ambient
    float amb = 0.3;
    vec3 ambient = amb*dLight.col;

    //diffuse
    vec3 normVec = normalize(normal);
    vec3 lightDirection = normalize(dLight.dir);
    float diff = max(dot(normVec, lightDirection), 0.0f);
    vec3 diffuse = diff*dLight.col;

    //specular
    float specIntens = 0.5f;
    vec3 viewDir = normalize(cam.pos - fragPos);
    vec3 reflectDir = reflect(-lightDirection, normVec);
    float spec = pow(max(dot(viewDir,reflectDir), 0.0f), 128);
    vec3 specular = spec*specIntens*dLight.col;

    vec3 finalColor = (ambient + diffuse + specular)*terrain.col;
    fragColor = vec4(finalColor,1.0f);
}