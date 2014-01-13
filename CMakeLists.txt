#CMake file created by Jacker
# Remember to use -DCMAKE_BUILD_TYPE=Release or Debug
cmake_minimum_required(VERSION 2.8.3)
include(CheckIncludeFile)

project(EasyGen)

if(MSVC)
	add_definitions (-D_CRT_SECURE_NO_WARNINGS)
endif()

find_package(OpenGL REQUIRED)

FILE(GLOB EASYGEN_SRC
	"src/*.h"
	"src/*.cpp"
	"src/*.rc"
)

include_directories("libs/zlib")
include_directories("libs/lcms2-2.4/include")
include_directories("libs/jasper-1.900.1/src/libjasper/include")
include_directories("libs/jpeg")
include_directories("libs/libtiff")
include_directories("libs/libpng162")
include_directories("libs/libmng-1.0.10")
include_directories("libs/IL/include")
include_directories("libs/ILU/include")
include_directories("libs/ILUT/include")
include_directories("libs/include")

#################################################
# HANDLE JPEG
#################################################
check_include_file ( stddef.h HAVE_STDDEF_H )
check_include_file ( stdlib.h HAVE_STDLIB_H )

set(JPEG_LIB "${CMAKE_SOURCE_DIR}/libs/jpeg")

if(WIN32 AND NOT CYGWIN)
  set(TWO_FILE_COMMANDLINE true)
endif()

configure_file (${JPEG_LIB}/jconfig.h.cmake ${JPEG_LIB}/jconfig.h)

#include_directories(${CMAKE_CURRENT_BINARY_DIR})
FILE(GLOB JPEG_LIB_HEADERS 
  ${JPEG_LIB}/jerror.h
  ${JPEG_LIB}/jmorecfg.h
  ${JPEG_LIB}/jpeglib.h
  ${JPEG_LIB}/jconfig.h
)

FILE(GLOB JPEG_LIB_SRC
  ${JPEG_LIB}/jmemnobs.c
  ${JPEG_LIB}/jaricom.c
  ${JPEG_LIB}/jcapimin.c
  ${JPEG_LIB}/jcapistd.c
  ${JPEG_LIB}/jcarith.c
  ${JPEG_LIB}/jccoefct.c
  ${JPEG_LIB}/jccolor.c
  ${JPEG_LIB}/jcdctmgr.c
  ${JPEG_LIB}/jchuff.c
  ${JPEG_LIB}/jcinit.c
  ${JPEG_LIB}/jcmainct.c
  ${JPEG_LIB}/jcmarker.c
  ${JPEG_LIB}/jcmaster.c
  ${JPEG_LIB}/jcomapi.c
  ${JPEG_LIB}/jcparam.c
  ${JPEG_LIB}/jcprepct.c
  ${JPEG_LIB}/jcsample.c
  ${JPEG_LIB}/jctrans.c
  ${JPEG_LIB}/jdapimin.c
  ${JPEG_LIB}/jdapistd.c
  ${JPEG_LIB}/jdarith.c
  ${JPEG_LIB}/jdatadst.c
  ${JPEG_LIB}/jdatasrc.c
  ${JPEG_LIB}/jdcoefct.c
  ${JPEG_LIB}/jdcolor.c
  ${JPEG_LIB}/jddctmgr.c
  ${JPEG_LIB}/jdhuff.c
  ${JPEG_LIB}/jdinput.c
  ${JPEG_LIB}/jdmainct.c
  ${JPEG_LIB}/jdmarker.c
  ${JPEG_LIB}/jdmaster.c
  ${JPEG_LIB}/jdmerge.c
  ${JPEG_LIB}/jdpostct.c
  ${JPEG_LIB}/jdsample.c
  ${JPEG_LIB}/jdtrans.c
  ${JPEG_LIB}/jerror.c
  ${JPEG_LIB}/jfdctflt.c
  ${JPEG_LIB}/jfdctfst.c
  ${JPEG_LIB}/jfdctint.c
  ${JPEG_LIB}/jidctflt.c
  ${JPEG_LIB}/jidctfst.c
  ${JPEG_LIB}/jidctint.c
  ${JPEG_LIB}/jquant1.c
  ${JPEG_LIB}/jquant2.c
  ${JPEG_LIB}/jutils.c
  ${JPEG_LIB}/jmemmgr.c
  ${JPEG_LIB}/cderror.h
  ${JPEG_LIB}/cdjpeg.h
  ${JPEG_LIB}/jdct.h
  ${JPEG_LIB}/jinclude.h
  ${JPEG_LIB}/jmemsys.h
  ${JPEG_LIB}/jpegint.h
  ${JPEG_LIB}/jversion.h
  ${JPEG_LIB}/transupp.h
)

