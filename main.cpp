#include <iostream>
#include <windows.h>

using namespace std;

#define HEIGHT 24
#define WIDTH 79

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

void setcursor(bool visible, DWORD size) // set bool visible = 0 - invisible, bool visible = 1 - visible
{
    if (size == 0)
    {
        size = 20;	// default cursor size Changing to numbers from 1 to 20, decreases cursor width
    }
    CONSOLE_CURSOR_INFO lpCursor;
    lpCursor.bVisible = visible;
    lpCursor.dwSize = size;
    SetConsoleCursorInfo(console, &lpCursor);
}

char** borders(char** tab, int height = HEIGHT, int width= WIDTH) {

    for (int i = 0; i < height; ++i) {

        tab[i][0] = 'o';
        tab[i][width - 1] = 'o';
    }
    for (int i = 0; i < width; ++i) {

        tab[0][i] = 'o';
        tab[height - 1][i] = 'o';
    }

    return tab;
}

struct position {
    int x;
    int y;
};

class window {
    static const int height;
    static const int width;
    char** field;
    window(const window&);

public:
    window() {
        field = new char* [window::height];
        for (int i = 0; i < window::height; ++i) {
            field[i] = new char[window::width];
        }
    }
    ~window() {
        for (int i = 0; i < window::height; ++i) {
            delete[] field[i];
        }
        delete[] field;
    }

    void print() {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                cout << field[i][j];
            }
            cout << endl;
        }
    }

    void clear() {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                field[i][j] = ' ';
            }
        }
    }

    void set_borders() {
        field = borders(field);
    }
    int get_width() const { return width; }
    int get_height() const { return height; }


    void draw(int y, int x, char what) {
        //y = (y < 0) ? 0 : (y >= height ? height - 1 : y);
        //x = (x < 0) ? 0 : (x >= width ? width - 1 : x);

        field[y][x] = what;
    }

} field;


class point {
    position pos;
    char symbol;
public:
    point() : symbol('X'), pos() {
        pos.x = pos.y = -1;
    }

    void set_pos(int x, int y) {
        pos.x = x;
        pos.y = y;
    }

    void reposition(const window& field) {
        pos.x = rand() % field.get_width();
        pos.y = rand() % field.get_height();
    }

    int get_x() const { return pos.x; }
    int get_y() const { return pos.y; }
    char get_symbol() const { return symbol; }
} food;

class wonsz {
    enum { UP, DOWN, LEFT, RIGHT } dir;
    char symbol, head_symbol;
    position pos[100];
    position& head;
    int speed;
    int size;
    bool can_turn;
public:
    wonsz(int x, int y) :
        symbol('O'), head_symbol('8'), pos(),
        speed(1), size(1), dir(RIGHT),
        head(pos[0]), can_turn(true)
    {
        pos[0].x = x;
        pos[0].y = y;
    }

    bool check_food(const point& food) {
        if (food.get_x() == head.x && food.get_y() == head.y) {
            size += 1;
            return true;
        }
        return false;
    }

    void get_input(const window& field) {
        if (GetAsyncKeyState(VK_UP) && dir != DOWN) {
            dir = UP;
        }
        if (GetAsyncKeyState(VK_DOWN) && dir != UP) {
            dir = DOWN;
        }
        if (GetAsyncKeyState(VK_LEFT) && dir != RIGHT) {
            dir = LEFT;
        }
        if (GetAsyncKeyState(VK_RIGHT) && dir != LEFT) {
            dir = RIGHT;
        }
    }

    void move(const window& field) {
        position next = { 0, 0 };
        switch (dir) {
        case UP:
            next.y = -speed;
            break;
        case DOWN:
            next.y = speed;
            break;
        case LEFT:
            next.x = -speed;
            break;
        case RIGHT:
            next.x = speed;
        }
        for (int i = size - 1; i > 0; --i) {
            pos[i] = pos[i - 1];
        }
        head.x += next.x;
        head.y += next.y;

        if (head.x < 0 || head.y < 0 || head.x >= field.get_width() || head.y >= field.get_height()) {
            throw "Game Over! Ur dead lol";
        }
    }

    void draw(window& field) {
        for (int i = 0; i < size; ++i) {
            if (i == 0) {
                field.draw(pos[i].y, pos[i].x, head_symbol);
            }
            else {
                field.draw(pos[i].y, pos[i].x, symbol);
            }
        }
    }

    int get_x() const { return head.x; }
    int get_y() const { return head.y; }
    char get_symbol() const { return symbol; }
} snake(1, 1);


const int window::height = HEIGHT;
const int window::width = WIDTH;


int main() {
    field.clear();
    setcursor(0, 0);

    food.set_pos(5, 5);

    while (TRUE) {
        field.clear();
        field.set_borders();

        snake.get_input(field);
        try {
            snake.move(field);
        }
        catch (const char* er) {
            field.clear();
            cerr << er << endl;
            system("pause");
            return -1;
        }
        snake.draw(field);
        field.draw(food.get_y(), food.get_x(), food.get_symbol());

        if (snake.check_food(food)) {
            food.reposition(field);
        }

        field.print();

        Sleep(1000 / 30);
        system("cls");
    }

    return 0;
}