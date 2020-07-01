#version 330 core

in vec3 normal;
in vec3 fragPos;

out vec4 fragColor;

struct PointLight
{
    vec3 col;
    vec3 pos;
    vec3 atten;
};
uniform PointLight lamp;

struct Camera
{
    vec3 pos;
};
uniform Camera cam;

struct Monkey
{
    vec3 col;
};
uniform Monkey monkey;

void main()
{
    //ambient
    float amb = 0.1f;
    vec3 ambient = amb*lamp.col;

    //diffuse
    vec3 normVec = normalize(normal);
    vec3 lightDir = normalize(lamp.pos - fragPos);
    float diff = max(dot(normVec,lightDir), 0.0f);
    vec3 diffuse = diff*lamp.col;

    //specular
    float specIntens = 0.5f;
    vec3 viewDir = normalize(cam.pos - fragPos);
    vec3 reflectDir = reflect(-lightDir,normVec);
    float spec = pow(max(dot(viewDir,reflectDir), 0.0f), 128);
    vec3 specular = spec*specIntens*lamp.col;

    //attenuation
    float dist = length(lamp.pos - fragPos);
    float attenFactor = 1.0f/(lamp.atten.x +
                              lamp.atten.y*dist +
                              lamp.atten.z*dist*dist);

    vec3 finalColor = (ambient + diffuse + specular)*monkey.col*attenFactor;
    fragColor = vec4(finalColor,1.0f);
}