
//Ilyas Kececi 759745


#include <iostream>
#include <time.h>
#include <cstdlib>
#include <windows.h>
#include <process.h>
#include <conio.h>
using namespace std;

#define MAX 100 
#define WIDTH 80	/* Here I am drawing playing area for snake. In the next two for loops, we draw the walls for the snake game.
The value for WIDTH and HEIGHT were defined at the start of the program to be 80 and 25 respectively.*/  
#define HEIGHT 25
#define INIT_SNAKE_LENGTH 4
#define FOOD 1

//negative values represent areas that cannot be touched or else game over
#define WALL -2
#define SNAKE -1
#define NOTHING 0

#define RIGHT 0
#define UP 1
#define LEFT 2
#define DOWN 3
#define EXIT -1
static int dx[5] = { 1, 0, -1, 0 };
static int dy[5] = { 0, -1, 0, 1 };
//direction array, for use with RIGHT, UP, LEFT, DOWN constants
//(1, 0) is RIGHT
//(0, -1) is UP (because the row number increase from top to bottom)
//(-1, 0) is LEFT
//(0, 1) is DOWN

int input = RIGHT;	//global variable to take in the user's input
int item = NOTHING;

void gotoxy(int column, int row)		//Here I used gotoxy function places cursor at a desired location on screen i.e. we can change cursor position using gotoxy function.
{
	HANDLE hStdOut;			//these functions Your process gets the standard output identifier from the console and console buffer information (also, talk to Microsoft)
	COORD coord;

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	coord.X = column;
	coord.Y = row;
	SetConsoleCursorPosition(hStdOut, coord);
}

void clearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells and cell attributes in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X * csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,		//handle to console screen buffer
		(TCHAR)' ',		//character to write to the buffer
		cellCount,		//number of cells to write to
		homeCoords,		//coordinates of first cell
		&count			//receives the number of characters written
	)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,			//handle to console screen buffer
		csbi.wAttributes,	//Character attributes to use
		cellCount,			//Number of cells to set attribute
		homeCoords,			//Coordinate of first cell
		&count				//receives the number of characters written
	)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
}

//check if 2 directions are opposite
int oppositeDirection(int input1, int input2)
{
	if (input1 == LEFT && input2 == RIGHT)
		return 1;
	if (input1 == RIGHT && input2 == LEFT)
		return 1;
	if (input1 == UP && input2 == DOWN)
		return 1;
	if (input1 == DOWN && input2 == UP)
		return 1;

	return 0;
}

struct Coordinate
{
	int x, y;
};

class snake
{
private:
	int length;
	Coordinate body[WIDTH * HEIGHT];
	int direction;
	int ground[MAX][MAX];
	int foodCounter;
public:
	void initGround();
	void initSnake();
	void updateSnake(int delay);
	void updateFood();
	void firstDraw();
	int getFoodCounter();
};

void snake::initGround()		//starts off by creating an empty space of size [MAX][MAX] in the first for loop. 
								//MAX is defined in the beginning of the program to be 100 so the space created is a square with dimension 100 by 100 characters.
{
	int i, j;
	for (i = 0; i < MAX; i++)
		for (j = 0; j < MAX; j++)
			ground[i][j] = 0;

	for (i = 0; i <= WIDTH + 1; i++)
	{
		//top bottom wall
		ground[0][i] = WALL;
		ground[HEIGHT + 1][i] = WALL;
	}
	for (i = 0; i <= HEIGHT + 1; i++)
	{
		//right left wall
		ground[i][0] = WALL;
		ground[i][WIDTH + 1] = WALL;
	}
}

void snake::initSnake()			/*The initSnake() function initialises a snake of length INIT_SNAKE_LENGTH with its head at the centre of the playable ground.
The initial direction of the snake is set to the RIGHT from the input variable.input is a global variable which is used to take in the user’s input to control the 
movement of the snake. Lastly, the foodcounter is used to count the number of food picked up by the snake to show the final score of the player when the game ends.
*/ {
	length = INIT_SNAKE_LENGTH;		//set head of snake to be at the centre
	body[0].x = WIDTH / 2;
	body[0].y = HEIGHT / 2;
	direction = input;
	foodCounter = 0;

	int i;
	for (i = 1; i < length; i++)
	{
		body[i].x = body[i - 1].x - dx[direction];		//if moving right,
		body[i].y = body[i - 1].y - dy[direction];		//body is on the left
	}
	//let the ground know the snake's position
	for (i = 0; i < length; i++)
		ground[body[i].y][body[i].x] = SNAKE;
}

