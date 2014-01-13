textures/%METASHADER_NAME%.vertex
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map %TEXTURE_0_MAP%
		rgbGen vertex
		tcmod scale %TEXTURE_0_TCMOD%
	}
}
