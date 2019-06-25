#include "Buffer.h"

CBLight::CBLight(LightSource* lightSource)
{
	Light light = lightSource->GetLight();

	this->Active      = Utils::ToVec4Float(light.active, (int)lightSource->SourceType());
	this->Ambient     = glm::vec4(light.material.ambient, 0.0f);
	this->Attenuation = glm::vec4(light.attenuation.constant, light.attenuation.linear, light.attenuation.quadratic, 0.0f);
	this->Diffuse     = light.material.diffuse;
	this->Direction   = glm::vec4(light.direction, 0.0f);
	this->Specular    = glm::vec4(light.material.specular.intensity, light.material.specular.shininess);
	this->Position    = glm::vec4(light.position,  0.0f);

	if ((light.innerAngle > 0.1f) && (light.outerAngle > light.innerAngle))
		this->Angles = glm::vec4(glm::cos(light.innerAngle), glm::cos(light.outerAngle), 0.0f, 0.0f);

	this->ViewProjection = (lightSource->Projection() * lightSource->View(0));
}

CBMatrix::CBMatrix(Component* mesh, bool removeTranslation)
{
	this->Model  = mesh->Matrix();
	this->Normal = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->Model))));
	this->MVP    = RenderEngine::CameraMain->MVP(this->Model, removeTranslation);
}

CBMatrix::CBMatrix(LightSource* lightSource, Component* mesh)
{
	glm::mat4 depthTransform = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.5f, 1.0f
	);

	this->Model = mesh->Matrix();
	this->MVP   = lightSource->MVP(this->Model);

	glm::mat4 projection = lightSource->Projection();

	for (uint32_t i = 0; i < MAX_TEXTURES; i++)
		this->VP[i] = (projection * lightSource->View(i));

	// Apply the depth transform
	switch (RenderEngine::SelectedGraphicsAPI) {
	case GRAPHICS_API_DIRECTX11:
	case GRAPHICS_API_DIRECTX12:
		depthTransform[1][1] *= -1.0f;

		this->MVP = (depthTransform * this->MVP);

		for (uint32_t i = 0; i < MAX_TEXTURES; i++)
			this->VP[i] = (depthTransform * this->VP[i]);

		break;
	case GRAPHICS_API_VULKAN:
		this->MVP = (depthTransform * this->MVP);
		break;
	default:
		break;
	}
}

CBColor::CBColor(const glm::vec4 &color)
{
	this->Color = color;
}

CBDefault::CBDefault(Component* mesh, const DrawProperties &properties)
{
	for (uint32_t i = 0; i < MAX_LIGHT_SOURCES; i++) {
		if (SceneManager::LightSources[i] != nullptr)
			this->LightSources[i] = CBLight(SceneManager::LightSources[i]);
	}

	for (int i = 0; i < MAX_TEXTURES; i++)
		this->IsTextured[i] = Utils::ToVec4Float(mesh->IsTextured(i));

	for (int i = 0; i < MAX_TEXTURES; i++)
		this->TextureScales[i] = glm::vec4(mesh->Textures[i]->Scale.x, mesh->Textures[i]->Scale.y, 0.0f, 0.0f);

	this->MeshSpecular = glm::vec4(mesh->ComponentMaterial.specular.intensity, mesh->ComponentMaterial.specular.shininess);
	this->MeshDiffuse  = mesh->ComponentMaterial.diffuse;

	this->ClipMax        = glm::vec4(properties.ClipMax, 0.0f);
	this->ClipMin        = glm::vec4(properties.ClipMin, 0.0f);
	this->EnableClipping = Utils::ToVec4Float(properties.EnableClipping);

	this->CameraPosition = glm::vec4(RenderEngine::CameraMain->Position(), 0.0f);
	this->ComponentType  = Utils::ToVec4Float(static_cast<int>(mesh->Type()));
	this->EnableSRGB     = Utils::ToVec4Float(RenderEngine::EnableSRGB);
	this->WaterProps     = {};

	if (mesh->Type() == COMPONENT_WATER) {
		auto water = dynamic_cast<Water*>(mesh->Parent);
		this->WaterProps = { water->FBO()->MoveFactor(), water->FBO()->WaveStrength, 0.0f, 0.0f };
	}
}

CBDepth::CBDepth(const glm::vec3 &lightPosition, int depthLayer)
{
	this->lightPosition = glm::vec4(lightPosition, static_cast<float>(depthLayer));
}

CBHUD::CBHUD(const glm::vec4 &color, bool transparent)
{
	this->MaterialColor = color;
	this->IsTransparent = Utils::ToVec4Float(transparent);
}

#if defined _WINDOWS

