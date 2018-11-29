#include "Shaders/Common/Uniforms.hlsl"
#include "Shaders/Common/Inputs.hlsl"

struct FragmentOutput
{
	[[vk::location(0)]] float4	color : SV_Target0;
};

FragmentOutput main(FragmentInput i)
{
	FragmentOutput	o;

	o.color = float4(1, 0, 1, 1);

	return o;
}
