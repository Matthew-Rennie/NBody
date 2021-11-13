
struct InputType
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};


float4 main(InputType input) : SV_TARGET
{
    // return input.color;
    return float4(input.color.r, input.color.g, input.color.b, 1);
}



