#include "stdafx.h"
#include "GifProcesser.h"
using namespace Gdiplus;

GifProcesser::GifProcesser()
{
	_image = NULL;
	_guids = NULL;
	_items = NULL;
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &input, NULL);
}

GifProcesser::~GifProcesser()
{
	if (_items != NULL)
	{
		free(_items);
		_items = NULL;
	}
	if (_guids != NULL)
	{
		delete[] _guids;
		_guids = NULL;
	}
	if (_image != NULL)
	{
		delete _image;
		_image = NULL;
	}
	GdiplusShutdown(m_gdiplusToken);
}

bool GifProcesser::Create(IStream* stream)
{
	_image = Image::FromStream(stream);
	if (_image == NULL)return false;

	_frameDimensionsCount = _image->GetFrameDimensionsCount();
	_width = _image->GetWidth();
	_height = _image->GetHeight();
	_guids = new GUID[_frameDimensionsCount];

	_image->GetFrameDimensionsList(_guids, _frameDimensionsCount);
	_frameCount = _image->GetFrameCount(&_guids[0]);
	int sz = _image->GetPropertyItemSize(PropertyTagFrameDelay);
	_items = (PropertyItem *)malloc(sz);
	_image->GetPropertyItem(PropertyTagFrameDelay, sz, _items);
	return GetEncoderClsid();
}

void GifProcesser::GetFrame(IStream* stream, INT idx)
{
	Bitmap bmp(_width, _height, PixelFormat32bppARGB);
	Graphics g(&bmp);

	_image->SelectActiveFrame(_guids, 10);
	g.DrawImage(_image, 0, 0);

	LARGE_INTEGER lMove;
	ULARGE_INTEGER lLast;
	lMove.QuadPart = 0;
	lLast.QuadPart = 0;
	stream->Seek(lMove, STREAM_SEEK_SET, &lLast);
	bmp.Save(stream, &_clsid);
}

bool GifProcesser::GetEncoderClsid()
{
	UINT  num = 0;
	UINT  size = 0;
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)	return false;

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return false; 

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, _T("image/png")) == 0)
		{
			_clsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return true;
		}
	}
	free(pImageCodecInfo);
	return false;
}
