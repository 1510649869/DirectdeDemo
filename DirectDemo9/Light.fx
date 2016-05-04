float4x4 matWorldViewProj;     //����*��Ӱ*ͶӰ	���ڶԶ����������ת��
float4x4 matWorld;             //��������ϵ
float4   vecLightDir;        //���շ�������
float4   vecEye;             //�ӵ�����(��Ӱ��λ��)
float4   vDiffuseColor;      //���������ɫ
float4   vSpecularColor;     //����߹���ɫ
float4   vAmbient;           //��������ɫ

//���������
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
	vtop.Pos =        mul(input.Pos, matWorldViewProj);     //---�任��λ��
	vtop.N   =          mul(input.N, matWorld);                //---�任���ߣ���������������֧�䣩
	float4 PosWorld = mul(input.Pos, matWorld);     //---�任��λ�ã���������������֧�䣩
	vtop.L  = vecLightDir;                         //---���շ�������
	vtop.V = vecEye-PosWorld;               //---����ӵ�����
	vtop.uv = input.tex;
	return vtop;
}
float4 PS(VToP vtop) : COLOR
{
	float3 Normal   = normalize(vtop.N);
	float3 LightDir = normalize(vtop.L);
	float3 ViewDir  = normalize(vtop.V);                                 //---������ؼ��㣬�����õ��䷽�����ԣ���Ҫ���ȵ�λ����
	//---���Ǹ����䳤�ȿ���˥�����������ۣ�����ں������Դ��ʵ���л�Ҫ���ܡ�
	float  Diff = saturate(dot(Normal, LightDir));//---���շ����뷨������=˥��������0--1֮��
	//---�����չ�����������淴����Ĺ�Ͷ�һЩ�������չ�����������淴��Ĺ����һЩ������ܺ����~
	float3 Reflect = normalize(reflect(-LightDir, Normal));
	float  Specular = pow(saturate(dot(Reflect, ViewDir)), 4);         //---���ھ���߹��ʵ��˼·���ؼ�Ҫ��������澵�淴��(������뷴���Գ��ڶ��㷨��)֮��ķ�����Ƿ��������ۡ�
	//---���ǲ�̫��Ҫ���������ȫ��ֱ�������ۣ��۲�����=���������������������һ����ƫ�򣨹۲������뷴�������ļн�С��ĳһֵ����
	//---pow(saturate(dot(Reflect, ViewDir)), 10)�������˴�����ҿ������ŸĶ���10����������۲�Ч��~
	return vAmbient + vDiffuseColor * Diff + vSpecularColor * Specular + tex2D(TextureSampler, vtop.uv);;   //---������ɫ = ������ + ����� + ����߹�
}
technique SpecularLight
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS();
		PixelShader  = compile ps_2_0 PS();
	}
}