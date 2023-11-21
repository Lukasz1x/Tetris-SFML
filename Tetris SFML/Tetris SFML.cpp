#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "Brick.h"
#include "Block_Shape.h"

using namespace sf;
using namespace std;

bool end_game = 0;
chrono::milliseconds interval(500);
const int width = 1280; 
const int height = 720;
const int size = 32; 
const Vector2f px_start(481, 40);
const Vector2f px_stop = px_start + Vector2f(9 * 32, 19 * 32);
const Vector2f move_left(-32, 0);
const Vector2f move_right(32, 0);
const Vector2f move_down(0, 32);
const Vector2f px_next(159, 283);

void endgame()
{
    end_game = 1;
    RenderWindow gameover(VideoMode(320, 180), "Game Over!");
    RectangleShape background;
    Texture background_texture;
    background_texture.loadFromFile("assets/gameover.png");
    background.setSize(Vector2f(320, 180));
    background.setTexture(&background_texture);
    background.setPosition(0, 0);
    while (gameover.isOpen())
    {
        Event event;
        while (gameover.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                gameover.close();
            }
        }
        gameover.draw(background);
        gameover.display();
    }
}

void load(vector<Block_Shape>& shapes)
{
    string filename = "assets/shapes.txt";
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }
    for (int i = 0; i < 7; i++)
    {
        Vector2f tab[4];
        string textureFilename;
        Texture block_texture;

        for (int j = 0; j < 4; j++)
        {
            inputFile >> tab[j].x;
            inputFile >> tab[j].y;
        }
        inputFile >> textureFilename;
        block_texture.loadFromFile("assets/"+ textureFilename);
        shapes.emplace_back(tab, block_texture);
    }
}

bool is_move_possible(vector<Brick> &blocks, Brick test, Vector2f move)
{
    if ((test.get_position().x + move.x < px_start.x) || (test.get_position().x + move.x > px_stop.x) || (test.get_position().y + move.y > px_stop.y))
    {
        //std::cout << test.get_position().x + move.x << endl;
        return false;
    }
    for (auto& block : blocks)
    {
        if (!block.is_movable())
        {
            if (block.get_position() == (test.get_position() + move))
            {
                return false;
            }
        }
    }
    return true;
}

void add_block(vector<Brick>& next_bricks, vector<Block_Shape>& shapes, vector<Brick>& blocks)
{
    static bool srandInitialized = false;
    static int previous_number;
    int number;

    if (!srandInitialized) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        srandInitialized = true;
    }
    do
    {
        number = rand() % 7;
    }
    while (number == previous_number);
    
    for (auto& brick : next_bricks)
    {
        brick.go_up();
    }
    for (int i = 0; i < 4; i++)
    {
        next_bricks.emplace_back(px_next + Vector2f(shapes[number].get_position(i).x * 32, shapes[number].get_position(i).y * 32), shapes[number].get_texture());
        next_bricks[next_bricks.size() - 1].set_shape(number);
    }
    previous_number = number;
    if (next_bricks.size() >= 4*4)
    {
        for (auto& block : blocks)
        {
            if ((block.get_position().x >= 481 + 3 * 32) && (block.get_position().x <= 481 + 6 * 32) && (block.get_position().y <= 40 + 32) && (!block.is_movable()))
            {
                endgame();
                return;
            }
        }
        number = next_bricks[0].get_shape();
        for (int i = 0; i < 4; i++)
        {
            blocks.emplace_back(Vector2f(481 + 4 * 32, 40) + Vector2f(shapes[number].get_position(i).x * 32, shapes[number].get_position(i).y * 32), shapes[number].get_texture(), number);
        }
        for (int i = 0; i < 4; i++)
        {
            next_bricks.erase(next_bricks.begin());
        }
    }
    
}

int count(vector<Brick>& blocks, Vector2f move)
{
    int counter = 0;
    for (auto& block : blocks)
    {
        if (block.is_movable() && is_move_possible(blocks, block, move))
        {
            counter++;
        }
    }
    return counter;
}

