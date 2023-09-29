#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;

uniform bool flip;
uniform bool use_normals;
uniform bool use_color;
uniform vec4 color;
uniform vec3 view_pos;

struct PointLight {
  vec3 pos;
  vec3 color;
  vec3 ambient;
  float intensity;
  float radius;
};

struct DirectionalLight {
  vec3 color;
  vec3 ambient;
  vec3 direction;
};

struct SpotLight {
  vec3 pos;
  vec3 color;
  vec3 ambient;
  vec3 direction;
  float cutoff;
  float outer_cutoff;
  float falloff_multiplier;
};

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

layout(std140, binding = 0) uniform LightUBO {
  PointLight point_lights[MAX_POINT_LIGHTS];
  int point_light_count;
};

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float nom = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;

  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);

  return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) *
                  pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 CalculatePointLight(PointLight light, vec3 N, vec3 V, vec3 fragpos,
                         vec3 albedo, vec3 F0, float roughness,
                         float metallic) {
  // calculate per-light radiance
  vec3 L = normalize(light.pos - view_pos - fragpos);
  vec3 H = normalize(V + L);
  float distance = length(light.pos - view_pos - fragpos);
  float attenuation = 1.0 / (distance * distance);
  vec3 radiance = light.color * attenuation;

  // Cook-Torrance BRDF
  float NDF = DistributionGGX(N, H, roughness);
  float G = GeometrySmith(N, V, L, roughness);
  vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

  vec3 numerator = NDF * G * F;
  float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
  vec3 specular = numerator / denominator;
  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= 1.0 - metallic;
  float NdotL = max(dot(N, L), 0.0);
  return (kD * albedo / PI + specular) * radiance * NdotL;
}

void main() {
  vec4 Albedo = texture(g_albedo, TexCoords);
  vec3 FragPos = texture(g_position, TexCoords).rgb;
  vec3 Normal = texture(g_normal, TexCoords).rgb;
  float Specular = texture(g_albedo, TexCoords).a;

  vec3 N = normalize(Normal);
  vec3 V = normalize(-FragPos);

  float roughness = 0.2;
  roughness = Albedo.r * 0.5;
  float metallic = 0.0;

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, Albedo.rgb, metallic);
  vec3 Lo = vec3(0.0);
  // ambient
  vec3 color;
  for (int i = 0; i < point_light_count; ++i) 
  {
    Lo += CalculatePointLight(point_lights[i], N, V, FragPos, Albedo.rgb, F0, roughness, metallic);
  }
  vec3 diffuse = Albedo.rgb;
  // vec3 ambient = (vec3(0.08, 0.05, 0.02) + (diffuse * vec3(0.6)));
  vec3 ambient = (diffuse * vec3(0.6));
  color = Lo + ambient;
  FragColor = vec4(color, 1);
}
