sampler2D tex : register(s0);
float4 light;

void main(in float2 b : TEXCOORD, out vector c : COLOR)
{
    c = tex2D(tex, b);
    float tmp = c.w;
    float d1 = b.x * b.x + b.y * b.y;
    float d2 = b.x * b.x + (1 - b.y) * (1 - b.y);
    float d3 = (1 - b.x) * (1 - b.x) + b.y * b.y;
    float d4 = (1 - b.x) * (1 - b.x) + (1 - b.y) * (1 - b.y);
    float lgt = (light.x * d1 + light.y * d2 + light.z * d3 + light.w * d4) / (d1 + d2 + d3 + d4);
    c *= lgt;
    c.w = tmp;
}