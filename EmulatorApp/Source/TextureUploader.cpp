#include "TextureUploader.h"

template <>
void TextureUploader<glm::vec4>::TexImage2D() {
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA32F,
		texture.resolution.x, texture.resolution.y,
		0, GL_RGBA, GL_FLOAT,
		&texture.pixels[0]
	);
}

template <>
void TextureUploader<glm::i8vec3>::TexImage2D() {
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB32F,
		texture.resolution.x, texture.resolution.y,
		0, GL_RGB, GL_UNSIGNED_BYTE,
		&texture.pixels[0]
	);
}