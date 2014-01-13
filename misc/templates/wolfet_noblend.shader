textures/%METASHADER_NAME%_%I%
{
	q3map_baseshader textures/%METASHADER_NAME%_base
	qer_editorimage %TEXTURE_I_MAP%
	surfaceparm landmine
	surfaceparm grasssteps
	{
		map %TEXTURE_I_MAP%
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}