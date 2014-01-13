To add a shader template named "qwerty" you must:

* insert the word "qwerty" into templatelist.txt, without ""

* create three files:

  qwerty_noblend.shader,	used for triangles where there is one texture.
  qwerty_blend.shader,		used for triangles where two textures meet.
  qwerty_vertexremap.shader,	used to remap all shaders for vertex light.

* Inside some of the files above you can use these identifiers,
  ENCLOSED BY % % (for example %METASHADER_NAME%):

  The table below shows where each identifier can be used:

	-------------------------------------------------------------------------------------------------
				qwerty_noblend.shader	qwerty_blend.shader	qwerty_vertexremap.shader
	-------------------------------------------------------------------------------------------------
	METASHADER_NAME			OK			OK			OK
	I				OK			OK
	J							OK
	-------------------------------------------------------------------------------------------------
	TEXTURE_I_MAP			OK			OK
	TEXTURE_I_SIZEX			OK			OK
	TEXTURE_I_SIZEY			OK			OK
	-------------------------------------------------------------------------------------------------
	TEXTURE_J_MAP						OK
	TEXTURE_J_SIZEX						OK
	TEXTURE_J_SIZEY						OK
	-------------------------------------------------------------------------------------------------
	TEXTURE_0_MAP			OK			OK			OK
	TEXTURE_0_SIZEX			OK			OK			OK
	TEXTURE_0_SIZEY			OK			OK			OK
	TEXTURE_0_SGEN			OK			OK			OK
	TEXTURE_0_TGEN			OK			OK			OK
	TEXTURE_0_TCMOD			OK			OK			OK
	------------------------------------------------------------------------------------------------
	MESH_WIDTHX			OK			OK			OK
	MESH_WIDTHY			OK			OK			OK
	MESH_DIVWIDTHX			OK			OK			OK
	MESH_DIVWIDTHY			OK			OK			OK
	------------------------------------------------------------------------------------------------

 No, there is nothing wrong with the table above. If you are wondering why TEXTURE_I and TEXTURE_J do not
 have a SGEN, TGEN and TCMOD fields the answer is: EasyGen currently does not support per layer scaling, that is
 only one factor is acquired to scale the terrain and the data is calculated basing on the texture at
 index 0 of the alphamap palette. This may be a good reason to use texture of the same size while designing
 terrain maps with EasyGen. According to this, it should make no sense keeping TEXTURE_I / J SIZEX / Y, but
 that didnt cost too much so i did it.

 I dont know if MESH_* identifiers could be useful or not... btw I have added them.