CBLightDX::CBLightDX(LightSource* lightSource)
{
	Light light = lightSource->GetLight();

	this->Active      = Utils::ToXMFLOAT4(light.active, (int)lightSource->SourceType());
	this->Ambient     = Utils::ToXMFLOAT4(light.material.ambient, 0.0f);
	this->Attenuation = { light.attenuation.constant, light.attenuation.linear, light.attenuation.quadratic, 0.0f };
	this->Diffuse     = Utils::ToXMFLOAT4(light.material.diffuse);
	this->Direction   = Utils::ToXMFLOAT4(light.direction, 0.0f);
	this->Position    = Utils::ToXMFLOAT4(light.position,  0.0f);
	this->Specular    = Utils::ToXMFLOAT4(light.material.specular.intensity, light.material.specular.shininess);

	if ((light.innerAngle > 0.1f) && (light.outerAngle > light.innerAngle))
		this->Angles = { glm::cos(light.innerAngle), glm::cos(light.outerAngle), 0.0f, 0.0f };

	this->ViewProjection = Utils::ToXMMATRIX(lightSource->Projection() * lightSource->View(0));
}

CBLightDX::CBLightDX(const CBLight &light)
{
	this->Active         = Utils::ToXMFLOAT4(light.Active);
	this->Ambient        = Utils::ToXMFLOAT4(light.Ambient);
	this->Angles         = Utils::ToXMFLOAT4(light.Angles);
	this->Attenuation    = Utils::ToXMFLOAT4(light.Attenuation);
	this->Diffuse        = Utils::ToXMFLOAT4(light.Diffuse);
	this->Direction      = Utils::ToXMFLOAT4(light.Direction);
	this->Position       = Utils::ToXMFLOAT4(light.Position);
	this->Specular       = Utils::ToXMFLOAT4(light.Specular);
	this->ViewProjection = Utils::ToXMMATRIX(light.ViewProjection);
}

CBMatrixDX::CBMatrixDX(const CBMatrix &matrices)
{
	this->Normal = Utils::ToXMMATRIX(matrices.Normal);
	this->Model  = Utils::ToXMMATRIX(matrices.Model);
	this->MVP    = Utils::ToXMMATRIX(matrices.MVP);

	for (uint32_t i = 0; i < MAX_TEXTURES; i++)
		this->VP[i] = Utils::ToXMMATRIX(matrices.VP[i]);
}

CBColorDX::CBColorDX(const CBMatrix &matrices, const glm::vec4 &color)
{
	this->MB    = CBMatrixDX(matrices);
	this->Color = Utils::ToXMFLOAT4(color);
}

CBDefaultDX::CBDefaultDX(const CBMatrix &matrices, Component* mesh, const glm::vec3 &clipMax, const glm::vec3 &clipMin, bool enableClipping)
{
	this->MB = CBMatrixDX(matrices);

	for (uint32_t i = 0; i < MAX_LIGHT_SOURCES; i++) {
		if (SceneManager::LightSources[i] != nullptr)
			this->LightSources[i] = CBLightDX(SceneManager::LightSources[i]);
	}

	for (int i = 0; i < MAX_TEXTURES; i++)
		this->IsTextured[i] = Utils::ToXMFLOAT4(mesh->IsTextured(i));

	for (int i = 0; i < MAX_TEXTURES; i++)
		this->TextureScales[i] = DirectX::XMFLOAT4(mesh->Textures[i]->Scale.x, mesh->Textures[i]->Scale.y, 0.0f, 0.0f);

	this->MeshSpecular = Utils::ToXMFLOAT4(mesh->ComponentMaterial.specular.intensity, mesh->ComponentMaterial.specular.shininess);
	this->MeshDiffuse  = Utils::ToXMFLOAT4(mesh->ComponentMaterial.diffuse);

	this->ClipMax        = Utils::ToXMFLOAT4(clipMax, 0.0f);
	this->ClipMin        = Utils::ToXMFLOAT4(clipMin, 0.0f);
	this->EnableClipping = Utils::ToXMFLOAT4(enableClipping);

	this->CameraPosition = Utils::ToXMFLOAT4(RenderEngine::CameraMain->Position(), 0.0f);
	this->ComponentType  = Utils::ToXMFLOAT4(static_cast<int>(mesh->Type()));
	this->EnableSRGB     = Utils::ToXMFLOAT4(RenderEngine::EnableSRGB);
	this->WaterProps     = {};

	if (mesh->Type() == COMPONENT_WATER) {
		auto water = dynamic_cast<Water*>(mesh->Parent);
		this->WaterProps = { water->FBO()->MoveFactor(), water->FBO()->WaveStrength, 0.0f, 0.0f };
	}
}

