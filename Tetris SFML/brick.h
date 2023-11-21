#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

class Brick : public Drawable
{
	int shape;
	bool movable;
	RectangleShape block;
	void draw(RenderTarget& target, RenderStates state) const override;

public:
	Brick(Vector2f position, Texture& texture, int shape=-1);
	~Brick() = default;

	Vector2f get_position();
	int get_shape();
	void set_shape(int shape);
	void set_new_position(Vector2f position);
	void go_right();
	void go_left();
	void go_down();
	void go_up();
	void stop();
	bool is_movable();
};

