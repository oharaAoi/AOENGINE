// HLSL adaptation of user-supplied https://www.shadertoy.com/view/ddcfDN.
// Blackbody function credited by the source to fishy (DtX3W8).
// Hash functions credited by the source to Dave Hoskins (4djSRW).
// Changes: world-space surface integration, continuous flow, bounded temperature,
// HDR output; source screen-space brightest-neighbour bokeh is not included.
#include "../Object3d.hlsli"
struct MagmaParameters { float time; float patternScale; float temperature; float intensity; };
ConstantBuffer<MagmaParameters> gMagma : register(b0);
float responseCurve(float w, float p, float v) {
    return sin(6.28318530718 * saturate((v-p)/w+0.5)-1.57079632679)*0.5+0.5;
}
float3 blackbody(float t) {
    float3 col = 0, m = 0;
    t = max(t, 1.0);
    [unroll] for (int i=0; i<4; ++i) {
        float v=400.0+75.0*i;
        float3 r=float3(responseCurve(100,440,v),responseCurve(200,550,v),responseCurve(200,600,v));
        m+=r;
        // Clamp the exponent to prevent overflow for cold rock.
        col+=2*v*v*v/(exp(min(v/t,80.0))-1)*r;
    }
    return col/max(m,0.0001);
}
float2 hash22(float2 p) {
    float3 p3=frac(p.xyx*float3(.1031,.1030,.0973));
    p3+=dot(p3,p3.yzx+33.33);
    return frac((p3.xx+p3.yz)*p3.zy);
}
float3 hash33(float3 p) {
    p=frac(p*float3(.1031,.1030,.0973));
    p+=dot(p,p.yxz+33.33);
    return frac((p.xxy+p.yxx)*p.zyx);
}
float2 cell2(float2 uv) {
    float best=999; float2 cell=0;
    [unroll] for(int x=-1;x<=1;++x) {
        [unroll] for(int y=-1;y<=1;++y) {
            float2 base=floor(uv)+float2(x,y);
            float2 cellPosition=base+hash22(base);
            float d=dot(cellPosition-uv,cellPosition-uv);
            if(d<best) {best=d;cell=cellPosition;}
        }
    }
    return cell;
}
float edges3(float3 uv) {
    float d1=999,d2=999;
    [unroll] for(int x=-1;x<=1;++x) {
        [unroll] for(int y=-1;y<=1;++y) {
            [unroll] for(int z=-1;z<=1;++z) {
                float3 base=floor(uv)+float3(x,y,z);
                float d=distance(base+hash33(base),uv);
                if(d<d1) {d2=d1;d1=d;} else if(d<d2) d2=d;
            }
        }
    }
    return (abs(d1-d2)+1-d1)*0.5;
}
struct Output { float4 color:SV_TARGET0; float4 motion:SV_TARGET1; };
Output main(VertexShaderOutput input) {
    Output o;
    float2 uv=input.texcoord*gMagma.patternScale+float2(gMagma.time,gMagma.time*.27);
    float2 cell=cell2(uv)/16;
    float random=.1*edges3(float3(cell*5,10));
    float cracks=edges3(float3(cell,0));
    float lines=edges3(float3(cell,10));
    float temperature=gMagma.temperature-cracks*30-random*40;
    float3 rock=lerp(float3(.025,.018,.03),float3(.004,.004,.006),smoothstep(0,.3,lines+random));
    float heat=smoothstep(0,1,(temperature-20)*.25);
    float3 lava=blackbody(temperature)*gMagma.intensity+float3(.008,.004,.003);
    o.color=float4(lerp(rock,lava,heat),1);
    float2 motion=input.positionNDC.xy/input.positionNDC.w-input.positionPrev.xy/input.positionPrev.w;
    o.motion=float4(abs(motion),0,1);
    return o;
}
