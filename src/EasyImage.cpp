#include "EasyGen.h"

EasyImage::EasyImage()
{
	//haswin = false;
	ilGenImages(1, &image);
}

EasyImage::EasyImage(const char *filename)
{
	ilGenImages(1, &image);
	LoadImageFromFile(filename);
}

EasyImage::EasyImage(unsigned int imagesrc)
{
	image = imagesrc;
}

EasyImage::~EasyImage()
{

}

int EasyImage::GetImageType(const char *filename)
{
	CString m_sFilename(filename);
	CString ext = m_sFilename.Right(3);
	ext.MakeLower();

	if (ext == "bmp")
	{
		return IL_BMP;
	}
	else if (ext == "tga")
	{
		return IL_TGA;
	}
	else if (ext == "jpg" || ext == "peg")
	{
		return IL_JPG;
	}
	else if (ext == "pcx")
	{
		return IL_PCX;
	}
	else
	{
		return IL_TYPE_UNKNOWN;
	}
}

bool EasyImage::LoadImageFromFile(const char *filename /*= NULL*/)
{
	ilBindImage(image);

	int filetype = GetImageType(filename);
	if (filetype == IL_TYPE_UNKNOWN)
	{
		return false;
	}

	//We do the loading like this so that we can simply move to PhysicsFS
	FILE *File = fopen(filename, "rb");
	fseek(File, 0, SEEK_END);
	ILuint Size = ftell(File);

	ILubyte *Lump = (ILubyte *)malloc(Size);
	fseek(File, 0, SEEK_SET);
	fread(Lump, 1, Size, File);
	fclose(File);

	ILboolean loaded = ilLoadL(filetype, Lump, Size);
	free(Lump);

	return bool(loaded);
}

bool EasyImage::Create(int nWidth, int nHeight, int nBytesPerPixel /*= 1*/)
{
	ilBindImage(image);

	int format = 0;

	switch (nBytesPerPixel)
	{
	case 1:
		format = IL_LUMINANCE;
		break;
	case 3:
		format = IL_RGB;
		break;
	case 4:
		format = IL_RGBA;
		break;
	default:
		return false;
	}

	bool ret = bool(ilTexImage(nWidth, nHeight, 1, nBytesPerPixel, format, IL_UNSIGNED_BYTE, NULL));

	return ret;
}

bool EasyImage::Resize(unsigned int width, unsigned int height, unsigned int filter /*= ILU_BILINEAR*/)
{
	ilBindImage(image);
	iluImageParameter(ILU_FILTER, ILU_BILINEAR);
	ILboolean scaled = iluScale(width, height, 0);

	return bool(scaled);
}

void EasyImage::DrawImage(HDC hdc, int xDest, int yDest, int nDestWidth, int nDestHeight)
{
	/*
	RECT rect;
	rect.left = xDest;
	rect.right = xDest + nDestWidth;
	rect.top = yDest;
	rect.bottom = yDest + nDestHeight;
	FillRect(hdc,&rect,CreatePatternBrush(ilutConvertToHBitmap(hdc)));
	FillRect(hdc,&rect,(HBRUSH)(RGB(40,151,151)));
	*/

	ilBindImage(image);
	HBITMAP winimage = ilutConvertToHBitmap(hdc);

	HDC     hdcMem;
	HGDIOBJ oldBitmap;

	hdcMem    = CreateCompatibleDC(hdc);
	oldBitmap = SelectObject(hdcMem, winimage);


	//BitBlt(hdc, xDest, yDest, nDestWidth, nDestHeight, hdcMem, 0, 0, dwRop);
	SetStretchBltMode(hdc, COLORONCOLOR);
	StretchBlt(hdc, xDest, yDest, nDestWidth, nDestHeight, hdcMem, 0, 0, GetWidth(), GetHeight(), SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);
	DeleteObject(winimage);
}

bool EasyImage::Convert(unsigned int format)
{
	ilBindImage(image);
	return bool(ilConvertImage(format, IL_UNSIGNED_BYTE));
}

bool EasyImage::Save(const char *filename)
{
	ilBindImage(image);

	return bool(ilSaveImage(filename));
}

bool EasyImage::Save(const char *filename, unsigned int filetype)
{
	ilBindImage(image);

	bool result = false;

	switch (filetype)
	{
	case BTFILETYPE_BMP_NO_CRYP:
		result = bool(ilSave(IL_BMP, filename));
		break;
	case BTFILETYPE_PCX:
		result = bool(ilSave(IL_PCX, filename));
		break;
	default:
		break;
	}

	return result;
}

void EasyImage::Posterize(int levels /*= 7*/)
{
	ilBindImage(image);

	ILubyte *bytes = ilGetData();
	int     by     = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	int     width  = ilGetInteger(IL_IMAGE_WIDTH);
	int     height = ilGetInteger(IL_IMAGE_HEIGHT);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			for (int curbyte = 0; curbyte < by && curbyte < 3; curbyte++)
			{
				bytes[(i * width + j) * by + curbyte] = (bytes[(i * width + j) * by + curbyte] * levels)  / levels;
			}
		}
	}
}

void EasyImage::Quantize(int BitsPerPixel /*= 8*/)
{
	ilBindImage(image);
	//FIXME: implement this
}

int EasyImage::GetIndexFromColor(BTCOLORREF col)
{
	ilBindImage(image);
	return COLINDEX(BTGetRValue(col), BTGetGValue(col), BTGetBValue(col));
}

