#define NOMINMAX
#include "SoftwareRenderer.h"
#include "Mesh.h"
#include "ColorRGB.h"
#include "Texture.h"

using namespace dae;

SoftwareRenderer::SoftwareRenderer(SDL_Window* pWindow)
	: m_pWindow(pWindow)
{
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	//Create Buffers
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
}

void SoftwareRenderer::Render(std::vector<Mesh*> meshes, const Camera& camera)
{
	//@START
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);

	//Clear Buffers
	SDL_FillRect(m_pBackBuffer, 0, SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100));

	//RENDER LOGIC
	Render_Meshes(meshes, camera);

	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void dae::SoftwareRenderer::ToggleDepthBufferVisualization()
{
	m_DepthBufferVisualizationEnabled = !m_DepthBufferVisualizationEnabled;
	std::cout << "Depth Buffer Visualization: " << (m_DepthBufferVisualizationEnabled ? "On\n" : "Off\n");
}

void dae::SoftwareRenderer::ToggleNormalMap()
{
	m_IsNormalMapEnabled = !m_IsNormalMapEnabled;
	std::cout << "Normal Mapping: " << (m_IsNormalMapEnabled ? "On\n" : "Off\n");
}

void dae::SoftwareRenderer::CycleShadingMode()
{
	m_ShadingMode = static_cast<ShadingMode>((static_cast<int>(m_ShadingMode) + 1) % 4);
	std::string modeString{};
	switch (m_ShadingMode)
	{
	case ShadingMode::ObservedArea:
		modeString = "Observed Area";
		break;
	case ShadingMode::Diffuse:
		modeString = "Diffuse";
		break;
	case ShadingMode::Specular:
		modeString = "Specular";
		break;
	case ShadingMode::Combined:
		modeString = "Combined";
		break;
	}
	std::cout << "Shading Mode: " << modeString << '\n';
}

