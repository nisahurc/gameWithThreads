#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <iostream>
#include <queue>
#include <ctime>
#include <string.h>
#define wHeight 40 // height of the road
#define wWidth 100 // width of the road
#define lineX 45 // x coordinate of the middle line
#define lineLEN 10 // distance of the middle line from the beginning and the end
#define EXITY 35 // coordinate showing the end of the road
#define leftKeyArrow 260 // ASCII code of the left arrow key
#define RightKeyArrow 261 // ASCII code of the right arrow key
#define leftKeyA 97// ASCII code of A
#define RightKeyD 100 // ASCII code of D
#define ESC 27 // // ASCII code of the ESC key
#define ENTER 10 // ASCII code of the ENTER key
#define KEYPUP 259 // ASCII code of the up arrow key
#define KEYDOWN 258 // ASCII code of the down arrow key
#define KEYERROR -1 // ASCII code returned if an incorrect key is pressed
#define SAVEKEY 115 // ASCII code of S
#define levelBound 300 // To increase level after 300 points
#define MAXSLEVEL 5 // maximum level
#define ISPEED 500000 // initial value for game moveSpeed
#define DRATESPEED 100000 // to decrease moveSpeed after each new level
#define MINX 5 // minimum x coordinate value when creating cars
#define MINY 10 // the maximum y coordinate value when creating the cars, then we multiply it by -1 and take its inverse
#define MINH 5 // minimum height when creating cars
#define MINW 5 // minimum width when creating cars
#define SPEEDOFCAR 3 // speed of the car driven by the player
#define YOFCAR 34 // y coordinate of the car used by the player
#define XOFCAR 45 // x coordinate of the car used by the player
#define IDSTART 10 // initial value for cars ID
#define IDMAX 20// maximum value for cars ID
#define COLOROFCAR 3 // color value of the car used by the player
#define POINTX 91 //x coordinate where the point is written
#define POINTY 42 //y coordinate where the point is written
#define MENUX 10 // x coordinate for the starting row of the menus
#define MENUY 5 // y coordinate for the starting row of the menus
#define MENUDIF 2 // difference between menu rows
#define MENUDIFX 20 // difference between menu columns
#define MENSLEEPRATE 200000 // sleep time for menu input
#define GAMESLEEPRATE 250000 // sleep time for player arrow keys
#define EnQueueSleep 1 // EnQueue sleep time
#define DeQueueSleepMin 2 // DeQueue minimum sleep time
#define numOfcolors 4 // maximum color value that can be selected for cars
#define maxCarNumber 5 // maximum number of cars in the queue
#define numOfChars 3 // maximum number of patterns that can be selected for cars
#define settingMenuItem 2 // number of options in the setting menu
#define mainMenuItem 6 // number of options in the main menu
using namespace std;
typedef struct Car{
    int ID;
    int x;
    int y;
    int height;
    int width;
    int speed;
    int clr;
    bool isExist;
    char chr;
}Car;
typedef struct Game{
    int leftKey;
    int rightKey;
    queue<Car> cars;
    bool IsGameRunning;
    bool IsSaveCliked;
    int counter;
    pthread_mutex_t mutexFile;
    Car current;
    int level;
    int moveSpeed;
    int points;
}Game;
Game playingGame; // Global variable used for new game
const char *gameTxt =  "game.txt";
const char *CarsTxt =  "cars.txt";
const char *pointsTxt =  "points.txt";
//Array with options for the Setting menu
const char *settingMenu[50] = {"Play with < and > arrow keys","Play with A and D keys"};

