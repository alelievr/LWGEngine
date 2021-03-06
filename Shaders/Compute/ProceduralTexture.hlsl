#include "Shaders/Common/InputCompute.hlsl"
#include "Shaders/Common/UniformStructs.hlsl"

[[vk::binding(0, 0)]]
ConstantBuffer< LWGC_PerFrame > frame;

[[vk::binding(0, 1)]]
uniform RWTexture2D< half4 > proceduralTexture;

float2 hash(float2 x)
{
    const float2 k = float2(0.3183099, 0.3678794);
    x = x * k + k.yx;
    return -1.0 + 2.0 * frac(16.0 * k * frac(x.x * x.y * (x.x + x.y)));
}

float noise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);

	float2 u = f*f*(3.0-2.0*f);

    return lerp(lerp(dot(hash(i + float2(0.0,0.0)), f - float2(0.0,0.0)),
                     dot(hash(i + float2(1.0,0.0)), f - float2(1.0,0.0)), u.x),
                lerp(dot(hash(i + float2(0.0,1.0)), f - float2(0.0,1.0)),
                     dot(hash(i + float2(1.0,1.0)), f - float2(1.0,1.0)), u.x), u.y);
}

[numthreads(8, 8, 1)]
void        main(ComputeInput i)
{
	float2 uv = i.dispatchThreadId.xy / 512;
	// proceduralTexture[i.dispatchThreadId.xy] = half4(uv, 1, 1);
	proceduralTexture[i.dispatchThreadId.xy] = half4(noise(uv * 10 + frame.time.x), 0, 0, 1);
}
