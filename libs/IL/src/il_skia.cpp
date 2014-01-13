///////////////////////////////////////////////////////////////////////////////
// Skia Wrapper, C style
// Written by Björn Ganster in 2012
///////////////////////////////////////////////////////////////////////////////

#include "Windows.h"
#include "D:\\devel\\devil-1.7.8 - 2012-04-19\\include\\IL\\il.h"
#include "core\SkStream.h"
#include "images\SkImageDecoder.h"
#include "core\SkStream.h"
#include "core\SkTemplates.h"

// Load an image
// buf: Pointer to input image data
// bufSize: Number of bytes in buf
extern "C" {
ILboolean __stdcall skiaLoadImage(void* buf, size_t bufSize)
{
    SkMemoryStream stream(buf, bufSize, false);
    /*if (!stream.isValid()) {
        SkDebugf("ERROR: failed to build stream\n");
        return false;
    }*/

    SkImageDecoder* codec = SkImageDecoder::Factory(&stream);
    if (NULL == codec) {
        SkDebugf("ERROR: no codec found for file\n");
        return false;
    }

    SkAutoTDelete<SkImageDecoder> ad(codec);

    stream.rewind();
    int width, height;
    SkBitmap* bitmap = new SkBitmap;
    if (!codec->decode(&stream, bitmap, SkBitmap::kARGB_8888_Config,
                       SkImageDecoder::kDecodePixels_Mode)) {
        //SkDebugf("ERROR: codec failed for <%s>\n", srcPath);
        return false;
    }

    bitmap->setConfig(SkBitmap::kARGB_8888_Config, width, height);
    unsigned char* rgb = new unsigned char[width*height*3];
    unsigned char* argb = (unsigned char*) bitmap->getAddr(0,0);

    // Need to copy the buffers manually, the formats do not fit
    for (int y = 0; y < height; ++y) {
       for (int x = 0; x < width; ++x) {
          int i = y*width+x;
          int j = 4*i;
          i *= 3;
          rgb[i] = argb[j];
          rgb[i+1] = argb[j+1];
          rgb[i+2] = argb[j+2];
       }
    }

    ilSetPixels(0, 0, 0, // x, y, z offsets
                width, height, 0, IL_RGB, IL_UNSIGNED_BYTE, rgb);
    delete codec;
    delete bitmap;

    return true;
}
}