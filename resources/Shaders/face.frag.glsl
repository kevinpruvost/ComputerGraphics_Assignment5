/*
 * GLSL Fragment Shader code for OpenGL version 3.3
 */

#version 330 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct PointLight // 64 bytes
{
	vec3 position;
    float constant;
    
    vec3 ambient;
    float linear;

    vec3 diffuse;
    float quadratic; 

    vec3 specular;
    float pad0; // Padding
};

struct DirectionLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 128

layout (std140) uniform Lights
{
	PointLight pointLights[NR_POINT_LIGHTS];
//    DirectionLight directionLight;
//    SpotLight spotLight;
    int pointLightsCount;
};

layout (std140) uniform CameraProps
{
    vec4 viewPos;
    mat4 viewProj;
    mat4 view;
	mat4 projection;
};

vec3 CalcDirLight(DirectionLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

out vec4 color;

uniform sampler2D _texture;
uniform Material material;

void main()
{
    // Props
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos.xyz - FragPos);
    
    vec3 result = vec3(0.0);
    for(int i = 0; i < pointLightsCount; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    color = vec4(result, 1.0);
//	color = texture(_texture, TexCoords);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirectionLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    // vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    vec3 ambient = light.ambient * vec3(texture(_texture, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(_texture, TexCoords));
    vec3 specular = vec3(0.0);
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    //vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    //vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    vec3 ambient = light.ambient * vec3(texture(_texture, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(_texture, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(_texture, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}