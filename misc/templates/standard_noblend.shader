; this is a template comment and will not be saved into the shader
textures/%METASHADER_NAME%_%I%
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map %TEXTURE_I_MAP%
		rgbGen vertex
		tcmod scale %TEXTURE_0_TCMOD%
	}
}