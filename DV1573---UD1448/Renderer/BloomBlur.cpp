#include <Pch/Pch.h>
#include "Renderer/BloomBlur.h"

BloomBlur::BloomBlur()
{
}

BloomBlur::~BloomBlur()
{
	glDeleteFramebuffers(1, &hdrFBO);
	glDeleteTextures(2, colorBuffers);
	for (int i = 0; i < 2; i++)
	{
		glDeleteFramebuffers(1, &pingpongFBO[i]);
		glDeleteTextures(1, pingpongBuffer);
	}

}

int BloomBlur::createHdrFBO()
{
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	glGenTextures(2, colorBuffers);

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB16F,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			0,
			GL_RGB,
			GL_FLOAT,
			NULL
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + i,
			GL_TEXTURE_2D,
			colorBuffers[i],
			0
		);
	}

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	unsigned int attachment[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachment);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return 0;
}

int BloomBlur::createPingPingFBO()
{

	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB16F,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			0,
			GL_RGB,
			GL_FLOAT,
			NULL
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			pingpongBuffer[i],
			0
		);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return 0;
}

void BloomBlur::bindHdrFBO()
{
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void BloomBlur::bindPingPongFBO(bool horizontal)
{
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);

	glEnable(GL_DEPTH_TEST);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
}

unsigned int BloomBlur::getColorAttachment(int num)
{
	return colorBuffers[num];
}

unsigned int BloomBlur::getPingPongBuffer(int num)
{
	return pingpongBuffer[num];
}


void BloomBlur::blurIteration(int num)
{
	glActiveTexture(GL_TEXTURE0);
	bindPingPongFBO(horizontal);
	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
	if (num == 0)
		glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
	if (num == 1)
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
	//glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : pingpongBuffer[!horizontal]);

	renderQuad();
	horizontal = !horizontal;
	if (firstIteration)
		firstIteration = false;

}

int BloomBlur::getAmount()
{
	return amount;
}

bool BloomBlur::getHorizontal()
{
	return horizontal;
}

void BloomBlur::sendTextureLastPass()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);

}

void BloomBlur::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glStencilMask(~0);
	glDisable(GL_SCISSOR_TEST);
	glClearStencil(0x0);
	glClear(GL_STENCIL_BUFFER_BIT);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

}