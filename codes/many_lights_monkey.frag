#version 330 core

#define POINT_LIGHTS 7

in vec3 normal;
in vec3 fragPos;

out vec4 fragColor;

struct Monkey
{
    vec3 col;
};

struct DirLight
{
    vec3 dir;
    vec3 col;
};

struct PointLight
{
    vec3 col;
    vec3 pos;
    vec3 atten;
};

struct Camera
{
    vec3 pos;
};

//////////////////////////////////////////////

uniform DirLight dLight;
uniform PointLight pLight[POINT_LIGHTS];
uniform Monkey monkey;
uniform Camera camera;

//////////////////////////////////////////////

vec3 CalcDirLight(DirLight dLight, vec3 normVec, vec3 viewDir)
{
    //ambient
    float amb = 0.15f;
    vec3 ambient = amb*dLight.col;

    //diffuse
    vec3 lightDir = normalize(dLight.dir);
    float diff = max(dot(normVec, lightDir), 0.0f);
    vec3 diffuse = diff*dLight.col;

    //specular
    float specIntens = 0.5f;
    vec3 reflectDir = reflect(-lightDir, normVec);
    float spec = pow(max(dot(viewDir,reflectDir), 0.0f), 128);
    vec3 specular = spec*specIntens*dLight.col;

    return (ambient + diffuse + specular)*monkey.col;
}

vec3 CalcPointLight(PointLight pLight, vec3 normVec, vec3 viewDir)
{
    //ambient
    float amb = 0.15f;
    vec3 ambient = amb*pLight.col;

    //diffuse
    vec3 lightDir = normalize(pLight.pos - fragPos);
    float diff = max(dot(normVec,lightDir), 0.0f);
    vec3 diffuse = diff*pLight.col;

    //specular
    float specIntens = 0.5f;
    vec3 reflectDir = reflect(-lightDir, normVec);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 128);
    vec3 specular = spec*specIntens*pLight.col;

    //attenuation
    float dist = length(pLight.pos - fragPos);
    float attenFactor = 1.0f/(pLight.atten.x +
                              pLight.atten.y*dist +
                              pLight.atten.z*dist*dist);

    return (ambient + diffuse + specular)*attenFactor*monkey.col;
}

void main()
{
    vec3 normVec = normalize(normal);
    vec3 viewDir = normalize(camera.pos - fragPos);

    vec3 finalColor = vec3(0.0f);
    finalColor += CalcDirLight(dLight, normVec, viewDir);
    for (int i = 0; i < POINT_LIGHTS; ++i)
    {
        finalColor += CalcPointLight(pLight[i], normVec, viewDir);
    }
    fragColor = vec4(finalColor,1.0f);
}
