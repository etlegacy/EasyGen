textures/%METASHADER_NAME%_%I%
{
	q3map_baseshader textures/%METASHADER_NAME%_base
	surfaceparm landmine
	surfaceparm gravelsteps
	{
		map %TEXTURE_I_MAP%
		tcMod scale %TEXTURE_0_TCMOD%
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}