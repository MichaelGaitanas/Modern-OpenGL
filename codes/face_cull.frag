#version 330 core

in vec3 fragPos;
in vec3 normal;

struct Monkey
{
    vec3 col;
};

struct DirLight
{
    vec3 col;
    vec3 dir;
};

struct Camera
{
    vec3 pos;
};

uniform Monkey monkey;
uniform Camera cam;
uniform DirLight dLight;

out vec4 fragColor;

void main()
{
    //ambient
    float amb = 0.3;
    vec3 ambient = amb*dLight.col;

    //diffuse
    vec3 normVec = normalize(normal);
    vec3 lightDir = normalize(dLight.dir);
    float diff = max(dot(normVec, lightDir), 0.0f);
    vec3 diffuse = diff*dLight.col;

    //specular
    float specIntens = 0.5f;
    vec3 viewDir = normalize(cam.pos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normVec);
    float spec = pow(max(dot(viewDir,reflectDir), 0.0f), 128);
    vec3 specular = spec*specIntens*dLight.col;

    vec3 finalColor = (ambient + diffuse + specular)*monkey.col;
    fragColor = vec4(finalColor,1.0f);
}