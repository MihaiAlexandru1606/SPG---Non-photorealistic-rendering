#include "NPR.hpp"

#include <vector>
#include <iostream>
#include <algorithm>
#include <map>

#include <Core/Engine.h>

NPR::NPR()
{
	outputMode = 0;
	gpuProcessing = false;
	saveScreenToImage = false;
	window->SetSize(600, 600);
}

NPR::~NPR()
{
}

void NPR::Init()
{
	// Load default texture fore imagine processing 
	originalImage = TextureManager::
		LoadTexture(RESOURCE_PATH::TEXTURES + "Cube/posx.png", nullptr, "image", true, true);
	processedImage = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "Cube/posx.png", nullptr, "newImage", true,
	                                             true);

	SobelOutput = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "Cube/posx.png", nullptr, "newImage", true,
	                                          true);
	DilationOutput = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "Cube/posx.png", nullptr, "newImage", true,
	                                             true);
	CombineOutput = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "Cube/posx.png", nullptr, "newImage", true,
	                                            true);
	ColorNarrowingOutput = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "Cube/posx.png", nullptr, "newImage",
	                                                   true,
	                                                   true);
	SegmentationOutput = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "Cube/posx.png", nullptr, "newImage",
	                                                 true,
	                                                 true);

	{
		Mesh* mesh = new Mesh("quad");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	static std::string shaderPath = "Source/NPR/Shaders/";

	// Create a shader program for particle system
	{
		Shader* shader = new Shader("ImageProcessing");
		shader->AddShader((shaderPath + "VertexShader.glsl").c_str(), GL_VERTEX_SHADER);
		shader->AddShader((shaderPath + "FragmentShader.glsl").c_str(), GL_FRAGMENT_SHADER);

		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	this->number_interval = 10;
	this->state = 0;
}

void NPR::FrameStart()
{
}

void NPR::Update(float deltaTimeSeconds)
{
	ClearScreen();
	Texture2D* vector_text[6]{
		originalImage, SobelOutput, DilationOutput, CombineOutput, ColorNarrowingOutput, SegmentationOutput
	};
	auto shader = shaders["ImageProcessing"];
	shader->Use();

	if (saveScreenToImage)
	{
		window->SetSize(originalImage->GetWidth(), originalImage->GetHeight());
	}

	int flip_loc = shader->GetUniformLocation("flipVertical");
	glUniform1i(flip_loc, saveScreenToImage ? 0 : 1);

	int screenSize_loc = shader->GetUniformLocation("screenSize");
	glm::ivec2 resolution = window->GetResolution();
	glUniform2i(screenSize_loc, resolution.x, resolution.y);

	int outputMode_loc = shader->GetUniformLocation("outputMode");
	glUniform1i(outputMode_loc, outputMode);

	int gpuProcessing_loc = shader->GetUniformLocation("outputMode");
	glUniform1i(outputMode_loc, outputMode);

	int locTexture = shader->GetUniformLocation("textureImage");
	glUniform1i(locTexture, 0);

	auto textureImage = vector_text[state];
	textureImage->BindToTextureUnit(GL_TEXTURE0);

	RenderMesh(meshes["quad"], shader, glm::mat4(1));

	if (saveScreenToImage)
	{
		saveScreenToImage = false;
		GLenum format = GL_RGB;
		if (originalImage->GetNrChannels() == 4)
		{
			format = GL_RGBA;
		}
		glReadPixels(0, 0, originalImage->GetWidth(), originalImage->GetHeight(), format, GL_UNSIGNED_BYTE,
		             vector_text[state]->GetImageData());
		vector_text[state]->UploadNewData(vector_text[state]->GetImageData());
		SaveImage("shader_processing_" + std::to_string(outputMode));

		float aspectRatio = static_cast<float>(originalImage->GetWidth()) / originalImage->GetHeight();
		window->SetSize(static_cast<int>(600 * aspectRatio), 600);
	}
}

void NPR::FrameEnd()
{
	DrawCoordinatSystem();
}

void NPR::OnInputUpdate(float deltaTime, int mods)
{
}

void NPR::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_F || key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE)
	{
		OpenDialogue();
	}

	switch (key)
	{
	case GLFW_KEY_0:
		state = 0;
		break;
	case GLFW_KEY_1:
		state = 1;
		Sobel();
		break;
	case GLFW_KEY_2:
		state = 2;
		Dilation();
		break;
	case GLFW_KEY_3:
		state = 3;
		Combine();
		break;
	case GLFW_KEY_4:
		state = 4;
		ColorNarrowing();
		break;
	case GLFW_KEY_5:
		state = 5;
		Segmentation();
		break;
	default:
		break;
	}


	if (key == GLFW_KEY_S && mods & GLFW_MOD_CONTROL)
	{
		if (!gpuProcessing)
		{
			SaveImage("processCPU_" + std::to_string(outputMode));
		}
		else
		{
			saveScreenToImage = true;
		}
	}
}

