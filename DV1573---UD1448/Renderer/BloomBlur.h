#pragma once
#include <Pch/Pch.h>

class BloomBlur
{
private:
	unsigned int hdrFBO;
	unsigned int colorBuffers[2];

	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffer[2];

	bool horizontal = true;
	bool firstIteration = true;
	int amount = 8;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

public:
	BloomBlur();
	~BloomBlur();

	int createHdrFBO();
	int createPingPingFBO();

	void bindHdrFBO();
	void bindPingPongFBO(bool horizontal);

	unsigned int getColorAttachment(int num); 
	unsigned int getPingPongBuffer(int num);

	void blurIteration(int num);
	int getAmount();
	bool getHorizontal();

	void sendTextureLastPass();

	void renderQuad();
};