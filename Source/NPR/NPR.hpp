#ifndef NPR_HPP
#define NPR_HPP

#include <Component/SimpleScene.h>

#define WIN_API_FILE_BROWSING

#include "Region.hpp"

class NPR : public SimpleScene
{
public:
	NPR();
	~NPR();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

	void OpenDialogue();
	void OnFileSelected(std::string fileName);

	void SaveImage(std::string fileName);

	Texture2D *originalImage;
	Texture2D *processedImage;

	Texture2D *SobelOutput;
	Texture2D *DilationOutput;
	Texture2D *CombineOutput;
	Texture2D *ColorNarrowingOutput;
	Texture2D *SegmentationOutput;
	
	bool flip;
	int outputMode;
	bool gpuProcessing;
	bool saveScreenToImage;

	int number_interval;
	
	FrameBuffer* processed;

	void Sobel();
	void Dilation();
	void Combine();
	void ColorNarrowing();
	void Segmentation();
	int state;
};

#endif // NPR_HPP