void NPR::OnKeyRelease(int key, int mods)
{
}

void NPR::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}

void NPR::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void NPR::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void NPR::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void NPR::OnWindowResize(int width, int height)
{
}

void NPR::OpenDialogue()
{
}

void NPR::OnFileSelected(std::string fileName)
{
	if (fileName.size())
	{
		std::cout << fileName << std::endl;
		originalImage = TextureManager::LoadTexture(fileName.c_str(), nullptr, "image", true, true);
		processedImage = TextureManager::LoadTexture(fileName.c_str(), nullptr, "newImage", true, true);

		float aspectRatio = static_cast<float>(originalImage->GetWidth()) / originalImage->GetHeight();
		window->SetSize(static_cast<int>(600 * aspectRatio), 600);
	}
}

void NPR::SaveImage(std::string fileName)
{
	std::cout << "Saving image! ";
	processedImage->SaveToFile((fileName + ".png").c_str());
	std::cout << "[Done]" << std::endl;
}

void NPR::Sobel()
{
	const int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
	const int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = SobelOutput->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());

	for (int i = 0; i < imageSize.y; i++)
	{
		for (int j = 0; j < imageSize.x; j++)
		{
			float sum_X = 0;
			float sum_Y = 0;

			for (int y = -1; y <= 1; y++)
			{
				for (int x = -1; x <= 1; x++)
				{
					if ((y + i) >= 0 && (x + j) >= 0 && (y + i) < imageSize.y && (x + j) < imageSize.x)
					{
						int my_offet = channels * ((i + y) * imageSize.x + j + x);
						float px = 1.0f * (data[my_offet + 0] * 0.2f + data[my_offet + 1] * 0.71f + data[my_offet + 2] *
							0.07) / 255;
						sum_X += Gx[y + 1][x + 1] * px;
						sum_Y += Gy[y + 1][x + 1] * px;
					}
				}
			}

			float sum = std::sqrt(sum_Y * sum_Y + sum_X * sum_X);

			if (sum >= 0.5)
			{
				sum = 1;
			}
			else
			{
				sum = 0;
			}

			int offset = channels * (i * imageSize.x + j);

			// Reset save image data
			char value = sum * 255;
			memset(&newData[offset], value, 3);
		}
	}

	SobelOutput->UploadNewData(newData);
}

void NPR::Dilation()
{
	colorPixel B[3][3] = {
						{WHITE_PIXEL, WHITE_PIXEL, WHITE_PIXEL},
						{WHITE_PIXEL, WHITE_PIXEL, WHITE_PIXEL},
						{WHITE_PIXEL, WHITE_PIXEL, WHITE_PIXEL}
	};

	unsigned int channels = SobelOutput->GetNrChannels();
	unsigned char* data = SobelOutput->GetImageData();
	unsigned char* newData = DilationOutput->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(SobelOutput->GetWidth(), SobelOutput->GetHeight());
	for (int i = 0; i < imageSize.y; i++)
	{
		for (int j = 0; j < imageSize.x; j++)
		{
			//int offset = channels * (i * imageSize.x + j);
			unsigned char outColor = 0;

			for (int y = -1; y <= 1; y++)
			{
				for (int x = -1; x <= 1; x++)
				{
					if ((y + i) >= 0 && (x + j) >= 0 && (y + i) < imageSize.y && (x + j) < imageSize.x)
					{
						const int offset = channels * ((i + y) * imageSize.x + j + x);

						if (data[offset] == std::get<0>(B[x][y]) && data[offset + 1] == std::get<1>(B[x][y]) && data[offset + 2] == std::get<2>(B[x][y]))
						{
							outColor = 255;
						}else
						{
							outColor = 0;
						}
					}
				}
			}

			const int offset = channels * (i * imageSize.x + j);
			memset(&newData[offset], outColor, 3);
		}
	}

	DilationOutput->UploadNewData(newData);
}

void NPR::Combine()
{
	unsigned int channels = DilationOutput->GetNrChannels();
	unsigned char* data = DilationOutput->GetImageData();
	unsigned char* data1 = originalImage->GetImageData();
	unsigned char* newData = CombineOutput->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(CombineOutput->GetWidth(), CombineOutput->GetHeight());

	for (int i = 0; i < imageSize.y; i++)
	{
		for (int j = 0; j < imageSize.x; j++)
		{
			int offset = channels * (imageSize.x * i + j);

			if (data[offset] == 255 && data[offset + 1] == 255 && data[offset + 2] == 255)
			{
				newData[offset] = 255;
				newData[offset + 1] = 255;
				newData[offset + 2] = 255;
			}
			else
			{
				newData[offset] = data1[offset];
				newData[offset + 1] = data1[offset + 1];
				newData[offset + 2] = data1[offset + 2];
			}
		}
	}

	CombineOutput->UploadNewData(newData);
}