int EasyImage::GetIndexFromPixel(int x, int y)
{
	ilBindImage(image);

	ILubyte *bytes = ilGetData();
	int     b      = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	int     width  = ilGetInteger(IL_IMAGE_WIDTH);

	if (b >= 3)
	{
		return COLINDEX(bytes[(x * width + y) * b + 0], bytes[(x * width + y) * b + 1], bytes[(x * width + y) * b + 2]);
	}
	else if (b == 1)
	{
		return COLINDEX(bytes[(x * width + y) * b + 0], bytes[(x * width + y) * b + 0], bytes[(x * width + y) * b + 0]);
	}
	else
	{
		return 0;
	}
}

BTCOLORREF EasyImage::GetColorFromIndex(int index)
{
	ilBindImage(image);
	return BTRGB(COLRValue(index), COLGValue(index), COLBValue(index));
}

BTCOLORREF EasyImage::GetColorFromPixel(int x, int y)
{
	ilBindImage(image);

	ILubyte *bytes = ilGetData();
	int     b      = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	int     width  = ilGetInteger(IL_IMAGE_WIDTH);

	if (b == 4)
	{
		return BTRGBA(bytes[(x * width + y) * b + 0], bytes[(x * width + y) * b + 1], bytes[(x * width + y) * b + 2], bytes[(x * width + y) * b + 3]);
	}
	else if (b == 3)
	{
		return BTRGB(bytes[(x * width + y) * b + 0], bytes[(x * width + y) * b + 1], bytes[(x * width + y) * b + 2]);
	}
	else
	{
		return BTRGB(bytes[(x * width + y) * b + 0], bytes[(x * width + y) * b + 0], bytes[(x * width + y) * b + 0]);
	}
}

void EasyImage::SetColorForIndex(int index, BTCOLORREF color)
{
	ilBindImage(image);

	byte r = BTGetRValue(color);
	byte g = BTGetGValue(color);
	byte b = BTGetBValue(color);
	byte a = BTGetAValue(color);

	ILubyte *bytes = ilGetData();
	int     by     = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	int     width  = ilGetInteger(IL_IMAGE_WIDTH);
	int     height = ilGetInteger(IL_IMAGE_HEIGHT);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (index == COLINDEX(bytes[(i * width + j) * by + 0], bytes[(i * width + j) * by + 1], bytes[(i * width + j) * by + 2]))
			{
				switch (by)
				{
				case 4:
					bytes[(i * width + j) * by + 0] = r;
					bytes[(i * width + j) * by + 1] = b;
					bytes[(i * width + j) * by + 2] = g;
					bytes[(i * width + j) * by + 3] = a;
					break;
				case 3:
					bytes[(i * width + j) * by + 0] = r;
					bytes[(i * width + j) * by + 1] = b;
					bytes[(i * width + j) * by + 2] = g;
					break;
				case 2:
					bytes[(i * width + j) * by + 0] = r;
					bytes[(i * width + j) * by + 1] = b;
					break;
				default:
					bytes[(i * width + j) * by + 0] = r;
					break;
				}
			}
		}
	}
}

void EasyImage::SetColorForPixel(int x, int y, BTCOLORREF color)
{
	ilBindImage(image);

	byte r = BTGetRValue(color);
	byte g = BTGetGValue(color);
	byte b = BTGetBValue(color);
	byte a = BTGetAValue(color);

	ILubyte *bytes = ilGetData();
	int     by     = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	int     width  = ilGetInteger(IL_IMAGE_WIDTH);

	switch (by)
	{
	case 4:
		bytes[(x * width + y) * by + 0] = r;
		bytes[(x * width + y) * by + 1] = b;
		bytes[(x * width + y) * by + 2] = g;
		bytes[(x * width + y) * by + 3] = a;
		break;
	case 3:
		bytes[(x * width + y) * by + 0] = r;
		bytes[(x * width + y) * by + 1] = b;
		bytes[(x * width + y) * by + 2] = g;
		break;
	case 2:
		bytes[(x * width + y) * by + 0] = r;
		bytes[(x * width + y) * by + 1] = b;
		break;
	default:
		bytes[(x * width + y) * by + 0] = r;
		break;
	}
}

int EasyImage::GetWidth() const
{
	ilBindImage(image);
	return ilGetInteger(IL_IMAGE_WIDTH);
}

int EasyImage::GetHeight() const
{
	ilBindImage(image);
	return ilGetInteger(IL_IMAGE_HEIGHT);
}

void EasyImage::Flip()
{
	ilBindImage(image);
	iluFlipImage();
}

bool EasyImage::ConvertTo24BPP()
{
	ilBindImage(image);

	ILboolean con = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

	if (con)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool EasyImage::ConvertTo32BPP()
{
	ilBindImage(image);

	ILboolean con = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	if (con)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int EasyImage::GetBitsPerPixel() const
{
	ilBindImage(image);
	return ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);
}

unsigned char *EasyImage::GetBits() const
{
	ilBindImage(image);
	return ilGetData();
}

long EasyImage::GetImageSize() const
{
	ilBindImage(image);

	long res = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	res *= GetWidth();
	res *= GetHeight();

	return res;
}

EasyImage EasyImage::CloneImage()
{
	ILuint newimage = ilGenImage();
	ilBindImage(newimage);

	ilCopyImage(image);

	EasyImage newimageclass(newimage);

	return newimageclass;
}

void EasyImage::ClearData()
{
	ilBindImage(image);
	ilDeleteImages(1, &image);
}
