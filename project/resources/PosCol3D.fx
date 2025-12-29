// Matrix Global
row_major float4x4 gWorldViewProj : WorldViewProjection;
row_major float4x4 gWorldMatrix;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;
float3 gLightDirection = float3(0.577f, -0.577f, 0.577f);
float3 gCameraPosition : CAMERA;

float1 PI = 3.14159265358979323846f;
float1 LIGHT_INTENSITY = 7.f;
float1 SHININESS = 25.f;

float3x3 indentityMatrix ={{1,0,0},{0,1,0},{0,0,1}};

RasterizerState gRasterizerState
{
    CullMode = Back;
    FrontCounterClockwise = false;
};

BlendState gBlendState
{
    BlendEnable[0] = false;
    SrcBlend = one;
    DestBlend = zero;
    BlendOp = add;
    SrcBlendAlpha = one;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less;
    StencilEnable = true;
};

SamplerState gSamplerState
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

// Input/Output Structs
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 WorldPosition : WORLD;
};

// Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    //output.Position = float4(input.Position, 1.f);
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.WorldPosition = mul(float4(input.Position, 1.f), gWorldMatrix);
    output.TexCoord = input.TexCoord;
    output.Normal = normalize(mul(input.Normal, (float3x3)gWorldMatrix));
    output.Tangent = normalize(mul(input.Tangent, (float3x3)gWorldMatrix));
    return output;
}

float3 PhongShading(const VS_OUTPUT input)
{
    float3 binormal = normalize(cross(input.Normal, input.Tangent));
    float3x3 tangentSpaceAxis = {input.Tangent, binormal, input.Normal};
    
    // Normal Mapping
    float3 sampledNormal = gNormalMap.Sample(gSamplerState, input.TexCoord);
    sampledNormal = (2.f * sampledNormal) - float3(1.f, 1.f, 1.f);
    sampledNormal = normalize(mul(sampledNormal, tangentSpaceAxis));

    // Observed Area
    float1 observedArea = saturate(dot(sampledNormal, normalize(-gLightDirection)));

    // Diffuse
    float4 diffuseColor = gDiffuseMap.Sample(gSamplerState, input.TexCoord);
    float3 lambertShading = (LIGHT_INTENSITY * diffuseColor.rgb) / PI;

    // Specular
    float3 invViewDirection = normalize(gCameraPosition - input.WorldPosition.xyz);
    float1 specularColor = gSpecularMap.Sample(gSamplerState, input.TexCoord).r;
    float1 glossinessColor = gGlossinessMap.Sample(gSamplerState, input.TexCoord).r;
    float1 specularReflection = specularColor * pow(abs(dot(reflect(-gLightDirection, sampledNormal), invViewDirection)), glossinessColor * SHININESS);
    float3 specularShading = {specularReflection, specularReflection, specularReflection};

    // Combined
    return float3((lambertShading + specularShading) * observedArea);
}

// Pixel Shader
float4 PS_FLAT(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(gSamplerState, input.TexCoord);
    //return float4(input.Color, 1.f);
}

float4 PS_PHONG(VS_OUTPUT input) : SV_TARGET
{
    //return float4(normalize(input.Normal) * 0.5f + 0.5f, 1.f);
    float3 finalColor = PhongShading(input);
    return float4(finalColor, 1.f);
}

// Technique

technique11 DefaultTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS() ) );
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_PHONG() ) );
    }
}