void make_a_move(vector<Brick>& bricks, Vector2f move)
{
    if (count(bricks, move) == 4)
    {
        for (auto& brick : bricks)
        {
            if (brick.is_movable() && is_move_possible(bricks, brick, move))
            {
                if (move == move_down)
                {
                    brick.go_down();
                }
                else if (move == move_left)
                {
                    brick.go_left();
                }
                else if (move == move_right)
                {
                    brick.go_right();
                }
                
            }
        }
    }
}

Vector2f rotate_point(const Vector2f& point, const Vector2f& center)
{
    int cosA = 0;
    int sinA = 1;

    int translatedX = point.x - center.x;
    int translatedY = point.y - center.y;

    int rotatedX = translatedX * cosA - translatedY * sinA;
    int rotatedY = translatedX * sinA + translatedY * cosA;

    int finalX = rotatedX + center.x;
    int finalY = rotatedY + center.y;

    return Vector2f(finalX, finalY);
}

void rotate(vector<Brick>& bricks)
{
    vector<Brick>temp_bricks;
    for (int i = 0; i < 4; i++)
    {
        temp_bricks.push_back(bricks[bricks.size()-4+i]);
        
    }
    int y_min, y_max;
    if (!temp_bricks.empty()) 
    {
        y_min = temp_bricks[0].get_position().y;
        y_max = temp_bricks[0].get_position().y;

        for (auto& temp : temp_bricks) 
        {
            int currentY = temp.get_position().y;

            if (currentY < y_min) {
                y_min = currentY;
            }

            if (currentY > y_max) {
                y_max = currentY;
            }
        }
    }
    cout << y_max << " " << y_min << " " << (y_max - y_min) << endl;
    int height = (y_max - y_min) / 32;
    switch (height)
    {
    case 0: height = 1; break;
    case 2: height = 3; break;
    default:
        break;
    }

    switch (temp_bricks[0].get_shape())
    {
    case 6: //O shape
    {
        return;
    }
    case 2: //I shape
    {
        for (int i = 0; i < height; i++)
        {
            for (auto& temp : temp_bricks)
            {
                Vector2f t = rotate_point(temp.get_position(), temp_bricks[2].get_position());
                //cout << t.x<<" "<<t.y << endl;
                temp.set_new_position(t);
            }
        }
        break;
    }
    case 0:
    case 3:
    case 4:
    {
        for (auto& temp : temp_bricks)
        {
            Vector2f t = rotate_point(temp.get_position(), temp_bricks[2].get_position());
            //cout << t.x<<" "<<t.y << endl;
            temp.set_new_position(t);
        }
        break;
    }
    case 1:
    case 5:
    {
        for (int i = 0; i < height; i++)
        {
            for (auto& temp : temp_bricks)
            {
                Vector2f t = rotate_point(temp.get_position(), temp_bricks[1].get_position());
                //cout << t.x<<" "<<t.y << endl;
                temp.set_new_position(t);
            }
        }
        break;
    }
    default:
        break;
    }
    for (auto& temp : temp_bricks)
    {
        if ((temp.get_position().x < px_start.x) || (temp.get_position().x > px_stop.x) || (temp.get_position().y > px_stop.y))
        {
            return;
        }
        for (auto& brick : bricks)
        {
            if (!brick.is_movable())
            {
                if (brick.get_position() == temp.get_position())
                {
                    return;  //unable to rotate 
                }
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        bricks[bricks.size() - 4 + i] = temp_bricks[i];
    }
}

void remove_line(vector<Brick>& bricks, int& lines, int& score, int number=1)
{
    for (int i = 40 + 20 * 32; i >= 40; i -= 32)
    {
        int counter = 0;
        for (auto& brick : bricks)
        {
            if ((brick.get_position().y == i)&&(!brick.is_movable()))
            {
                counter++;
            }
        }
        if (counter == 10)
        {
            lines++;
            for (auto it = bricks.begin(); it != bricks.end(); ) {
                if ((it->get_position().y == i) && (!it->is_movable())) {
                    it = bricks.erase(it);
                }
                else if ((it->get_position().y < i) && (!it->is_movable())) {
                    it->go_down();
                    ++it;
                }
                else {
                    ++it;
                }
            }
            remove_line(bricks, lines, score, number+1);
            switch (number)
            {
            case 1:     score += 40;    break;
            case 2:     score += 60;    break;
            case 3:     score += 200;   break;
            case 4:     score += 900;    break;
            }
            return;
        }
    }
}

int main()
{
    int lines = 0;
    int level = 0;
    int score = 0;

    vector<Brick>bricks;
    vector<Brick>next_brick;
    vector<Block_Shape>shapes;

    Font font_arial;
    font_arial.loadFromFile("assets/arial.ttf");

    Text number[3];
    Text writing[3];
    
    writing[0].setString("Level:");
    writing[1].setString("Lines:");
    writing[2].setString("Score:");

    for (int i=0; i<3; i++)
    {
        writing[i].setCharacterSize(40);
        writing[i].setFillColor(Color(0, 0, 0));
        writing[i].Bold;
        writing[i].setFont(font_arial);
        writing[i].setOrigin(floor(writing[i].getLocalBounds().width / 2), floor(writing[i].getLocalBounds().height / 2));
        writing[i].setPosition(Vector2f(1043, 60 + 2 * i * 120));
        number[i].setCharacterSize(40);
        number[i].setFillColor(Color(0, 0, 0));
        number[i].Bold;
        number[i].setFont(font_arial);
        number[i].setPosition(Vector2f(1043, 120 + 2 * i * 120));
    }

    RectangleShape background;
    Texture background_texture;
    background_texture.loadFromFile("assets/background.png");
    background.setSize(Vector2f(1280, 720));
    background.setTexture(&background_texture);
    background.setPosition(0, 0);

    RenderWindow window(VideoMode(width, height), "Tetris");

    //muzyka
    Music soundtrack;
    if (!soundtrack.openFromFile("assets/soundtrack.ogg"))
        return -1;
    soundtrack.play();
    soundtrack.setVolume(50);
    soundtrack.setLoop(true);



    load(shapes);
    for (int i = 0; i < 4; i++)
    {
        add_block(next_brick, shapes, bricks);
    }

    auto startTime = std::chrono::steady_clock::now();
    while((window.isOpen()) && (!end_game))
    {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
        
        Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case Event::Closed:
            {
                window.close();
                break;
            }
            case Event::KeyPressed:
            {
                switch (event.key.code)
                {
                case Keyboard::A:
                case Keyboard::Left:
                {
                    make_a_move(bricks, move_left);
                    break;
                }
                case Keyboard::D:
                case Keyboard::Right:
                {
                    make_a_move(bricks, move_right);
                    break;
                }
                case Keyboard::S:
                case Keyboard::Down:
                {
                    make_a_move(bricks, move_down);
                    break;
                }
                case Keyboard::Up:
                case Keyboard::W:
                case Keyboard::Space:
                {
                    rotate(bricks);
                    break;
                }
                default:
                    break;
                }
            
            }

            default:
                break;
            }
        }
        
        if (elapsedTime >= interval)
        {
            if (count(bricks, move_down) == 4)
            {
                for (auto& brick : bricks)
                {
                    if (brick.is_movable() && is_move_possible(bricks, brick, move_down))
                    {
                        brick.go_down();
                    }
                }
            }
            else
            {
                for (auto& brick : bricks)
                {
                    if (brick.is_movable())
                    {
                        brick.stop();
                        interval=chrono::milliseconds(500 - level * 35);
                    }
                }
                add_block(next_brick, shapes, bricks);
            }
            startTime = std::chrono::steady_clock::now();
        }
        remove_line(bricks, lines, score);
        if (lines <= 100)
        {
            level = lines / 10;
        }  
        number[0].setString(to_string(level));
        number[1].setString(to_string(lines));
        number[2].setString(to_string(score));

        window.clear();
        window.draw(background);
        for (auto& brick : bricks)
        {
            window.draw(brick);
        }
        for (auto& brick : next_brick)
        {
            window.draw(brick);
        }
        for (int i = 0; i < 3; i++)
        {
            window.draw(writing[i]);
            number[i].setOrigin(floor(number[i].getLocalBounds().width / 2), floor(number[i].getLocalBounds().height / 2));
            window.draw(number[i]);
        }
        
        
        window.display();

    }
    window.close();
}