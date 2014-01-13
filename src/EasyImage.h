#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

typedef unsigned long BTCOLORREF;

#define BTRGB(r, g, b)         ((unsigned long)(((unsigned char)(r) | ((unsigned short)((unsigned char)(g)) << 8)) | (((unsigned long)(unsigned char)(b)) << 16)))
#define BTRGBA(r, g, b, a)      ((unsigned long)(((unsigned char)(r) | ((unsigned short)((unsigned char)(g)) << 8)) | (((unsigned long)(unsigned char)(b)) << 16) | (((unsigned long)(a)) << 24)))

#define BTGetRValue(rgba)    ((unsigned char)(rgba))
#define BTGetGValue(rgba)    ((unsigned char)(((unsigned short)(rgba)) >> 8))
#define BTGetBValue(rgba)    ((unsigned char)((rgba) >> 16))
#define BTGetAValue(rgba)    ((unsigned char)((rgba) >> 24))

#define COLINDEX(r, g, b)      ((unsigned int)(((unsigned char)(r) | ((unsigned short)((unsigned char)(g)) << 8)) | (((unsigned long)(unsigned char)(b)) << 16)))
#define COLRValue(rgba)      ((unsigned char)(rgba))
#define COLGValue(rgba)      ((unsigned char)(((unsigned short)(rgba)) >> 8))
#define COLBValue(rgba)      ((unsigned char)((rgba) >> 16))

enum BTFileType
{
	BTFILETYPE_UNKNOWN = -1,
	BTFILETYPE_BMP     = 0,
	BTFILETYPE_BMP_NO_CRYP,
	BTFILETYPE_PCX,
	BTFILETYPE_PNG,
	BTFILETYPE_PPM,
	BTFILETYPE_PGM,
	BTFILETYPE_PBM,
	BTFILETYPE_JPEG,
	BTFILETYPE_TGA,
	BTFILETYPE_TIFF,
	BTFILETYPE_GIF,
	BTFILETYPE_ILBM,
	BTFILETYPE_RAS,
	BTFILETYPE_EPS,
	BTFILETYPE_MNG,
	BTFILETYPE_WMF,
	BTFILETYPE_EMF,
	BTFILETYPE_APM,
	BTFILETYPE_PSP,
	BTFILETYPE_JP2,
	BTFILETYPE_CUT,
	BTFILETYPE_EXTERN
};

//New image stuff
class EasyImage
{
public:
	EasyImage();
	EasyImage(const char *filename);
	EasyImage(unsigned int imagesrc);
	~EasyImage();

	//Load from file
	bool LoadImageFromFile(const char *filename = NULL);

	//Create an empty image
	bool Create(int nWidth, int nHeight, int nBytesPerPixel = 1);

	//Resize the image
	bool Resize(unsigned int width, unsigned int height, unsigned int filter = ILU_BILINEAR);

	//Strech the image
	void DrawImage(HDC hdc, int xDest, int yDest, int nDestWidth, int nDestHeight);

	//Change image format
	bool Convert(unsigned int format);

	//Save the image
	bool Save(const char *filename);
	bool Save(const char *filename, unsigned int filetype);

	void Posterize(int levels = 7);
	void Quantize(int BitsPerPixel = 8);

	int GetIndexFromColor(BTCOLORREF col);
	int GetIndexFromPixel(int x, int y);

	BTCOLORREF GetColorFromIndex(int index);
	BTCOLORREF GetColorFromPixel(int x, int y);

	void SetColorForIndex(int index, BTCOLORREF color);
	void SetColorForPixel(int x, int y, BTCOLORREF color);

	int GetWidth() const;
	int GetHeight() const;

	void Flip();

	bool ConvertTo24BPP();
	bool ConvertTo32BPP();

	//Size params
	int GetBitsPerPixel() const;
	unsigned char *GetBits() const;
	long GetImageSize() const;

	EasyImage CloneImage();

	//DeleteObject on the old code
	void ClearData();
private:
	ILuint image;
	int GetImageType(const char *filename);
};