void snake::updateSnake(int delay) 
{
	int i;
	Coordinate prev[WIDTH * HEIGHT];
	for (i = 0; i < length; i++)
	{
		prev[i].x = body[i].x;
		prev[i].y = body[i].y;
	}

	if (input != EXIT && !oppositeDirection(direction, input))
		direction = input;

	body[0].x = prev[0].x + dx[direction];		//head of snake
	body[0].y = prev[0].y + dy[direction];		//follows the direction

	if (ground[body[0].y][body[0].x] < NOTHING)
	{
		item = -1;
		return;
	}

	if (ground[body[0].y][body[0].x] == FOOD)
	{
		length++;		//length of snake increases when it eats food
		item = FOOD;
	}
	else
	{
		ground[body[length - 1].y][body[length - 1].x] = NOTHING;
		item = NOTHING;
		gotoxy(body[length - 1].x, body[length - 1].y);		// if snake does not get food,
		cout << " ";						// erase last part because the snake is moving
	}

	for (i = 1; i < length; i++)
	{
		body[i].x = prev[i - 1].x;	//body follows the previous
		body[i].y = prev[i - 1].y;	//location that it was from
	}

	gotoxy(body[1].x, body[1].y);
	cout << "+";					//change the previous head to a body
	gotoxy(body[0].x, body[0].y);
	cout << "O";					//add a head to the snake

	//let the ground know the snake's position
	for (i = 0; i < length; i++)
		ground[body[i].y][body[i].x] = SNAKE;

	Sleep(delay);

	return;
}

void snake::updateFood()	/*This function is actually called every time the snake gets the food to create a new food on the ground 
but it can also be used to initialize the first food as what we are doing here. In the do while loop, we randomize a number for x and y to 
get the coordinate to place the food.*/
{
	int x, y;
	do
	{
		x = rand() % WIDTH + 1;
		y = rand() % HEIGHT + 1;
	} while (ground[y][x] != NOTHING);

	ground[y][x] = FOOD;
	foodCounter++;
	gotoxy(x, y);
	cout << "\u2022";	//prints a middle dot
}

void snake::firstDraw() /*The first clearScreen() function clears the command prompt console in case there is something written there. 
	We then loop through all the playable area within HEIGHT * WIDTH and display accordingly based on the number that is set on the ground[][] array beforehand. 
	The switch case within the for loops handles all the different scenarios that we have set.*/
{
	clearScreen();
	int i, j;
	for (i = 0; i <= HEIGHT + 1; i++)
	{
		for (j = 0; j <= WIDTH + 1; j++)
		{
			switch (ground[i][j])		/*When ground[i][j] is NOTHING, we print a blank space indicating that it is an accessible area.
When ground[i][j] is WALL, we print a ‘+’ sign for the corners, ‘-‘ for the top and bottom walls, and ‘|’ for the left and right walls.
When ground[i][j] is SNAKE, we print ‘+’ for the head and ‘0’ for the rest of the body.*/
			{
			case NOTHING:
				cout << " "; break;
			case WALL:
				if ((i == 0 && j == 0)
					|| (i == 0 && j == WIDTH + 1)
					|| (i == HEIGHT + 1 && j == 0)
					|| (i == HEIGHT + 1 && j == WIDTH + 1))
					cout << "+";    //the 4 corners
				else
					if (j == 0 || j == WIDTH + 1)
						cout << "|";    //left/right wall
					else
						cout << "-";    //top/bottom wall
				break;
			case SNAKE:
				if (i == body[0].y && j == body[0].x)
					cout << "o";
				else
					cout << "+";
				break;
			default:				//food
				cout << "\u2022";	//prints a middle dot
			}
		}
		cout << endl;
	}
}

int snake::getFoodCounter()
{
	return foodCounter;
}

void userInput(void* id)		/*The _beginthread(userInput, 0, (void*)0) function starts the function userInput(void* id) in a separate thread.
This is done so that the user’s input can be processed at anytime while the snake thread is looping continuously 
(if we use only 1 thread, the snake will pause for the user input before moving).*/
{
	do
	{										// here, We need to get input from users, so these four keyboard functions affect snake directions. 
		int c = _getch();
		switch (c)		
		{
		case 'W': case 'w': input = UP; break;
		case 'S': case 's': input = DOWN; break;
		case 'D': case 'd': input = RIGHT; break;
		case 'A': case 'a': input = LEFT; break;
		case 27:	    input = EXIT; break;
		}
	} while (input != EXIT && item >= 0);

	_endthread();
	return;
}

int main()
{
	int delay = 50;			//function moves the snake to the next space then stops for a period of 50 millisecon
	srand(time(NULL));		//initializes the random number generator that will be used to randomly place the “Food” for the Snake.
	snake nagini;
	nagini.initGround();		// function to initialise the playing area so lets take a look at the function itself.
	nagini.initSnake();			//This allows us to use the functions contained within the snake class.
	nagini.updateFood();
	nagini.firstDraw();
	_beginthread(userInput, 0, (void*)0);

	do
	{
		nagini.updateSnake(delay);
		if (item == FOOD)
			nagini.updateFood();
	} while (item >= 0 && input != EXIT);

	gotoxy(WIDTH / 2 - 5, HEIGHT / 2 - 2);
	cout << "GAME OVER";
	gotoxy(WIDTH / 2 - 8, HEIGHT / 2 + 2);
	cout << "Your score is " << nagini.getFoodCounter() - 1 << "!" << endl;
	gotoxy(WIDTH / 2, HEIGHT / 2);

	_getch();
	return 0;
}