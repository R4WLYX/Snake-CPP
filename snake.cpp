#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <windows.h>
#include <sstream>
#include <vector>

using namespace std;
using namespace chrono;

typedef milliseconds ms;

const int height = 51, width = 51;
const ms interval = ms(80);
const string game_over_text {R"(
 ::::::::      :::     ::::    ::::  ::::::::::      ::::::::  :::     ::: :::::::::: :::::::::  :::
 ::::::::      :::     ::::    ::::  ::::::::::      ::::::::  :::     ::: :::::::::: :::::::::  :::
:+:    :+:   :+: :+:   +:+:+: :+:+:+ :+:            :+:    :+: :+:     :+: :+:        :+:    :+: :+:
:+:    :+:   :+: :+:   +:+:+: :+:+:+ :+:            :+:    :+: :+:     :+: :+:        :+:    :+: :+:
+:+         +:+   +:+  +:+ +:+:+ +:+ +:+            +:+    +:+ +:+     +:+ +:+        +:+    +:+ +:+
+:+         +:+   +:+  +:+ +:+:+ +:+ +:+            +:+    +:+ +:+     +:+ +:+        +:+    +:+ +:+
:#:        +#++:++#++: +#+  +:+  +#+ +#++:++#       +#+    +:+ +#+     +:+ +#++:++#   +#++:++#:  +#+
:#:        +#++:++#++: +#+  +:+  +#+ +#++:++#       +#+    +:+ +#+     +:+ +#++:++#   +#++:++#:  +#+
+#+   +#+# +#+     +#+ +#+       +#+ +#+            +#+    +#+  +#+   +#+  +#+        +#+    +#+ +#+
+#+   +#+# +#+     +#+ +#+       +#+ +#+            +#+    +#+  +#+   +#+  +#+        +#+    +#+ +#+
#+#    #+# #+#     #+# #+#       #+# #+#            #+#    #+#   #+#+#+#   #+#        #+#    #+#    
#+#    #+# #+#     #+# #+#       #+# #+#            #+#    #+#   #+#+#+#   #+#        #+#    #+#    
 ########  ###     ### ###       ### ##########      ########      ###     ########## ###    ### ###
 ########  ###     ### ###       ### ##########      ########      ###     ########## ###    ### ###

)"};

bool game_over;
int frame, score;

class Snake {
    public:
    _COORD pos;
    int dir, length = 1;
    vector<_COORD> tail_pos;
};

class Fruit {
    public:
    _COORD pos;
};

Snake snake;
Fruit fruit;

bool Collision(_COORD a, _COORD b) {
    return (a.X == b.X && a.Y == b.Y);
}

void Setup() {
    game_over = false;
    frame = 0;
    score = 0;
    snake.pos.X = floor(width/2);
    snake.pos.Y = floor(height/2);
    snake.dir = 0;
    snake.length = 3;
    for (int i=0; i<snake.length; i++) {
        snake.tail_pos.emplace(snake.tail_pos.begin(), snake.pos);
        snake.pos.Y++;
    }
    snake.tail_pos.clear();
    fruit.pos.X = rand()%(width-2) + 1;
    fruit.pos.Y = rand()%(height-2) + 1;
}

void Draw() {
    stringstream ss;
    string buffer;

    ss<<"Score: "<<score<<"\n\n";
    buffer += ss.str();

    for (int x=0; x<width; x++) {
        buffer += "#";
    } buffer += "\n";

    for (int y=1; y<height-1; y++) {
        for (int x=0; x<width; x++) {
            string curr_pixel;
            curr_pixel = !(x==snake.pos.X && y==snake.pos.Y)? (!(x==fruit.pos.X && y==fruit.pos.Y)? (!(x==0 || x==width-1)? " " : "#") : "*") : "@";
            for (int i = 0; i < snake.tail_pos.size(); i++) {
                if (x==snake.tail_pos[i].X && y==snake.tail_pos[i].Y) {
                    curr_pixel = "o";
                    break;
                }
            }
            buffer += curr_pixel;
        } buffer += "\n";
    }

    for (int x=0; x<width; x++) {
        buffer += "#";
    } buffer += "\n\n";

    cout<<buffer;
}

void Input() {
    if ((GetKeyState('A') & 0x8000) && snake.dir != 2) {
        snake.dir = 1;
    } else if ((GetKeyState('D') & 0x8000) && snake.dir != 1) {
        snake.dir = 2;
    } else if ((GetKeyState('W') & 0x8000) && snake.dir != 4) {
        snake.dir = 3;
    } else if ((GetKeyState('S') & 0x8000) && snake.dir != 3) {
        snake.dir = 4;
    }
}

void Logic() {
    // Movements
    snake.tail_pos.emplace(snake.tail_pos.begin(), snake.pos);

    if (snake.dir > 0) frame++;

    switch (snake.dir) {
    case 1:
        snake.pos.X--;
        break;
    case 2:
        snake.pos.X++;
        break;
    case 3:
        snake.pos.Y--;
        break;
    case 4:
        snake.pos.Y++;
        break;
    }

    if (snake.tail_pos.size() == snake.length) snake.tail_pos.pop_back();

    // Colisions
    if (snake.pos.X == 0 || snake.pos.X == width-1 || snake.pos.Y == 0 || snake.pos.Y == height-1) {
        game_over = true;
        return;
    }
    if (Collision(snake.pos, fruit.pos)) {
        snake.length++;
        score++;
        fruit.pos.X = rand()%(width-2) + 1;
        fruit.pos.Y = rand()%(height-2) + 1;
    }

    for (int i=0; i<snake.tail_pos.size(); i++) {
        if (Collision(snake.pos, snake.tail_pos[i]) && frame > 4) {
            game_over = true;
            return;
        }
    }
}

int main() {
    restart:
    Setup();
    while (!game_over) {
        Draw();
        Input();
        Logic();
        this_thread::sleep_for(interval);
    } system("cls");
    cout<<game_over_text;
    cout<<"Score: "<<score<<"\n\n";
    cout<<"Press SPACE to restart or ESC to exit.";
    while (true) {
        if (GetKeyState(VK_SPACE) & 0x8000) break;
        if (GetKeyState(VK_ESCAPE) & 0x8000) return 0;
    }
    system("cls");
    goto restart;
}