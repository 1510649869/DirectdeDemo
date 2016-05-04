float4x4 matWorldViewProj;     //世界*摄影*投影	用于对顶点进行坐标转换
float4x4 matWorld;             //世界坐标系
float4   vecLightDir;        //光照方向向量
float4   vecEye;             //视点坐标(摄影机位置)
float4   vDiffuseColor;      //漫反射光颜色
float4   vSpecularColor;     //镜面高光颜色
float4   vAmbient;           //环境光颜色

//纹理采样器
sampler2D TextureSampler = sampler_state
{
	Filter   = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};
struct VToP
{
	float4 Pos  :  POSITION;
	float2 uv   :  TEXCOORD0;
	float3 L    :  TEXCOORD1;
	float3 N    :  TEXCOORD2;
	float3 V    :  TEXCOORD3;
};
struct Input
{
	float4 Pos:POSITION;
	float3 N  :NORMAL;
	float2 tex:TEXCOORD;
};

VToP VS(Input input)
{
	VToP vtop = (VToP)0;
	vtop.Pos =        mul(input.Pos, matWorldViewProj);     //---变换后位置
	vtop.N   =          mul(input.N, matWorld);                //---变换后法线（仅仅收世界坐标支配）
	float4 PosWorld = mul(input.Pos, matWorld);     //---变换后位置（仅仅受世界坐标支配）
	vtop.L  = vecLightDir;                         //---光照方向向量
	vtop.V = vecEye-PosWorld;               //---相对视点坐标
	vtop.uv = input.tex;
	return vtop;
}
float4 PS(VToP vtop) : COLOR
{
	float3 Normal   = normalize(vtop.N);
	float3 LightDir = normalize(vtop.L);
	float3 ViewDir  = normalize(vtop.V);                                 //---向量相关计算，大多仅用到其方向特性，需要事先单位化。
	//---若是根据其长度考虑衰减，则另当别论，这个在后续点光源的实现中还要介绍。
	float  Diff = saturate(dot(Normal, LightDir));//---光照方向与法线求点积=衰减控制在0--1之间
	//---正面照过来，物体表面反射出的光就多一些；侧面照过来，物体表面反射的光就少一些。这个很好理解~
	float3 Reflect = normalize(reflect(-LightDir, Normal));
	float  Specular = pow(saturate(dot(Reflect, ViewDir)), 4);         //---关于镜面高光的实现思路，关键要看物体表面镜面反射(入射光与反射光对称于顶点法线)之后的反射光是否射入人眼。
	//---我们不太好要求反射光线完全垂直射入人眼（观察向量=反射向量），允许其存在一定的偏向（观察向量与反射向量的夹角小于某一值）。
	//---pow(saturate(dot(Reflect, ViewDir)), 10)即是作此处理，大家可以试着改动下10这个参数，观察效果~
	return vAmbient + vDiffuseColor * Diff + vSpecularColor * Specular + tex2D(TextureSampler, vtop.uv);;   //---最后的颜色 = 环境光 + 漫射光 + 镜面高光
}
technique SpecularLight
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS();
		PixelShader  = compile ps_2_0 PS();
	}
}