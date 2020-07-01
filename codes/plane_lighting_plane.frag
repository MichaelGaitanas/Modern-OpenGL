#version 330 core

in vec3 normal;
in vec3 fragPos;

out vec4 fragColor;

struct Plane
{
    vec3 col;
};

struct Camera
{
    vec3 pos;
};

struct PointLight
{
    vec3 col;
    vec3 pos;
};

uniform Plane plane;
uniform Camera cam;
uniform PointLight lamp;

void main()
{
    //ambient
    float amb = 0.3f;
    vec3 ambient = amb*lamp.col;

    //diffuse
    vec3 normVec = normalize(normal);
    if (!gl_FrontFacing) // <=========== here
        normVec = -normVec; //lighten only one side of the plane
    vec3 lightDir = normalize(lamp.pos - fragPos);
    float diff = max(dot(normVec,lightDir), 0.0f);
    vec3 diffuse = diff*lamp.col;

    //specular
    float specIntens = 0.5f;
    vec3 viewDir = normalize(cam.pos - fragPos);
    vec3 reflectDir = reflect(-lightDir,normVec);
    float spec = pow(max(dot(viewDir,reflectDir), 0.0f), 128);
    vec3 specular = spec*specIntens*lamp.col;

    vec3 finalColor = (ambient + diffuse + specular)*plane.col;
    fragColor = vec4(finalColor,1.0f);
}