CBDefaultDX::CBDefaultDX(const CBDefault &default, const CBMatrix &matrices)
{
	this->MB = CBMatrixDX(matrices);

	for (uint32_t i = 0; i < MAX_LIGHT_SOURCES; i++)
		this->LightSources[i] = CBLightDX(default.LightSources[i]);

	for (int i = 0; i < MAX_TEXTURES; i++)
		this->IsTextured[i] = Utils::ToXMFLOAT4(default.IsTextured[i]);

	for (int i = 0; i < MAX_TEXTURES; i++)
		this->TextureScales[i] = Utils::ToXMFLOAT4(default.TextureScales[i]);

	this->MeshSpecular = Utils::ToXMFLOAT4(default.MeshSpecular);
	this->MeshDiffuse  = Utils::ToXMFLOAT4(default.MeshDiffuse);

	this->ClipMax        = Utils::ToXMFLOAT4(default.ClipMax);
	this->ClipMin        = Utils::ToXMFLOAT4(default.ClipMin);
	this->EnableClipping = Utils::ToXMFLOAT4(default.EnableClipping);

	this->CameraPosition = Utils::ToXMFLOAT4(default.CameraPosition);
	this->ComponentType  = Utils::ToXMFLOAT4(default.ComponentType);
	this->EnableSRGB     = Utils::ToXMFLOAT4(default.EnableSRGB);
	this->WaterProps     = Utils::ToXMFLOAT4(default.WaterProps);
}

CBDepthDX::CBDepthDX(const CBMatrix &matrices, const glm::vec3 &lightPosition, int depthLayer)
{
	this->MB            = CBMatrixDX(matrices);
	this->lightPosition = Utils::ToXMFLOAT4(lightPosition, static_cast<float>(depthLayer));
}

CBHUDDX::CBHUDDX(const CBMatrix &matrices, const glm::vec4 &color, bool transparent)
{
	this->MB            = CBMatrixDX(matrices);
	this->MaterialColor = Utils::ToXMFLOAT4(color);
	this->IsTransparent = Utils::ToXMFLOAT4(transparent);
}

CBSkyboxDX::CBSkyboxDX(const CBMatrix &matrices)
{
	this->MB = CBMatrixDX(matrices);
}

#endif