//Array with options for the Main menu
const char *mainMenu[50] = {"New Game","Load the last game","Instructions", "Settings","Points","Exit"};
void drawCar(Car c, int type, int direction); //prints or remove the given car on the screen
void printWindow(); //Draws the road on the screen
void *newGame(void *); // manages new game
void initGame(); // Assigns initial values to all control parameters for the new game
void initWindow(); //Creates a new window and sets I/O settings
void Menu(const char *mainMenu[], int currentSelection);
void instructions();
void settings();
void SettingsMenu(const char *settingMenu[], int currentSelection);
void printTree();
void printPoints();
void *EnqueueCars(void *arg);
void *DequeueCars(void *arg);
void *MoveCar(void *carArg);
void points();
//common
int main()
{
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    int currentSelection = 0;
    initWindow();
	while (1)
    {
		Menu(mainMenu, currentSelection);  // Update menu display
        int ch = getch();
        switch (ch)
        {
            case KEYPUP:
                if (currentSelection > 0)
                {
                    currentSelection--;
                }
                break;
            case KEYDOWN:
                if (currentSelection == mainMenuItem -1)
                {
                    currentSelection = mainMenuItem -1;
                }
                else
                    currentSelection++;
                break;
            case ENTER:
				clear();
				endwin();
				if(currentSelection==0)
                {

                    playingGame.leftKey = leftKeyArrow;
                    playingGame.rightKey = RightKeyArrow;
                    initGame();
                    initWindow();
                    pthread_t th1; //yeni thread oluştur
                    pthread_create(&th1, NULL, newGame,NULL);// Run newGame function with thread
                    pthread_join(th1, NULL);

                }
                /*if(currentSelection==1)
                {
                    //load the last game
                } */
                 if(currentSelection==2)
                {
                    instructions();
                }
                 if(currentSelection==3)
                {
                    settings();
                }
                 if(currentSelection==4)
                {
                    points();
                }
                 if(currentSelection==5)
                {
                    endwin();
                    exit(0);
                    break;
                }
				initWindow();
				currentSelection = 0;

            break;

       }
	   usleep(MENSLEEPRATE);

    }
    return 0;
}
//common
void Menu(const char *mainMenu[], int currentSelection)
{
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    for (int i = 0; i < mainMenuItem; i++)
    {
        int row = (i * MENUDIF)+MENUY;
        if (i == currentSelection)
        {
            attron((COLOR_PAIR(1)));
            mvprintw(row + MENUDIF, MENUX-2, "->%s", mainMenu[i]);
            printw("\n\n");
            attroff((COLOR_PAIR(1)));


        }
        else
        {
            attron(COLOR_PAIR(2)); // (green text)
            mvprintw(row + MENUDIF,MENUX-2, "  %s", mainMenu[i]);
            printw("\n\n");
            attroff(COLOR_PAIR(2));
        }

    }
    refresh();
}
//common
void instructions()
{
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    initWindow();
    int x =10, y = 5;
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(2));
    char text[50];
    sprintf(text,"< or A: moves the car to the left");
    mvprintw(y, x, text);
    y+= 2;
    sprintf(text,"> or D: moves the car to the right");
    mvprintw(y, x, text);
    y+= 2;
    sprintf(text,"ESC: exits the game without saving");
    mvprintw(y, x, text);
    y+= 2;
    sprintf(text,"S: saves and exits the game");
    mvprintw(y, x, text);
    attroff(COLOR_PAIR(2));
    refresh();
    sleep(5);
    refresh();
    endwin();

}
//common
void points()
{
    FILE *file;
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    initWindow();
    int x =10, y = 5;
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(2));
    file = fopen("points.txt", "r");
    int row = 0;
    if(feof(file)) //dosya boşsa
    {
       mvprintw(y, x, "No points..");
    }
    while (!feof(file)) { //dosya doluysa
        char line[100];
        if (fgets(line, sizeof(line), file) != NULL) {

            mvprintw(row, 0, "%s", line);
            row++;
            if(row %=10) //10 elemandan sonra yana geçsin
            {
                x+=10;
            }
        }
    }
    fclose(file);
    attroff(COLOR_PAIR(2));
    refresh();
    sleep(5);
    refresh();
    endwin();

}
//common
void settings()
{
    initWindow();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    int currentSelection = 0;
    SettingsMenu(settingMenu, currentSelection);
    bool selected = false;
	while (!selected)
    {
        int ch = getch();
        switch (ch)
        {
            case KEYPUP:

                if (currentSelection == 0)
                {

                    currentSelection = settingMenuItem - 1;
                }
                else
                    currentSelection--;
                break;
            case KEYDOWN:

                if (currentSelection == settingMenuItem -1)
                {
                    currentSelection = settingMenuItem -1;
                }
                else
                    currentSelection++;
                break;
            case ENTER:
                if (currentSelection == 0)
                {
                    playingGame.leftKey = leftKeyArrow;
                    playingGame.rightKey = RightKeyArrow;
                }
                else
                {
                    playingGame.leftKey = leftKeyA;
                    playingGame.rightKey = RightKeyD;
                }

				selected = true; //ekrana kapatma isteği için
                break;
        }
        SettingsMenu(settingMenu, currentSelection);
		usleep(MENSLEEPRATE); //bekleyip çıkmak için
    }
	clear();
    endwin();

}
//common
void SettingsMenu(const char *settingMenu[], int currentSelection)
{
	init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

    for (int i = 0; i < settingMenuItem; i++)
    {
        int row = (i * MENUDIF)+MENUY; // satır sütun araları ayarlama
        if (i == currentSelection)
        {
            attron((COLOR_PAIR(1))); // (red text)
            mvprintw(row+MENUDIF,MENUX-2 , "->%s\n", settingMenu[i]);
            printw("\n\n");
            attroff((COLOR_PAIR(1)));
        }
        else
        {
            attron(COLOR_PAIR(2)); //  (green text)
            mvprintw(row+MENUDIF ,MENUX-2, "  %s", settingMenu[i]);
            printw("\n\n");
            attroff(COLOR_PAIR(2));
        }
    }
    refresh();
}
void initGame()
{
    playingGame.cars = queue<Car>();
    playingGame.counter =IDSTART;
    playingGame.mutexFile = PTHREAD_MUTEX_INITIALIZER; //assigns the initial value for the mutex
    playingGame.level = 1;
    playingGame.moveSpeed = ISPEED;
    playingGame.points = 0;
    playingGame.IsSaveCliked = false;
    playingGame.IsGameRunning = true;
    playingGame.current.ID = IDSTART-1;
    playingGame.current.height = MINH;
    playingGame.current.width = MINW;
    playingGame.current.speed = SPEEDOFCAR;
    playingGame.current.x = XOFCAR;
    playingGame.current.y = YOFCAR;
    playingGame.current.clr = COLOROFCAR;
    playingGame.current.chr = '*';
}
//common
void *newGame(void *)
{
    printWindow();
    pthread_t enqThread , deqThread;
    pthread_create(&enqThread , NULL , EnqueueCars , NULL);
    pthread_create(&deqThread , NULL , DequeueCars , NULL);
    drawCar(playingGame.current,2,1); // Draw the car the player is driving on the screen
    printTree();
    printPoints();
    int key;
    while (playingGame.IsGameRunning) {
            if(playingGame.current.x == wWidth)
            {
                playingGame.current.speed = 0;
            }
            key = getch(); //İnput alma
            switch(key)
            {
                case ESC:
                    playingGame.IsGameRunning = false;
                    //endGame();
                    break;
                case SAVEKEY:
                    //saveGame();
                    playingGame.IsSaveCliked = true;
                    playingGame.IsGameRunning = false;
                    break;

                default:
                    if (key != KEYERROR) {
                        if(key ==playingGame.leftKey)
                        {
                        if(playingGame.current.x == 0 )
                        {
                        drawCar(playingGame.current,1,1); // removes player's car from screen
                        playingGame.current.x==playingGame.current.speed; // update position
                        drawCar(playingGame.current,2,1); // draw player's car with new position
                        }
                        else
                        { // sola basarsa
                        drawCar(playingGame.current,1,1); // removes player's car from screen
                        playingGame.current.x-=playingGame.current.speed; // update position
                        drawCar(playingGame.current,2,1); // draw player's car with new position
                        }
                    }
                    if(key ==playingGame.rightKey)
                    {
                        if(playingGame.current.x == wWidth - (playingGame.current.width+2) )//duvara geldiğinde durdurmak için
                        {
                        drawCar(playingGame.current,1,1); // removes player's car from screen
                        playingGame.current.x==playingGame.current.speed; // update position
                        drawCar(playingGame.current,2,1); // draw player's car with new position
                        }
                        else
                        {
                        drawCar(playingGame.current,1,1); // removes player's car from screen
                        playingGame.current.x+=playingGame.current.speed; // update position
                        drawCar(playingGame.current,2,1); // draw player's car with new position
                        }
                    }
                    break;
                }
            }
         usleep(GAMESLEEPRATE); // birazcık bekle kapan
        }

        pthread_join(enqThread , NULL);
        pthread_join(deqThread , NULL);
        endwin();
        return NULL;
}
//common
void initWindow()
{
	initscr();            // initialize the ncurses window
	start_color();        // enable color manipulation
	keypad(stdscr, true); // enable the keypad for the screen
	nodelay(stdscr, true);// set the getch() function to non-blocking mode
	curs_set(0);          // hide the cursor
	cbreak();             // disable line buffering
	noecho();             // don't echo characters entered by the user
	clear();              // clear the screen
    sleep(1);
}
void printWindow()
{
    for (int i = 1; i < wHeight - 1; ++i) {
		//mvprintw: Used to print text on the window, paramters order: y , x , string
        mvprintw(i, 2, "*"); //left side of the road
        mvprintw(i, 0, "*");
        mvprintw(i, wWidth - 1, "*");// right side of the road
        mvprintw(i, wWidth - 3, "*");
    }
    for (int i = lineLEN; i < wHeight -lineLEN ; ++i) { //line in the middle of the road
        mvprintw(i, lineX, "#");
    }
}
//common
void printTree()
{
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

        int y=5;
        for(int i =0;i<3;i++)
        {
       attron(COLOR_PAIR(2));
       mvprintw(y++,wWidth+5,"  *  ");
       mvprintw(y++,wWidth+5," * * ");
       mvprintw(y++,wWidth+5,"* * *");
       attroff(COLOR_PAIR(2));
       attron(COLOR_PAIR(1));
       mvprintw(y++,wWidth+7,"#");
       mvprintw(y++,wWidth+7,"#");
       attroff(COLOR_PAIR(1));
       y+=6;
 }
}
//common
void printPoints()
{
    initscr();
    start_color();
    init_pair(2 , COLOR_GREEN , COLOR_BLACK);
    attron(COLOR_PAIR(2));
    mvprintw(POINTY , POINTX , "Point : %d",playingGame.points);
    attroff(COLOR_PAIR(2));
}
//common
void drawCar(Car c, int type, int direction )

