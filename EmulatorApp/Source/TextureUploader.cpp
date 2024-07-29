#include "TextureUploader.h"

template <>
void TextureUploader<Color>::TexImage2D() {
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA32F,
		texture.width, texture.height,
		0, GL_RGB, GL_UNSIGNED_BYTE,
		&texture.pixels[0]
	);
}
