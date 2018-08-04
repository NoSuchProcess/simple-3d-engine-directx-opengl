#ifndef GE3D_GLOBALS_H
#include "../globals.h"
#endif

#ifndef GE3D_BUFFER_H
#define GE3D_BUFFER_H

class Buffer
{
public:
	Buffer(std::vector<uint32_t> &indices);
	Buffer(std::vector<float> &data);
	Buffer(std::vector<float> &vertices, std::vector<float> &normals, std::vector<float> &texCoords);
	Buffer();
	~Buffer();

public:
	#if defined _WINDOWS
		ID3D11BlendState*        BlendStatesDX11[NR_OF_SHADERS];
		ID3D11DepthStencilState* DepthStencilStatesDX11[NR_OF_SHADERS];
		ID3D11InputLayout*       InputLayoutsDX11[NR_OF_SHADERS];
		ID3D11RasterizerState*   RasterizerStatesDX11[NR_OF_SHADERS];
		ID3D12PipelineState*     PipelineStatesDX12[NR_OF_SHADERS];
		ID3D12RootSignature*     RootSignaturesDX12[NR_OF_SHADERS];

		ID3D11Buffer*         ConstantBuffersDX11[NR_OF_SHADERS];
		ID3D12Resource*       ConstantBuffersDX12[NR_OF_SHADERS];
		ID3D12DescriptorHeap* ConstantBufferHeapsDX12[NR_OF_SHADERS];
		ID3D12DescriptorHeap* SamplerHeapsDX12[NR_OF_SHADERS];

		DXMatrixBuffer  MatrixBufferValues;
		DXLightBuffer   LightBufferValues;
		DXDefaultBuffer DefaultBufferValues;
		DXHUDBuffer     HUDBufferValues;
		DXSkyboxBuffer  SkyboxBufferValues;
		DXSolidBuffer   SolidBufferValues;
		DXTerrainBuffer TerrainBufferValues;
		DXWaterBuffer   WaterBufferValues;
	#endif

private:
	UINT           bufferStride;
	GLuint         id;
	VkBuffer       indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkPipeline     pipelines[NR_OF_SHADERS];
	VkBuffer       uniformBuffers[NR_OF_UNIFORM_BUFFERS];
	VkDeviceMemory uniformBufferMemories[NR_OF_UNIFORM_BUFFERS];
	VkBuffer       vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	#if defined _WINDOWS
		ID3D11Buffer*            bufferDX11;
		ID3D12Resource*          bufferDX12;
		D3D12_INDEX_BUFFER_VIEW  indexBufferViewDX12;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferViewDX12;
	#endif

public:
	UINT           BufferStride();
	GLuint         ID();
	VkBuffer       IndexBuffer();
	VkPipeline     Pipeline(ShaderID shaderID);
	VkBuffer       UniformBuffer(UniformBufferType uniformBuffer);
	VkDeviceMemory UniformBufferMemory(UniformBufferType uniformBuffer);
	VkBuffer       VertexBuffer();

	#if defined _WINDOWS
		ID3D11Buffer*            BufferDX11();
		D3D12_INDEX_BUFFER_VIEW  IndexBufferViewDX12();
		D3D12_VERTEX_BUFFER_VIEW VertexBufferViewDX12();
	#endif

private:
	void init();

};

#endif