add_library (jpeg STATIC ${JPEG_LIB_SRC} ${JPEG_LIB_HEADERS})

#################################################
# HANDLE ZLIB
#################################################

set(ZLIB_SRC "${CMAKE_SOURCE_DIR}/libs/zlib")

FILE(GLOB ZLIB_SOURCES
	"${ZLIB_SRC}/*.h"
	"${ZLIB_SRC}/*.c"
)

add_library (zlib STATIC ${ZLIB_SOURCES})

#################################################
# HANDLE TIFF
#################################################

set(TIFF_LIB "${CMAKE_SOURCE_DIR}/libs/libtiff")

FILE(GLOB TIFF_SOURCE
	"${TIFF_LIB}/mkg3states.c"
	"${TIFF_LIB}/t4.h"
	"${TIFF_LIB}/tif_aux.c"
	"${TIFF_LIB}/tif_close.c"
	"${TIFF_LIB}/tif_codec.c"
	"${TIFF_LIB}/tif_color.c"
	"${TIFF_LIB}/tif_compress.c"
	"${TIFF_LIB}/tif_config.h"
	"${TIFF_LIB}/tif_config.vc.h"
	"${TIFF_LIB}/tif_config.wince.h"
	"${TIFF_LIB}/tif_dir.c"
	"${TIFF_LIB}/tif_dir.h"
	"${TIFF_LIB}/tif_dirinfo.c"
	"${TIFF_LIB}/tif_dirread.c"
	"${TIFF_LIB}/tif_dirwrite.c"
	"${TIFF_LIB}/tif_dumpmode.c"
	"${TIFF_LIB}/tif_error.c"
	"${TIFF_LIB}/tif_extension.c"
	"${TIFF_LIB}/tif_fax3.c"
	"${TIFF_LIB}/tif_fax3.h"
	"${TIFF_LIB}/tif_fax3sm.c"
	"${TIFF_LIB}/tif_flush.c"
	"${TIFF_LIB}/tif_getimage.c"
	"${TIFF_LIB}/tif_jbig.c"
	"${TIFF_LIB}/tif_jpeg.c"
	"${TIFF_LIB}/tif_luv.c"
	"${TIFF_LIB}/tif_lzm.c"
	"${TIFF_LIB}/tif_lzw.c"
	"${TIFF_LIB}/tif_next.c"
	"${TIFF_LIB}/tif_ojpeg.c"
	"${TIFF_LIB}/tif_open.c"
	"${TIFF_LIB}/tif_packbits.c"
	"${TIFF_LIB}/tif_pixarlog.c"
	"${TIFF_LIB}/tif_predict.c"
	"${TIFF_LIB}/tif_predict.h"
	"${TIFF_LIB}/tif_print.c"
	"${TIFF_LIB}/tif_read.c"
	"${TIFF_LIB}/tif_strip.c"
	"${TIFF_LIB}/tif_swab.c"
	"${TIFF_LIB}/tif_thunder.c"
	"${TIFF_LIB}/tif_tile.c"
	"${TIFF_LIB}/tif_version.c"
	"${TIFF_LIB}/tif_warning.c"
	"${TIFF_LIB}/tif_win32.c"
	"${TIFF_LIB}/tif_write.c"
	"${TIFF_LIB}/tif_zip.c"
	"${TIFF_LIB}/tif.h"
	"${TIFF_LIB}/tiffconfig.h"
	"${TIFF_LIB}/tiffio.h"
	"${TIFF_LIB}/tiffiop.h"
	"${TIFF_LIB}/tiffvers.h"
	"${TIFF_LIB}/uvcode.h"
)