void SoftwareRenderer::Render_Meshes(std::vector<Mesh*> meshes, const Camera& camera)
{
	std::vector<float> depthBuffer(m_Width * m_Height, FLT_MAX);

	for (auto& mesh : meshes)
	{
		if (mesh->GetShaderType() == ShaderType::Flat)
			continue; // Flat shader not supported
		std::vector<Vertex> meshInVertices = mesh->GetVertices();
		std::vector<uint32_t> meshIndices = mesh->GetIndices();

		std::vector<Vertex_Out> meshOutVertices(meshInVertices.size());

		VertexTransformationFunction(meshInVertices, meshOutVertices, mesh->m_WorldMatrix, camera);

		for (size_t i = 0; i < meshInVertices.size();)
		{
			uint32_t v0Index{}, v1Index{}, v2Index{};
			if (IndicesIntermediaryFunction(*mesh, v0Index, v1Index, v2Index, i) == false)
				break;

			// Check if triangle is valid
			if (v0Index == v1Index || v1Index == v2Index || v2Index == v0Index)
				continue;

			//Frustrum Culling
			if (CullTriangle(meshOutVertices[v0Index], meshOutVertices[v1Index], meshOutVertices[v2Index]))
				continue;

			Vertex_Out verticesScreenSpace[3]{
				meshOutVertices[v0Index],
				meshOutVertices[v1Index],
				meshOutVertices[v2Index]
			};

			//Viewport Transform
			verticesScreenSpace[0].position.x = (verticesScreenSpace[0].position.x + 1) / 2 * m_Width;
			verticesScreenSpace[0].position.y = (1 - verticesScreenSpace[0].position.y) / 2 * m_Height;

			verticesScreenSpace[1].position.x = (verticesScreenSpace[1].position.x + 1) / 2 * m_Width;
			verticesScreenSpace[1].position.y = (1 - verticesScreenSpace[1].position.y) / 2 * m_Height;

			verticesScreenSpace[2].position.x = (verticesScreenSpace[2].position.x + 1) / 2 * m_Width;
			verticesScreenSpace[2].position.y = (1 - verticesScreenSpace[2].position.y) / 2 * m_Height;

			//Bounding Box
			int topLeftPixelX = static_cast<int>(std::min({ verticesScreenSpace[0].position.x, verticesScreenSpace[1].position.x, verticesScreenSpace[2].position.x }));
			int topLeftPixelY = static_cast<int>(std::min({ verticesScreenSpace[0].position.y, verticesScreenSpace[1].position.y, verticesScreenSpace[2].position.y }));
			int bottomRightPixelX = static_cast<int>(std::max({ verticesScreenSpace[0].position.x, verticesScreenSpace[1].position.x, verticesScreenSpace[2].position.x })) + 1;
			int bottomRightPixelY = static_cast<int>(std::max({ verticesScreenSpace[0].position.y, verticesScreenSpace[1].position.y, verticesScreenSpace[2].position.y })) + 1;
			topLeftPixelX = std::clamp(topLeftPixelX, 0, m_Width);
			topLeftPixelY = std::clamp(topLeftPixelY, 0, m_Height);
			bottomRightPixelX = std::clamp(bottomRightPixelX, 0, m_Width);
			bottomRightPixelY = std::clamp(bottomRightPixelY, 0, m_Height);

			//Rasterization
			for (int px{ topLeftPixelX }; px < bottomRightPixelX; ++px)
			{
				for (int py{ topLeftPixelY }; py < bottomRightPixelY; ++py)
				{
					Vector2 pixelCenter{ px + 0.5f, py + 0.5f };
					const Vector2 v0{ verticesScreenSpace[0].position.x, verticesScreenSpace[0].position.y };
					const Vector2 v1{ verticesScreenSpace[1].position.x, verticesScreenSpace[1].position.y };
					const Vector2 v2{ verticesScreenSpace[2].position.x, verticesScreenSpace[2].position.y };

					const Vector2 v1v0 = v1 - v0;
					const Vector2 v2v1 = v2 - v1;
					const Vector2 v0v2 = v0 - v2;
					const Vector2 toPixel0 = pixelCenter - v0;
					const Vector2 toPixel1 = pixelCenter - v1;
					const Vector2 toPixel2 = pixelCenter - v2;

					const float crossV0 = Vector2::Cross(v2v1, toPixel1);
					const float crossV1 = Vector2::Cross(v0v2, toPixel2);
					const float crossV2 = Vector2::Cross(v1v0, toPixel0);

					if (crossV0 >= 0 &&
						crossV1 >= 0 &&
						crossV2 >= 0)
					{
						const float triangleArea = Vector2::Cross(v1v0, v2 - v0);
						const float barycentric0 = crossV0 / triangleArea;
						const float barycentric1 = crossV1 / triangleArea;
						const float barycentric2 = crossV2 / triangleArea;

						// Depth Interpolation
						const float zDepthValue = 1 / (barycentric0 / verticesScreenSpace[0].position.z +
							barycentric1 / verticesScreenSpace[1].position.z +
							barycentric2 / verticesScreenSpace[2].position.z);

						if (zDepthValue >= depthBuffer[px + py * m_Width])
							continue;

						depthBuffer[px + py * m_Width] = zDepthValue;

						// Pixel Shading
						ColorRGB finalColor{};
						if (m_DepthBufferVisualizationEnabled)
						{
							const float depthGrayValue = DepthRemap(depthBuffer[px + py * m_Width], 0.93f, 1.f);
							finalColor = ColorRGB{ depthGrayValue, depthGrayValue, depthGrayValue };
						}
						else
						{
							const float w0 = barycentric0 * verticesScreenSpace[0].position.w;
							const float w1 = barycentric1 * verticesScreenSpace[1].position.w;
							const float w2 = barycentric2 * verticesScreenSpace[2].position.w;

							const float interpolatedW = 1 / (w0 + w1 + w2);

							Vertex_Out interpolatedVertex{};
							interpolatedVertex.position = Vector4(
								static_cast<float>(px),
								static_cast<float>(py),
								zDepthValue,
								interpolatedW);

							interpolatedVertex.normal = (
								(verticesScreenSpace[0].normal * w0 +
									verticesScreenSpace[1].normal * w1 +
									verticesScreenSpace[2].normal * w2) * interpolatedW).Normalized();

							interpolatedVertex.tangent = (
								(verticesScreenSpace[0].tangent * w0 +
									verticesScreenSpace[1].tangent * w1 +
									verticesScreenSpace[2].tangent * w2) * interpolatedW).Normalized();

							interpolatedVertex.viewDirection = (
								(verticesScreenSpace[0].viewDirection * w0 +
									verticesScreenSpace[1].viewDirection * w1 +
									verticesScreenSpace[2].viewDirection * w2) * interpolatedW).Normalized();

							interpolatedVertex.uv = (
								verticesScreenSpace[0].uv * w0 +
								verticesScreenSpace[1].uv * w1 +
								verticesScreenSpace[2].uv * w2) * interpolatedW;

							finalColor = PixelShading(interpolatedVertex, *mesh);
						}

						finalColor.MaxToOne();

						m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
							static_cast<uint8_t>(finalColor.r * 255),
							static_cast<uint8_t>(finalColor.g * 255),
							static_cast<uint8_t>(finalColor.b * 255));
					}
				}
			}
		}
	}
}

float SoftwareRenderer::DepthRemap(float value, float min, float max) const
{
	const float depthMax = 1.f;
	const float depthMin = 0.f;
	return depthMin + (value - min) * (depthMax - depthMin) / (max - min);
}

