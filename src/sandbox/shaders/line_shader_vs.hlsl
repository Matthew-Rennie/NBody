// Standard issue vertex shader, apply matrices, pass info to pixel shader
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct InputType
{
	float4 position : POSITION;
    float3 color : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

OutputType main(InputType input)
{
	OutputType output = (OutputType)0;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.color = input.color;

    return output;
}