add_library (tiff STATIC ${TIFF_SOURCE})

#################################################
# HANDLE PNG
#################################################

set(PNG_LIB "${CMAKE_SOURCE_DIR}/libs/libpng162")

FILE(GLOB PNG_SOURCES
	"${PNG_LIB}/*.h"
	"${PNG_LIB}/*.c"
)

add_library (png STATIC ${PNG_SOURCES})

#################################################
# HANDLE MNG
#################################################

set(MNG_LIB "${CMAKE_SOURCE_DIR}/libs/libmng-1.0.10")

FILE(GLOB MNG_SOURCES
	"${MNG_LIB}/*.h"
	"${MNG_LIB}/*.c"
)
add_library (mng STATIC ${MNG_SOURCES})
add_dependencies(mng zlib)

#################################################
# HANDLE LCMS
#################################################

set(LCMS_LIB "${CMAKE_SOURCE_DIR}/libs/lcms2-2.4/src")

FILE(GLOB LCMS_SOURCES
	"${LCMS_LIB}/*.h"
	"${LCMS_LIB}/*.c"
)
add_library (lcms STATIC ${LCMS_SOURCES})

#################################################
# HANDLE JASPER
#################################################

set(JASPER_LIB "${CMAKE_SOURCE_DIR}/libs/jasper-1.900.1/src/libjasper")

FILE(GLOB JASPER_SOURCES
	"${JASPER_LIB}/*/*.h"
	"${JASPER_LIB}/*/*.c"
)
add_library (jasper STATIC ${JASPER_SOURCES})
add_dependencies(jasper jpeg)

#################################################
# HANDLE IL
#################################################

set(CUSTOM_LIBS jpeg zlib tiff png mng lcms jasper)

set(IL_LIB "${CMAKE_SOURCE_DIR}/libs/IL/src")

FILE(GLOB IL_SOURCES
	"${IL_LIB}/*.h"
	"${IL_LIB}/*.c"
	"${IL_LIB}/il_exr.cpp"
	"${IL_LIB}/il_gif.cpp"
	"${IL_LIB}/il_nvidia.cpp"
	"${IL_LIB}/il_squish.cpp"
	"${IL_LIB}/il_utx.cpp"
)
add_library (illib STATIC ${IL_SOURCES})
add_dependencies(illib jpeg zlib tiff png mng lcms jasper)
target_link_libraries(illib ${CUSTOM_LIBS})

#################################################
# HANDLE ILU
#################################################

set(ILU_LIB "${CMAKE_SOURCE_DIR}/libs/ILU/src")

FILE(GLOB ILU_SOURCES
	"${ILU_LIB}/*.h"
	"${ILU_LIB}/*.c"
)
add_library (ilulib STATIC ${ILU_SOURCES})
add_dependencies(ilulib illib)
set_target_properties(ilulib
	PROPERTIES COMPILE_DEFINITIONS "IL_STATIC_LIB"
)

#################################################
# HANDLE ILUT
#################################################

set(ILUT_LIB "${CMAKE_SOURCE_DIR}/libs/ILUT/src")

FILE(GLOB ILUT_SOURCES
	"${ILUT_LIB}/*.h"
	"${ILUT_LIB}/*.c"
)
add_library (ilutlib STATIC ${ILUT_SOURCES})
add_dependencies(ilutlib illib)
set_target_properties(ilutlib
	PROPERTIES COMPILE_DEFINITIONS "IL_STATIC_LIB"
)

#################################################
# LETS BUILD EASYGEN
#################################################

include_directories(${OPENGL_INCLUDE_DIR})
set(DEVIL_LIBS illib ilulib ilutlib)
ADD_DEFINITIONS(-D_AFXDLL)
SET(CMAKE_MFC_FLAG 6)
add_executable(EasyGen WIN32 ${EASYGEN_SRC})
add_dependencies(EasyGen illib ilulib ilutlib jpeg zlib tiff png mng lcms jasper)
target_link_libraries(EasyGen ${DEVIL_LIBS} ${CUSTOM_LIBS} ${OPENGL_LIBRARIES})