ColorRGB SoftwareRenderer::PixelShading(const Vertex_Out& v, const Mesh& mesh) const
{
	// Light Constants
	const Vector3 lightDirection{ 0.577f, -0.577f, 0.577f };
	const ColorRGB ambientLight{ 0.03f, 0.03f, 0.03f };

	// Normal Mapping
	const Vector3 binormal = Vector3::Cross(v.normal, v.tangent).Normalized();
	const Matrix tangentSpaceAxis{ v.tangent, binormal, v.normal, Vector3::Zero };

	Vector3 sampledNormal{};
	if (!mesh.GetNormalMap() || !m_IsNormalMapEnabled)
		sampledNormal = v.normal;
	else
	{
		const ColorRGB sampledNormalColor = mesh.GetNormalMap()->Sample(v.uv);
		sampledNormal = Vector3{ sampledNormalColor.r, sampledNormalColor.g, sampledNormalColor.b };
		sampledNormal = (2.f * sampledNormal) - Vector3{ 1.f,1.f,1.f };
		sampledNormal = tangentSpaceAxis.TransformVector(sampledNormal).Normalized();
	}

	// Observed Area
	const float observedArea = std::max(Vector3::Dot(sampledNormal, -lightDirection.Normalized()), 0.f);
	if (m_ShadingMode == ShadingMode::ObservedArea)
	{
		return ColorRGB{ observedArea, observedArea, observedArea };
	}

	// Diffuse
	const float kd = 7.0f;
	const ColorRGB diffuseColor = mesh.GetDiffuseTexture() ? mesh.GetDiffuseTexture()->Sample(v.uv) : colors::Magenta;
	const ColorRGB lambertBRDF = (kd * diffuseColor) / static_cast<float>(M_PI);

	if (m_ShadingMode == ShadingMode::Diffuse)
	{
		return (lambertBRDF * observedArea);
	}

	// Specular
	const float shininess = 25.f;
	const float specularColor = mesh.GetSpecularMap() ? mesh.GetSpecularMap()->Sample(v.uv).r : 1.f;
	const float phongExponentColor = mesh.GetGlossinessMap() ? mesh.GetGlossinessMap()->Sample(v.uv).r : 1.f;
	const float specularReflection = specularColor * powf(std::abs(Vector3::Dot(Vector3::Reflect(-lightDirection, sampledNormal), v.viewDirection)), phongExponentColor * shininess);
	ColorRGB specularBRDF = ColorRGB{ specularReflection, specularReflection, specularReflection };

	if (m_ShadingMode == ShadingMode::Specular)
	{
		return specularBRDF * observedArea;
	}

	// Combined
	return ambientLight + (lambertBRDF + specularBRDF) * observedArea;
}

bool SoftwareRenderer::CullTriangle(const Vertex_Out& v0, const Vertex_Out& v1, const Vertex_Out& v2) const
{
	if (v0.position.x < -1 || v0.position.x > 1 ||
		v1.position.x < -1 || v1.position.x > 1 ||
		v2.position.x < -1 || v2.position.x > 1)
	{
		return true;
	}
	if (v0.position.y < -1 || v0.position.y > 1 ||
		v1.position.y < -1 || v1.position.y > 1 ||
		v2.position.y < -1 || v2.position.y > 1)
	{
		return true;
	}
	if (v0.position.z < 0 || v0.position.z > 1 ||
		v1.position.z < 0 || v1.position.z > 1 ||
		v2.position.z < 0 || v2.position.z > 1)
	{
		return true;
	}
	return false;
}

void SoftwareRenderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out, const Matrix& worldMatrix, const Camera& camera) const
{
	vertices_out.clear();

	Matrix viewMatrix = camera.viewMatrix;
	Matrix projectionMatrix = camera.projectionMatrix;
	Matrix worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;

	for (const Vertex& vertex : vertices_in)
	{
		Vertex_Out vertex_out{};
		vertex_out.position = worldViewProjectionMatrix.TransformPoint(Vector4(vertex.position, 1));
		vertex_out.position.w = 1.f / vertex_out.position.w;

		vertex_out.normal = worldMatrix.TransformVector(vertex.normal).Normalized();
		vertex_out.tangent = worldMatrix.TransformVector(vertex.tangent).Normalized();

		//Perspective Divide
		vertex_out.position.x *= vertex_out.position.w;
		vertex_out.position.y *= vertex_out.position.w;
		vertex_out.position.z *= vertex_out.position.w;

		vertex_out.viewDirection = (camera.origin - vertex_out.position.GetXYZ()).Normalized();

		vertex_out.uv = vertex.uv;
		vertices_out.emplace_back(vertex_out);
	}
}

bool SoftwareRenderer::IndicesIntermediaryFunction(const Mesh& mesh, uint32_t& v0Index, uint32_t& v1Index, uint32_t& v2Index, size_t& i) const
{
	std::vector<uint32_t> indices = mesh.GetIndices();
	if (mesh.GetPrimitiveTopology() == PrimitiveTopology::TriangleStrip)
	{
		if (i + 2 >= indices.size())
			return false;
		v0Index = indices[i];
		if (i % 2 == 0)
		{
			v1Index = indices[i + 1];
			v2Index = indices[i + 2];
		}
		else
		{
			v1Index = indices[i + 2];
			v2Index = indices[i + 1];
		}
		++i;
	}
	else //Triangle List
	{
		v0Index = indices[i];
		v1Index = indices[i + 1];
		v2Index = indices[i + 2];
		i += 3;
	}
	return true;
}

bool SoftwareRenderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}