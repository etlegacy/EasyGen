textures/%METASHADER_NAME%_%I%to%J%
{
	q3map_baseshader textures/%METASHADER_NAME%_base
	qer_editorimage %TEXTURE_I_MAP%
	surfaceparm landmine
	surfaceparm grasssteps
	{
		map %TEXTURE_I_MAP%
	}
	{
		map %TEXTURE_J_MAP%
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}