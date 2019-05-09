#pragma once
#include <gdiplus.h>
//////////////////////////////////////////////////////////////////////////
//这个类使用GDIPLUS 来加载GIF 然后可以将其中的帧提取出来

class GifProcesser
{
public:
	GifProcesser();
	~GifProcesser();
	bool Create(IStream* stream);
	void GetFrame(IStream* stream, INT idx);
	UINT GetFrameCount(){ return _frameCount; }
protected:
	ULONG_PTR m_gdiplusToken;
private:
	bool GetEncoderClsid();
private:
	Gdiplus::Image* _image;
	Gdiplus::PropertyItem* _items; 
	GUID* _guids;
	CLSID _clsid;
	UINT _frameDimensionsCount;
	UINT _width;
	UINT _height;
	UINT _frameCount;
};