Buffer::Buffer(std::vector<uint32_t> &indices)
{
	this->init();

	switch (RenderEngine::SelectedGraphicsAPI) {
	#if defined _WINDOWS
	case GRAPHICS_API_DIRECTX11:
		RenderEngine::Canvas.DX->CreateIndexBuffer11(indices, this);
		break;
	case GRAPHICS_API_DIRECTX12:
		RenderEngine::Canvas.DX->CreateIndexBuffer12(indices, this);
		break;
	#endif
	case GRAPHICS_API_OPENGL:
		glCreateBuffers(1, &this->id);

		if (id > 0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (indices.size() * sizeof(uint32_t)), &indices[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		break;
	case GRAPHICS_API_VULKAN:
		RenderEngine::Canvas.VK->CreateIndexBuffer(indices, this);
		break;
	default:
		throw;
	}
}

Buffer::Buffer(std::vector<float> &data)
{
	this->init();

	glCreateBuffers(1, &this->id);

	if (id > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, (data.size() * sizeof(float)), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

Buffer::Buffer(std::vector<float> &vertices, std::vector<float> &normals, std::vector<float> &texCoords)
{
	this->init();

	this->normals   = normals;
	this->texCoords = texCoords;
	this->vertices  = vertices;

	switch (RenderEngine::SelectedGraphicsAPI) {
	case GRAPHICS_API_DIRECTX11:
		RenderEngine::Canvas.DX->CreateVertexBuffer11(vertices, normals, texCoords, this);
		RenderEngine::Canvas.DX->CreateConstantBuffers11(this);
		break;
	case GRAPHICS_API_DIRECTX12:
		RenderEngine::Canvas.DX->CreateVertexBuffer12(vertices, normals, texCoords, this);
		RenderEngine::Canvas.DX->CreateConstantBuffers12(this);
		break;
	case GRAPHICS_API_VULKAN:
		RenderEngine::Canvas.VK->CreateVertexBuffer(vertices, normals, texCoords, this);
		RenderEngine::Canvas.VK->InitPipelines(this);
		break;
	default:
		throw;
	}
}

Buffer::Buffer()
{
	this->init();
}

Buffer::~Buffer()
{
	#if defined _WINDOWS
		for (uint32_t i = 0; i < NR_OF_SHADERS; i++)
		{
			_RELEASEP(this->ConstantBuffersDX11[i]);
			_RELEASEP(this->ConstantBuffersDX12[i]);
			_RELEASEP(this->SamplerHeapsDX12[i]);
			_RELEASEP(this->ConstantBufferHeapsDX12[i]);

			_RELEASEP(this->DepthStencilStatesDX11[i]);
			_RELEASEP(this->BlendStatesDX11[i]);
			_RELEASEP(this->RasterizerStatesDX11[i]);
			_RELEASEP(this->InputLayoutsDX11[i]);

			_RELEASEP(this->PipelineStatesDX12[i]);
			_RELEASEP(this->PipelineStatesColorDX12[i]);
			_RELEASEP(this->PipelineStatesDepthDX12[i]);
			_RELEASEP(this->RootSignaturesDX12[i]);
			_RELEASEP(this->RootSignaturesColorDX12[i]);
			_RELEASEP(this->RootSignaturesDepthDX12[i]);
		}

		_RELEASEP(this->VertexBufferDX11);
		_RELEASEP(this->VertexBufferDX12);
	#endif

	if (this->id > 0) {
		glDeleteBuffers(1, &this->id);
		this->id = 0;
	}

	for (uint32_t i = 0; i < NR_OF_UBOS_VK; i++)
		RenderEngine::Canvas.VK->DestroyBuffer(&this->Uniform.Buffers[i], &this->Uniform.BufferMemories[i]);

	for (uint32_t i = 0; i < NR_OF_SHADERS; i++) {
		RenderEngine::Canvas.VK->DestroyPipeline(&this->Pipeline.Pipelines[i]);
		RenderEngine::Canvas.VK->DestroyPipeline(&this->Pipeline.PipelinesFBO[i]);
	}

	RenderEngine::Canvas.VK->DestroyPipelineLayout(&this->Pipeline.Layout);
	RenderEngine::Canvas.VK->DestroyUniformSet(&this->Uniform.Pool, &this->Uniform.Layout);
	RenderEngine::Canvas.VK->DestroyBuffer(&this->IndexBuffer,  &this->IndexBufferMemory);
	RenderEngine::Canvas.VK->DestroyBuffer(&this->VertexBuffer, &this->VertexBufferMemory);
}

void Buffer::init()
{
	this->BufferStride       = 0;
	this->id                 = 0;
	this->IndexBuffer        = nullptr;
	this->IndexBufferMemory  = nullptr;
	this->Pipeline           = {};
	this->Uniform            = {};
	this->VertexBuffer       = nullptr;
	this->VertexBufferMemory = nullptr;

	#if defined _WINDOWS
		this->VertexBufferDX11     = nullptr;
		this->VertexBufferDX12     = nullptr;
		this->IndexBufferViewDX12  = {};
		this->VertexBufferViewDX12 = {};

		this->BlendStatesDX11[NR_OF_SHADERS]         = {};
		this->ConstantBuffersDX11[NR_OF_SHADERS]     = {};
		this->ConstantBuffersDX12[NR_OF_SHADERS]     = {};
		this->ConstantBufferHeapsDX12[NR_OF_SHADERS] = {};
		this->DepthStencilStatesDX11[NR_OF_SHADERS]  = {};
		this->InputLayoutsDX11[NR_OF_SHADERS]        = {};
		this->RasterizerStatesDX11[NR_OF_SHADERS]    = {};
		this->PipelineStatesDX12[NR_OF_SHADERS]      = {};
		this->PipelineStatesColorDX12[NR_OF_SHADERS] = {};
		this->PipelineStatesDepthDX12[NR_OF_SHADERS] = {};
		this->RootSignaturesDX12[NR_OF_SHADERS]      = {};
		this->RootSignaturesColorDX12[NR_OF_SHADERS] = {};
		this->RootSignaturesDepthDX12[NR_OF_SHADERS] = {};
		this->SamplerHeapsDX12[NR_OF_SHADERS]        = {};

		this->ConstantBufferColor   = {};
		this->ConstantBufferDefault = {};
		this->ConstantBufferDepth   = {};
		this->ConstantBufferHUD     = {};
		this->ConstantBufferSkybox  = {};
	#endif
}

GLuint Buffer::ID()
{
	return this->id;
}

size_t Buffer::Normals()
{
	return this->normals.size();
}

void Buffer::ResetPipelines()
{
	for (uint32_t i = 0; i < NR_OF_SHADERS; i++) {
		RenderEngine::Canvas.VK->DestroyPipeline(&Pipeline.Pipelines[i]);
		RenderEngine::Canvas.VK->DestroyPipeline(&Pipeline.PipelinesFBO[i]);
	}

	RenderEngine::Canvas.VK->DestroyBuffer(&this->VertexBuffer, &this->VertexBufferMemory);
	RenderEngine::Canvas.VK->CreateVertexBuffer(this->vertices, this->normals, this->texCoords, this);
}

size_t Buffer::TexCoords()
{
	return this->texCoords.size();
}

size_t Buffer::Vertices()
{
	return this->vertices.size();
}
