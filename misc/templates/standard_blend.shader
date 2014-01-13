; this is a template comment and will not be saved into the shader
textures/%METASHADER_NAME%_%I%to%J%
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map %TEXTURE_I_MAP%
		rgbGen vertex
		alphaGen vertex
		tcmod scale %TEXTURE_0_TCMOD%
	}
	{
		map %TEXTURE_J_MAP%
		rgbGen vertex
		alphaGen vertex
		tcmod scale %TEXTURE_0_TCMOD%
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}