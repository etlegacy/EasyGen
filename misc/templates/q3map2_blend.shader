; this template is for building a shader that will work with
; q3map2.exe, an advanced compiler version programmed by ydnar 
; based upon original q3map.exe source code
; http://shaderlab.com/q3map2/
;
textures/%METASHADER_NAME%_%I%to%J%
{
	q3map_lightmapsamplesize 64
	q3map_lightmapaxis z
	q3map_texturesize %TEXTURE_0_SIZEX% %TEXTURE_0_SIZEY%
	q3map_tcGen ivector ( %TEXTURE_0_SGEN% ) ( %TEXTURE_0_TGEN% )
	
	{
		map %TEXTURE_I_MAP%
	}
	{
		map %TEXTURE_J_MAP%
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		tcGen lightmap
	}
}