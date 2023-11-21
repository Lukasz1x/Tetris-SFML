#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

class Block_Shape
{
	Vector2f position[4];
	Texture texture;

public:
	Block_Shape(Vector2f array[4], Texture &texture_png);
	~Block_Shape() = default;

	Vector2f get_position(int i);
	Texture& get_texture();
};