{
	//If the user does not want to exit the game and the game continues
    if(playingGame.IsSaveCliked!=true && playingGame.IsGameRunning==true)
    {
            init_pair(c.ID, c.clr, 0);// Creates a color pair: init_pair(short pair ID, short foregroundcolor, short backgroundcolor);
            //0: Black (COLOR_BLACK)
			//1: Red (COLOR_RED)
			//2: Green (COLOR_GREEN)
			//3: Yellow (COLOR_YELLOW)
			//4: Blue (COLOR_BLUE)
			attron(COLOR_PAIR(c.ID));//enable color pair
            char drawnChar;
            if (type == 1 )
               drawnChar = ' '; // to remove car
            else
               drawnChar= c.chr; //  to draw char
		    //mvhline: used to draw a horizontal line in the window
			//shallow. : mvhline(int y, int x, chtype ch, int n)
			//y: horizontal coordinate
			//x: vertical coordinate
			//ch: character to use
			//n: Length of the line
            mvhline(c.y, c.x, drawnChar, c.width);// top line of rectangle
            mvhline(c.y + c.height - 1, c.x, drawnChar, c.width); //bottom line of rectangle
            if(direction == 0) // If it is any car on the road
                mvhline(c.y + c.height, c.x, drawnChar, c.width);
            else //player's card
                mvhline(c.y -1, c.x, drawnChar, c.width);
		    //mvvline: used to draw a vertical line in the window
			//shallow. : mvhline(int y, int x, chtype ch, int n)
			//y: horizontal coordinate
			//x: vertical coordinate
			//ch: character to use
			//n: Length of the line
            mvvline(c.y, c.x, drawnChar, c.height); //left line of rectangle
            mvvline(c.y, c.x + c.width - 1, drawnChar, c.height); //right line of rectangle
            char text[5];
            if (type == 1 )
                sprintf(text,"  "); //to remove point
            else
                 sprintf(text,"%d",c.height * c.width); // to show car's point in rectangle
            mvprintw(c.y+1, c.x +1, text);// display car's point in rectangle
            attroff(COLOR_PAIR(c.ID));// disable color pair
    }
}
//common
void *EnqueueCars( void *arg)
{
    while(playingGame.IsGameRunning)
    {
        if(playingGame.cars.size() < maxCarNumber)
        {

            Car newCar;
            newCar.ID = playingGame.counter++;
            newCar.x = MINX + rand() % (wWidth - 2 * MINX);
            newCar.y = -MINY;
            newCar.height = MINH + rand() % 3;// between 5 and 7
            newCar.width = MINW + rand() % 3;// between 5 and 7
            newCar.speed = 1 + rand() % 2;
            newCar.clr = 1 + rand()% + numOfcolors;
            newCar.isExist = true;
            newCar.chr = '*';
            if(newCar.x + newCar.width >= lineX || newCar.x + newCar.width >= wWidth-2)
            {
               newCar.x = MINX + rand() % (wWidth - 2 * MINX);
            }
            if(newCar.ID > IDMAX)
              playingGame.counter = IDSTART;
              playingGame.moveSpeed = 500000;
              playingGame.cars.push(newCar);

        }
        sleep(EnQueueSleep);
    }
    return NULL;
}
//common
void *DequeueCars(void *arg)
{
    while(playingGame.IsGameRunning)
    {
        if(!playingGame.cars.empty())
        {
            Car movingCar = playingGame.cars.front();
            playingGame.cars.pop();
            pthread_t moveThread;
            pthread_create(&moveThread , NULL , MoveCar ,&movingCar);
            pthread_detach(moveThread);
        }
        sleep((rand() % 2) ? 4 : DeQueueSleepMin ); //random süre ayarlama
    }
    return NULL;
}
//common
void *MoveCar(void *carArg)
{
    Car *movingCar = (Car *)carArg;
    while(playingGame.IsGameRunning && movingCar ->y < EXITY)
    {
        drawCar(*movingCar , 1 , 0);
        movingCar->y += 1 + rand() % movingCar->speed; //y of the car +=  1 + rand()  % speed of the car
        drawCar(*movingCar , 0 , 0);
        if(movingCar->y >= EXITY) // sınırı geçtiyse
        {
            playingGame.points += movingCar->height * movingCar->width;//points toplama

            drawCar(*movingCar,1,0); //araba silme
            if(playingGame.points >= levelBound * playingGame.level && playingGame.level < MAXSLEVEL) //level ayarlama
            {
                playingGame.level++;
                playingGame.moveSpeed -= DRATESPEED;
            }
        }
        // arbaya çarpma koşulu
        if((playingGame.current.y <= (movingCar->y) + movingCar->height)&&
            (playingGame.current.y + playingGame.current.height >= movingCar ->y) &&
           (playingGame.current.x <= movingCar->x + movingCar->width) &&
           (playingGame.current.x + playingGame.current.width >= movingCar->x))
        {
            playingGame.IsGameRunning = false;
            return NULL;
        }
        usleep(playingGame.moveSpeed);
    }
    //poinstleri dosyaya yazdırma
    printPoints();
    FILE *file;
    file=fopen("points.txt","w");
    fprintf(file, "Point: %d\n", playingGame.points);
    fclose(file);
    return NULL;
}











