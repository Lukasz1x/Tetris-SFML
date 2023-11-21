#include "Block_Shape.h"

Block_Shape::Block_Shape(Vector2f array[4], Texture& texture_png)
{
	for (int i = 0; i < 4; i++)
	{
		position[i] = array[i];
	}
	texture = texture_png;
}

Vector2f Block_Shape::get_position(int i)
{
	return position[i];
}

Texture& Block_Shape::get_texture()
{
	return texture;
}