void NPR::ColorNarrowing()
{
	int med = 255 / number_interval;

	unsigned int channels = CombineOutput->GetNrChannels();
	unsigned char* data = CombineOutput->GetImageData();
	unsigned char* newData = ColorNarrowingOutput->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(CombineOutput->GetWidth(), CombineOutput->GetHeight());

	for (int i = 0; i < imageSize.y; i++)
	{
		for (int j = 0; j < imageSize.x; j++)
		{
			int offset = channels * (imageSize.x * i + j);
			newData[offset] = (data[offset] / med) * med + med / 2;
			newData[offset + 1] = (data[offset + 1] / med) * med + med / 2;
			newData[offset + 2] = (data[offset + 2] / med) * med + med / 2;

			if (newData[offset] >= 255)
			{
				newData[offset] = 255;
			}

			if (newData[offset + 1] >= 255)
			{
				newData[offset + 1] = 255;
			}

			if (newData[offset + 2] >= 255)
			{
				newData[offset + 2] = 255;
			}
		}
	}

	ColorNarrowingOutput->UploadNewData(newData);
}

void NPR::Segmentation()
{
	unsigned int channels = CombineOutput->GetNrChannels();
	unsigned char* data = CombineOutput->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(CombineOutput->GetWidth(), CombineOutput->GetHeight());
	std::map<int, Region> mapRegion;
	int** map_of_index = new int*[imageSize.y];

	for (int i = 0; i < imageSize.y; i++)
	{
		map_of_index[i] = new int[imageSize.x];

		memset(map_of_index[i], -1, imageSize.x);
	}

	// se calculeaza regiune si culoarea
	for (int i = 0; i < imageSize.y; i++)
	{
		for (int j = 0; j < imageSize.x; j++)
		{
			int offset = channels * (i * imageSize.x + j);
			colorPixel pixel(data[offset], data[offset + 1], data[offset + 2]);

			if (i == 0 && j == 0)
			{
				Region region(pixel);
				map_of_index[i][j] = region.index;
				mapRegion.insert({region.index, region});
				continue;
			}

			if (i == 0)
			{
				if (mapRegion[map_of_index[i][j - 1]].matchColor(pixel))
				{
					map_of_index[i][j] = map_of_index[i][j - 1];
					mapRegion[map_of_index[i][j - 1]].updateRegionColor(pixel);
				}
				else
				{
					Region region(pixel);
					map_of_index[i][j] = region.index;
					mapRegion.insert({region.index, region});
				}

				continue;
			}

			if (j == 0)
			{
				if (mapRegion[map_of_index[i - 1][j]].matchColor(pixel))
				{
					map_of_index[i][j] = map_of_index[i - 1][j];
					mapRegion[map_of_index[i - 1][j]].updateRegionColor(pixel);
				}
				else
				{
					Region region(pixel);
					map_of_index[i][j] = region.index;
					mapRegion.insert({region.index, region});
				}

				continue;
			}

			if (mapRegion[map_of_index[i - 1][j]].matchColor(pixel))
			{
				map_of_index[i][j] = map_of_index[i - 1][j];
				mapRegion[map_of_index[i - 1][j]].updateRegionColor(pixel);
			}
			else if (mapRegion[map_of_index[i - 1][j - 1]].matchColor(pixel))
			{
				map_of_index[i][j] = map_of_index[i - 1][j - 1];
				mapRegion[map_of_index[i - 1][j - 1]].updateRegionColor(pixel);
			}
			else if (mapRegion[map_of_index[i][j - 1]].matchColor(pixel))
			{
				map_of_index[i][j] = map_of_index[i][j - 1];
				mapRegion[map_of_index[i][j - 1]].updateRegionColor(pixel);
			}
			else
			{
				Region region(pixel);
				map_of_index[i][j] = region.index;
				mapRegion.insert({region.index, region});
			}
		}
	}


	for (int i = 0; i < imageSize.y; i++)
	{
		for (int j = 0; j < imageSize.x; j++)
		{
			int offset = channels * (i * imageSize.x + j);
			newData[offset] = std::get<0>(mapRegion[map_of_index[i][j]].colorRegion);
			newData[offset + 1] = std::get<1>(mapRegion[map_of_index[i][j]].colorRegion);
			newData[offset + 2] = std::get<2>(mapRegion[map_of_index[i][j]].colorRegion);
		}
	}

	SobelOutput->UploadNewData(newData);

	for (int i = 0; i < imageSize.y; i++)
	{
		delete[] map_of_index[i];
	}

	delete[] map_of_index;
}