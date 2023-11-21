#include "Brick.h"


Brick::Brick(Vector2f position, Texture& texture, int shape)
{
	movable = true;

	block.setPosition(position);
	block.setSize(Vector2f(32, 32));
	block.setTexture(&texture);
	this->shape = shape;
}

Vector2f Brick::get_position()
{
	return block.getPosition();
}

void Brick::draw(RenderTarget& target, RenderStates state) const
{
	target.draw(this->block);
}

void Brick::go_right()
{
	block.setPosition(block.getPosition() + Vector2f(32, 0));
}

void Brick::go_left()
{
	block.setPosition(block.getPosition() + Vector2f(-32, 0));
}

void Brick::go_down()
{
	block.setPosition(block.getPosition() + Vector2f(0, 32));
}

void Brick::go_up()
{
	block.setPosition(block.getPosition() + Vector2f(0, -74));
}

void Brick::stop()
{
	movable = false;
}

bool Brick::is_movable()
{
	return movable;
}

void Brick::set_shape(int shape)
{
	this->shape = shape;
}

int Brick::get_shape()
{
	return shape;
}

void Brick::set_new_position(Vector2f position)
{
	block.setPosition(position);
}