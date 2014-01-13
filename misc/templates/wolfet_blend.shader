; this is a template comment and will not be saved into the shader
textures/%METASHADER_NAME%_%I%to%J%
{
	q3map_baseshader textures/%METASHADER_NAME%_base
	{
		map %TEXTURE_I_MAP%
		tcMod scale %TEXTURE_0_TCMOD%
	}
	{
		map %TEXTURE_J_MAP%
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcMod scale %TEXTURE_0_TCMOD%
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}