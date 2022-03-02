Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse, float4 ambiant)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity + ambiant);
    return colour;
}

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    float4 lightColour1;
    float4 lightColour2;

    float4 diffuseColour = { 0.5, 0.5, 0.5, 0 };
    float4 ambiantColour = { 0.2, 0.2, 0.2, 0 };
    float3 lightDirection1 = { -0.1, -0.2, -0.6 };
    float3 lightDirection2 = { 0.1, 0.2, 0.6 };

    textureColour = texture0.Sample(sampler0, input.tex);
    lightColour1 = calculateLighting(-lightDirection1, input.normal, diffuseColour, ambiantColour);
    lightColour2 = calculateLighting(-lightDirection2, input.normal, diffuseColour, ambiantColour);
    return (lightColour1 + lightColour2) * textureColour;
}



