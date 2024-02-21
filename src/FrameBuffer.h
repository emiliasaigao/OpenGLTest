#pragma once

#include"Texture.h"

class FrameBuffer {
public:
	unsigned int ID;
	unsigned int textureID;
	unsigned int renderBufferID;
	unsigned int width, height;
	bool isDepthBuff;

	FrameBuffer(const unsigned int& w, const unsigned int& h, bool isDepthBuffer = false)
		: width(w),height(h),isDepthBuff(isDepthBuffer) {
		setUpFrameBuffer();
	}

private:
	void setUpFrameBuffer();
};