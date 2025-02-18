#include <iostream>
#include <thread>
#include <conio.h>
#include <ctime>
#include <windows.h>
#include <string>
#include <cstdlib>
#include "sound.h"
using namespace std;
//constants
const int MAP_SIZE_WIDTH = 100; 
const int MAP_SIZE_HEIGHT = 25;
const int xStart = 50;
const int yStart = 12;      
const int yGap = 3; 
//color
const int wallColor = 15, BulColor = 14, hpColor = 10, scoreColor = 11, mWallColor = 8, goalColor = 12;
const int playerColor = 223, enemyColor = 9;
//global settings
int score = 0;
int modeNum = 1;
int attacked = 0;
bool win = false;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);//color
//header
void cursorTo(int x, int y);


//self-defined type
struct Position
{
	int x;
	int y;
	const Position operator+(const Position& p2); 
};
//operator overloading
const Position Position::operator+(const Position& p2)
{
	Position p = {0, 0};
	p.x = this->x + p2.x;
	p.y = this->y + p2.y;
	return p;
}

//another headers(need Position)
void shoot(Position startPos, Position dir, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH], int range);
bool meetWall(Position checkPos, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);

//Player
class Player
{
private:
	Position pos;
	int healthPoint;
	int bulletNum;
public:
	//public variables
	int direction;
	//constructors
	Player();
	Player(Position p, int h, int b, int d);
	//copy constructors
	Player(const Player& player);
	//member functions
	void printStatus();
	void getItem(int itemNumber);
	void playerShoot(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
	void checkAndMove(Position checkPos, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
	Position getPos();
	void HPcost(int damage);
	int getPlayerHP();
};
//constructors
Player::Player()
{
	pos = {5, 5};
	healthPoint = 3;
	bulletNum = 0;
	direction = 1;
}
Player::Player(Position p, int h, int b, int d)
{
	this->pos = p;
	this->healthPoint = h;
	this->bulletNum = b;
	this->direction = d;
}
//copy constructor
Player::Player(const Player& player)
{
	this->pos = player.pos;
	this->healthPoint = player.healthPoint;
	this->bulletNum = player.bulletNum;
	this->direction = player.direction;
}
//member function
void Player::printStatus()
{
	//clear
    cursorTo(0, MAP_SIZE_HEIGHT);
    cout << "                                   ";
    cursorTo(0, MAP_SIZE_HEIGHT + 1);
    cout << "                                   ";
    cursorTo(0, MAP_SIZE_HEIGHT + 3);
    cout << "                                   ";
    cursorTo(0, MAP_SIZE_HEIGHT);
    //healthPoint
    SetConsoleTextAttribute(hConsole, hpColor);
    cout << "Health Point: ";
    cout << healthPoint;
    //bullet
    SetConsoleTextAttribute(hConsole, BulColor);
    cout << "\n";
    cout << "Bullet Number: ";
    cout << bulletNum;
    //score
    SetConsoleTextAttribute(hConsole, scoreColor);
    cout << "\n\n";
    cout << "Score: ";
    cout << score;
	//done
    SetConsoleTextAttribute(hConsole, 15);
    return;
}
void Player::getItem(int itemNumber)
{
	switch(itemNumber)
	{
        case 2:
            bulletNum += 1;
            break;
        case 3:
            healthPoint += 1;
            break;
        case 4:
            score += 1;
	}
	printStatus();
	return;
}
void Player::playerShoot(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	if (bulletNum == 0)
		return;
	//set color
	SetConsoleTextAttribute(hConsole, BulColor);
	//settings
	this->bulletNum--;
    int range = 5;
    Position shootDir = {0};
    //shoot
    switch (this->direction)
    {
    	case 1:
    		shootDir = {0, -1};
    		break;
    	case 2:
    		shootDir = {0, 1};
    		break;
    	case 4:
    		shootDir = {1, 0};
    		break;
    	case 3:
    		shootDir = {-1, 0};
	}
	shoot(this->pos, shootDir, map, range);
	//reset color
	SetConsoleTextAttribute(hConsole, 15);
	return;
} 
void Player::checkAndMove(Position checkPos, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	//check if front is not wall 
    if(!(meetWall(checkPos, map)))//map[checkPos.y][checkPos.x] != 1 && map[checkPos.y][checkPos.x] != 5
    {
    	//player disappear
    	map[this->pos.y][this->pos.x] = 0;
        cursorTo(this->pos.x, this->pos.y);
        printf(" ");
        if (map[checkPos.y][checkPos.x] == -2)
        {
        	win = true;
        	return;
		}
        //check if get item
    	this->getItem(map[checkPos.y][checkPos.x]);
    	//adjust player position
    	switch(this->direction)
    	{
    		case 1:
    			this->pos.y--;
    			break;
    		case 2:
    			this->pos.y++;
    			break;
    		case 4:
    			this->pos.x++;
    			break;
    		case 3:
    			this->pos.x--;
		}
        map[this->pos.y][this->pos.x] = -1;
    }
    //if front is a movable wall
    else if (map[checkPos.y][checkPos.x] != 1 && map[checkPos.y][checkPos.x] == 5)
    {
    	//check the place in front of the movable wall
    	Position frontOfMW = {0};
    	switch(this->direction)
    	{
    		case 1:
    			frontOfMW.x = this->pos.x;
    			frontOfMW.y = this->pos.y - 2;
    			break;
    		case 2:
    			frontOfMW.x = this->pos.x;
    			frontOfMW.y = this->pos.y + 2;
    			break;
    		case 4:
    			frontOfMW.x = this->pos.x + 2;
    			frontOfMW.y = this->pos.y;
    			break;
    		case 3:
    			frontOfMW.x = this->pos.x - 2;
    			frontOfMW.y = this->pos.y;
		}
		//if movable
    	if(!(meetWall(frontOfMW, map)) && map[frontOfMW.y][frontOfMW.x] < 90)
        {
        	//player disappear
        	map[this->pos.y][this->pos.x] = 0;
	        cursorTo(this->pos.x, this->pos.y);
	        printf(" ");
        	//move the movable wall
            cursorTo(frontOfMW.x, frontOfMW.y);
            SetConsoleTextAttribute(hConsole, mWallColor);
            cout << "#";
            SetConsoleTextAttribute(hConsole, 15);
            map[frontOfMW.y][frontOfMW.x] = 5;
            //player position change
            switch(this->direction)
    		{
	    		case 1:
	    			this->pos.y--;
	    			break;
	    		case 2:
	    			this->pos.y++;
	    			break;
	    		case 4:
	    			this->pos.x++;
	    			break;
	    		case 3:
	    			this->pos.x--;
			}
        	map[this->pos.y][this->pos.x] = -1;
        }
	}
	//player appear at new place and item(if is 2, 3, 4) disappear
    cursorTo(this->pos.x, this->pos.y);//moving cursor to the new position

    SetConsoleTextAttribute(hConsole, playerColor);

    switch(this->direction)
	{                

		case 1:
			printf("^");
			break;
		case 2:
			printf("v");
			break;
		case 4:
			printf(">");
			break;
		case 3:
			printf("<");
	}
    SetConsoleTextAttribute(hConsole, 15);
	return;
}
Position Player::getPos()
{
	return this->pos;
}
void Player::HPcost(int damage)
{
	this->healthPoint -= damage;
	return;
}
int Player::getPlayerHP()
{
	return this->healthPoint;
}


//Enemy
class Enemy
{
protected:
	static const int moveThershold = 70000;
	Position pos;
	int speed;
	int HP;
	int enemyID;
	int moveCnt;
	int itemRecord;
	bool alive;
	int color;
	//private functions
	void checkAndMove(Position checkPos, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
    void printBackRecord(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
    bool dead(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
    Position decideCheckPos();
public:
	//public variables
	int direction;//enemy might don't need this
	//constructors
	Enemy();
	Enemy(Position p, int s, int h, int d, int i);
	//copy constructor
	Enemy(const Enemy& e);
	//member functions
	void enemyRespawn(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
	virtual void enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]) = 0;//put this into the main while loop, if (e1.getEnemyHP != 0)
	bool enemyStatus();
};
//constructor
Enemy::Enemy()
{
	pos = {0, 0};
	speed = 0;
	HP = 0;
	direction = 4;
	enemyID = 0;
	moveCnt = 0;
	itemRecord = 0;
	alive = true;
	color = enemyColor;
}
Enemy::Enemy(Position p, int s, int h, int d, int i)
{

	this->pos = p;
	this->speed = s * modeNum;
	this->HP = h * modeNum;
	this->direction = d;
	this->enemyID = i;
	this->moveCnt = 0;
	this->itemRecord = 0;
	this->alive = true;
	
	if (this->HP >= 7)
		this->color = 4;
	else if (this->HP >= 4)
		this->color = 6;
	else
		this->color = enemyColor;
}
//copy constrctor
Enemy::Enemy(const Enemy& e)
{
	this->pos = e.pos;
	this->speed = e.speed;
	this->HP = e.HP;
	this->direction = e.direction;
	this->enemyID = e.enemyID;
	this->moveCnt = e.moveCnt;
	this->itemRecord = e.itemRecord;
	this->alive = e.alive;
	this->color = e.color;
}
//member function
void Enemy::enemyRespawn(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	this->alive = true;
	this->HP = 1 * modeNum;
	map[this->pos.y][this->pos.x] = enemyID;
	cursorTo(this->pos.x, this->pos.y);
    SetConsoleTextAttribute(hConsole, this->color);
	cout << "@";
    SetConsoleTextAttribute(hConsole, 15);
	return;
}
void Enemy::printBackRecord(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])//differ for every enemy
{
    map[this->pos.y][this->pos.x] = this->itemRecord;
    //print the Item out
    cursorTo(this->pos.x, this->pos.y);
    switch(this->itemRecord)
    {
        case 2:
            SetConsoleTextAttribute(hConsole, BulColor);
            cout << "B";
            SetConsoleTextAttribute(hConsole, 15);
            break;
        case 3:
            SetConsoleTextAttribute(hConsole, hpColor);
            cout << "H";
            SetConsoleTextAttribute(hConsole, 15);
            break;
        case 4:
            SetConsoleTextAttribute(hConsole, scoreColor);
            cout << "$";
            SetConsoleTextAttribute(hConsole, 15);
            break;
    }
    this->itemRecord = 0;
}
void Enemy::checkAndMove(Position checkPos, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	//before move
	if (this->itemRecord != 0)
	{
		printBackRecord(map);
	}
	else
	{
		//enemy disappear
		map[this->pos.y][this->pos.x] = 0;
		cursorTo(this->pos.x, this->pos.y);
		cout << " ";
	}
	//get front ID
	int front = map[checkPos.y][checkPos.x];
	//then...
	if (front != 0)//front is not 0(blank)
	{
		if (front == -1)//front is player
		{
			attacked += this->HP;
			this->HP = 0;
		}
		else if (front >= 2 && front <= 4)//front is item
		{
			//record the itemType
			this->itemRecord = front;
	        //adjust enemy position
			this->pos = checkPos;
		    map[this->pos.y][this->pos.x] = enemyID;
		    cursorTo(this->pos.x, this->pos.y);//moving cursor to the new position
            SetConsoleTextAttribute(hConsole, this->color);
            cout << "@";
            SetConsoleTextAttribute(hConsole, 15);
			//move and print the turned(or maybe we can just don't do this)
		}
	}
	else
	{
        //adjust enemy position
    	this->pos = checkPos;
        map[this->pos.y][this->pos.x] = enemyID;
        cursorTo(this->pos.x, this->pos.y);//moving cursor to the new position
        SetConsoleTextAttribute(hConsole, this->color);
        cout << "@";
        SetConsoleTextAttribute(hConsole, 15);
	}
	return;
}
bool Enemy::dead(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	if (this->alive && (this->HP == 0 || map[this->pos.y][this->pos.x] == -99 || map[this->pos.y][this->pos.x] == -1))
	{
		//if bumped by player
		if (map[this->pos.y][this->pos.x] == -1)
		{
			attacked = this->HP;
		}
		//ex bump into player
		else
		{
			if (this->itemRecord != 0)
			{
				printBackRecord(map);
			}
			else
			{
				map[this->pos.y][this->pos.x] = 0;
				cursorTo(this->pos.x, this->pos.y);
				cout << " ";
			}
		}
		this->alive = false;
	}
	if(!this->alive)
	{
		return true;
	}
	return false;
}
Position Enemy::decideCheckPos()
{
	Position checkPos = {0};
	switch(this->direction)
	{
		case 1://up
			checkPos = {this->pos.x, this->pos.y - 1};
			break;
		case 2://down
			checkPos = {this->pos.x, this->pos.y + 1};
			break;
		case 3://left
			checkPos = {this->pos.x - 1, this->pos.y};
			break;
		case 4://right
			checkPos = {this->pos.x + 1, this->pos.y};
	}
	return checkPos;
}
void Enemy::enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])//differ for every enemy
{
	
}
bool Enemy::enemyStatus()
{
	return this->alive;
}


//Clockwise
class EnemyClockwise : public Enemy
{
private:
	
public:
	//constructors
	EnemyClockwise();
	EnemyClockwise(Position p, int s, int h, int d, int i);
	//copy constructor
	EnemyClockwise(const EnemyClockwise& e);
	//member function
	void enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
};
//constructor
EnemyClockwise::EnemyClockwise()
{
	
}
EnemyClockwise::EnemyClockwise(Position p, int s, int h, int d, int i) : Enemy(p, s, h, d, i)
{
	
}
//copy constructor
EnemyClockwise::EnemyClockwise(const EnemyClockwise& e) : Enemy(e)
{
	
}
//member function
void EnemyClockwise::enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	//if enemy has died
	if (this->dead(map))
	{
		return;
	}
	//enemy move speed
	this->moveCnt += this->speed;
	//deciding move direction
	Position checkPos = decideCheckPos();
	//if front is not wall(any kind) or start/end point, change direction
	if (meetWall(checkPos, map) || map[checkPos.y][checkPos.x] > 90 || map[checkPos.y][checkPos.x] == -2)
	{
		switch(this->direction)
		{
			case 1:
				this->direction = 4;
				break;
			case 2:
				this->direction = 3;
				break;
			case 3:
				this->direction = 1;
				break;
			case 4:
				this->direction = 2;
		}
	}
	//if movable
	else if (this->moveCnt >= moveThershold)
	{
		this->moveCnt = 0;
		checkAndMove(checkPos, map);
	}	
	return;
}

//counter
class EnemyCounterClockwise : public Enemy
{
private:
	
public:
	//constructors
	EnemyCounterClockwise();
	EnemyCounterClockwise(Position p, int s, int h, int d, int i);
	//copy constructor
	EnemyCounterClockwise(const EnemyCounterClockwise& e);
	//member function
	void enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
};
//constructor
EnemyCounterClockwise::EnemyCounterClockwise()
{
	
}
EnemyCounterClockwise::EnemyCounterClockwise(Position p, int s, int h, int d, int i) : Enemy(p, s, h, d, i)
{
	
}
//copy constructor
EnemyCounterClockwise::EnemyCounterClockwise(const EnemyCounterClockwise& e) : Enemy(e)
{
	
}
//member function
void EnemyCounterClockwise::enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	//if enemy has died
	if (this->dead(map))
	{
		return;
	}
	//enemy move speed
	this->moveCnt += this->speed;
	//deciding move direction
	Position checkPos = decideCheckPos();
	//if front is not wall(any kind) or start/end point, change direction
	if (meetWall(checkPos, map) || map[checkPos.y][checkPos.x] > 90 || map[checkPos.y][checkPos.x] == -2)
	{
		switch(this->direction)
		{
			case 1:
				this->direction = 3;
				break;
			case 2:
				this->direction = 4;
				break;
			case 3:
				this->direction = 2;
				break;
			case 4:
				this->direction = 1;
		}
	}
	//if movable
	else if (this->moveCnt >= moveThershold)
	{
		this->moveCnt = 0;
		checkAndMove(checkPos, map);
	}	
	return;
}

//random
class EnemyRandom : public Enemy
{
private:
	int step;
public:
	//constructors
	EnemyRandom();
	EnemyRandom(Position p, int s, int h, int d, int i);
	//copy constructor
	EnemyRandom(const EnemyRandom& e);
	//member function
	void enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
};
//constructor
EnemyRandom::EnemyRandom() : step(0)
{
	
}
EnemyRandom::EnemyRandom(Position p, int s, int h, int d, int i) : Enemy(p, s, h, d, i)
{
	step = 0;
}
//copy constructor
EnemyRandom::EnemyRandom(const EnemyRandom& e) : Enemy(e)
{
	
}
//member function
void EnemyRandom::enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	//if enemy has died
	if (this->dead(map))
	{
		return;
	}
	//enemy move speed
	this->moveCnt += this->speed;
	//deciding move direction
	if (step > 3 && this->moveCnt >= moveThershold)
	{
		int rn = 0;
		rn = (rand() + this->pos.x + this->pos.y) % 4 + 1 ;
		this->direction = rn;
		step = 0;
	}
	Position checkPos = decideCheckPos();
	//change direction
	if (meetWall(checkPos, map) || map[checkPos.y][checkPos.x] == -2 || map[checkPos.y][checkPos.x] > 90)//should be better
	{
		switch(this->direction)
		{
			case 1:
				this->direction = 4;
				break;
			case 2:
				this->direction = 3;
				break;
			case 3:
				this->direction = 1;
				break;
			case 4:
				this->direction = 2;
		}
		step = 0;
	}
	//if movable
	else if (this->moveCnt >= moveThershold)
	{
		this->moveCnt = 0;
		checkAndMove(checkPos, map);
		step++;
	}	
	return;
}



//enemys
class EnemyTeam
{
private:
	int capacity;
	int cnt;
	Enemy** enemyPtr;
public:
	//constructors
	EnemyTeam();
	//copy constructor
//	EnemyTeam(const EnemyTeam& team);//might not need here
	//destructors
	~EnemyTeam();
	//member functions
	void addCEnemy(Position p, int s, int h, int d, int i);
	void addCCEnemy(Position p, int s, int h, int d, int i);
	void addREnemy(Position p, int s, int h, int d, int i);
	void allEnemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
};
EnemyTeam::EnemyTeam()
{
	this->cnt = 0;
	this->capacity = 1000;
	this->enemyPtr = new Enemy*[this->capacity];
}
EnemyTeam::~EnemyTeam()
{
	for(int i = 0; i < this->cnt; i++)
		delete this->enemyPtr[i];
	delete [] this->enemyPtr;
	return;
}
void EnemyTeam::addCEnemy(Position p, int s, int h, int d, int i)
{
	if (this->cnt < this->capacity)
	{
		this->enemyPtr[cnt] = new EnemyClockwise(p, s, h, d, i);
		cnt++;
		return;
	}
	return;
}
void EnemyTeam::addCCEnemy(Position p, int s, int h, int d, int i)
{
	if (this->cnt < this->capacity)
	{
		this->enemyPtr[cnt] = new EnemyCounterClockwise(p, s, h, d, i);
		cnt++;
		return;
	}
	return;
}
void EnemyTeam::addREnemy(Position p, int s, int h, int d, int i)
{
	if (this->cnt < this->capacity)
	{
		this->enemyPtr[cnt] = new EnemyRandom(p, s, h, d, i);
		cnt++;
		return;
	}
	return;
}
void EnemyTeam::allEnemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	srand(time(nullptr));//
	for (int i = 0; i < cnt; i++)
	{
		if (enemyPtr[i]->enemyStatus())
		{
			enemyPtr[i]->enemyMove(map);
		}
	}
}


//header 
//other
void delayPrint(string s);
//before game
void menu();
void mode();
void settings();
//in game
void printInfo();//game hint
int direction(Position from, Position to);
void printMap(int direction, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
//after game
void gameEnding();
void winning(int startTime);
void losing();

////////////////////////////////////////////////////   


int main()
{
    //create map (-2 is the end, 2 is bullet, 3 is hp, 4 is score)
    int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH] = 
    {
/*0*/    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*1*/    1, 0, 1, 0, 0,92, 0, 0, 1, 3, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 2, 0, 4, 1, 2, 0, 4, 1, 2, 0, 4, 1, 2, 0, 0, 1, 3, 2, 3, 2, 3, 2, 3, 2, 1, 0, 0, 0, 0,93, 0,93, 0,93, 0, 0, 0, 0, 3, 2, 3, 2, 3, 2, 0, 5, 0,91, 0,91, 0,91, 0,91, 0,91, 0,91, 0,91, 0,91, 0,91, 4, 1, 1, 0, 0, 5, 5, 5, -2, 
/*2*/    1, 0, 1,92, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0,93, 2, 0, 1, 0, 1, 5, 1, 0, 1, 5, 1, 0, 1, 5, 1, 0, 1, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 2, 1, 4, 1, 5, 1, 
/*3*/    1, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 4, 4, 0,93, 0, 0, 1, 3, 1, 0, 1, 3, 1, 0, 1, 3, 1, 0, 1, 3, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 5, 1, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 2, 3, 0, 0, 2, 3, 0, 1, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 1, 4, 0, 0, 4, 5, 1, 
/*4*/    1, 0, 0, 0, 2, 2, 2, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0,93, 1, 4, 4, 0,93, 0, 3, 1, 0, 1, 0, 1, 0, 1, 0, 1,93, 1,93, 1,93, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 1, 0, 0, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 0, 0, 1, 4, 2, 1, 0, 0, 4, 1, 
/*5*/    1, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 2, 0, 0, 1, 2, 1, 3, 1, 0, 1, 4, 1, 0, 1, 3, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 5, 1, 0, 1, 1, 4, 0, 0, 2, 3, 0, 0, 1, 1, 2, 0, 0, 0,93,93, 0, 0, 1, 4, 1, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 2, 0, 2, 0, 2, 0, 0, 0, 4, 1, 1, 4, 0, 1, 0, 4, 1, 
/*6*/    1, 0, 1,92, 0, 0, 0,92, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1,93, 0,93, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 5, 1, 0, 1, 5, 1, 0, 1, 5, 1, 0, 1, 1,92, 0, 4, 0,92, 1, 0, 1, 4, 1, 4, 0, 3, 5, 0, 0, 1, 1, 2, 1, 1, 0,93,93, 0, 0, 1, 4, 1, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 2, 0, 2, 0, 2, 0, 0, 0, 1, 4, 2, 0,91, 0, 4, 1, 
/*7*/    1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 2, 0, 4, 1, 2, 0, 3, 1, 2, 0, 4, 1, 3, 3, 1, 0, 0, 3, 0, 0, 1, 0, 1, 4, 4, 1, 4, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 0, 0, 0, 0, 1, 4, 1, 0, 0,93, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 4, 1, 1, 4,93, 0, 1, 4, 1, 
/*8*/    1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 3, 3, 3, 3, 3, 0, 1, 0, 0, 0, 1, 0, 2, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 4, 3, 2, 3, 4, 1,93, 1, 4, 4, 4, 1, 4, 0, 0, 5, 3, 0, 0,93, 0, 3, 1, 1, 0, 0, 1, 4, 1, 0, 0,93, 0, 0, 1, 2, 2, 1, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 1, 4, 3, 0,93, 0, 4, 1, 
/*9*/    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 2,92, 2,92, 3,92, 3,92, 2,92, 3, 0, 1, 0, 1, 0, 0, 3, 0, 0, 1, 0, 1, 5, 5, 5, 5, 1, 4, 2, 3, 2, 0, 0,93, 5, 3, 1, 1, 2, 0, 1, 4, 1, 0, 0,93, 0, 1, 3, 3, 3, 3, 1, 3, 3, 3, 3, 1, 0, 0, 0, 4, 1, 1, 3, 1, 1, 1, 0, 1, 
/*10*/   1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,92, 1, 0,92, 2,93, 0, 2, 0, 0, 5, 0, 1, 3, 0, 1, 0, 0, 1, 3, 1, 0, 0, 0, 0, 1, 3, 1, 0, 4, 1, 0, 1,92, 0, 4, 0,92, 1, 0, 1, 2, 2, 5, 3, 3, 1, 4, 2, 3, 5, 0,93, 0, 0, 0, 0, 3, 0, 1, 4, 1, 0, 0,93, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 0, 0, 0, 1, 4, 3, 1, 0, 4, 4, 1, 
/*11*/   1, 1, 5, 1, 5, 1, 5, 1, 5, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 2, 0, 2, 1, 2, 0, 0, 0, 0, 2, 1, 2, 0, 3, 1, 0, 1, 5, 1, 1, 1, 1, 1, 0, 1, 2, 2, 0, 0, 3, 3, 1, 4, 2, 0, 0, 0, 0, 0, 1, 1, 2, 0, 1, 5, 1, 0, 0,93, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 4, 1, 1, 0, 1,93, 4, 4, 1, 
/*12*/   1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 2, 0, 0, 0, 0,91, 0, 2, 1, 2, 3, 0, 0, 1, 0, 0, 1, 3, 1, 0, 0, 0, 0, 1, 3, 1, 0, 4, 1, 0, 1,91, 0, 2, 0,91, 1,93, 1, 1, 1, 1, 5, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, 1, 1, 3, 0, 1, 3, 1, 0, 0,93, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 0, 0, 0, 1, 4, 0, 1, 1, 1, 1, 1, 
/*13*/   1, 0, 1, 0, 0, 3, 2, 3, 0, 1, 1, 0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 1, 3, 0, 0, 0,92, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 1, 0, 0, 0, 0, 0, 1,92, 0, 0,92, 1, 0, 3, 3,93,93, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0,93, 1, 4, 4, 4, 4, 4,93, 4, 4, 4, 4, 4, 1, 0, 0, 4, 1, 1, 0, 0,91, 0, 0, 1, 
/*14*/   1,93, 0, 0, 0, 2, 3, 1, 0, 0, 1, 0, 1, 0, 0, 0, 4, 4, 3, 0, 0, 1, 0, 0, 1, 2,91, 0, 0,93,93, 0, 0, 0, 0, 0, 0,93,93, 0, 4, 1, 0, 1, 2, 0, 3, 0, 2, 1, 0, 0, 0, 0, 1, 0, 3, 3, 0,93, 5, 1, 4, 0, 0, 5, 0, 0, 0, 0, 5, 3, 1, 0, 0,93, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 
/*15*/   1, 0, 0, 0, 1, 3, 2, 3, 0, 0, 1, 0, 1, 0, 0, 3, 4, 4, 0, 0, 0, 1, 0, 0, 4, 0,92, 2, 0,93,93, 0, 1, 1, 1, 1, 0,93,93, 0, 3, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 3, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 4, 0, 0, 0, 0, 0, 0, 5, 2, 1, 0, 0,93, 0, 0, 1, 3, 3, 3, 3, 3, 3, 3, 1, 0, 0, 0, 0, 4, 1, 1, 1, 1, 1, 1, 0, 1, 
/*16*/   1, 1, 0, 0,93, 0, 0, 0, 0, 0, 1, 5, 1, 2,93,93,93, 3, 0, 0, 2, 1, 0, 1, 4, 0, 1, 0, 0, 0, 0, 1, 4, 4, 4, 4, 1, 0, 0, 0, 4, 1, 0, 1,91, 0, 2, 0,91, 1, 0, 4, 1, 0, 1, 2, 3, 0, 0, 1, 2, 1, 4, 1, 4, 0, 0, 5, 0, 0, 5, 3, 1, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 1, 0, 0, 0, 0, 0, 0, 1, 4, 0, 0, 4, 4, 0, 1, 
/*17*/   1, 4, 0, 0, 0, 0, 0, 0, 1, 3, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,93, 4, 4, 0, 1, 2, 0, 0, 1, 4, 2, 2, 2, 2, 4, 1, 0, 0, 2, 1, 0, 1, 1, 1, 1, 1, 5, 1, 0, 1, 2, 0, 1, 3, 3, 0, 0, 1, 0, 1, 4, 0, 1, 4, 0, 0, 0, 0, 5, 2, 1, 3, 0, 0,93, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,93, 0, 0, 4, 1, 1, 0,93, 4, 4, 0, 1, 
/*18*/   1, 4, 0, 1, 0, 0, 0,93, 3, 2, 1, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 1,93, 0, 1, 0, 1, 0, 0, 0, 1, 2, 0, 0, 0, 0, 2, 1, 0, 0, 4, 1, 0, 1,92, 0, 3, 0,92, 1, 0, 4, 1, 0, 1, 0, 0, 0, 0, 1, 3, 1,93, 0, 0, 1, 4, 0, 0, 0, 1, 3, 1, 4, 0, 0, 0, 0, 0, 0, 0, 1, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 0, 1, 1, 1, 1, 1, 
/*19*/   1, 4, 0, 0, 0,93, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 5, 1, 1, 1, 1,93, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 0, 1, 0, 2, 0, 2, 0, 1, 0, 1, 3, 0, 1, 0, 0, 0, 0, 1, 0, 1, 4, 3, 5, 0, 1, 4, 0, 0, 1, 2, 1, 4, 3, 0, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 5, 0, 0, 4, 1, 1, 0, 0, 0, 0, 4, 1,
/*20*/   1, 0, 0, 2, 3, 0, 0, 0, 0, 0, 1, 3, 4, 3, 1,92, 0,91, 1, 3, 2, 1, 0, 1, 4, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 4, 1, 0, 1, 3, 0, 2, 0, 3, 1, 0, 4, 1, 0, 1, 0, 0, 3, 2, 1, 2, 1, 4, 0, 1, 0, 0, 1, 4, 4, 1, 0, 1, 4, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 0, 0, 0, 0, 4, 1, 
/*21*/   1, 0,93, 3, 2, 3, 0, 0, 1, 4, 1, 4, 2, 4, 1, 0,93, 0, 1, 5, 3, 1, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 1, 0, 2, 0, 2, 0, 1, 0, 1, 2, 0, 1, 0, 0, 2, 2, 1, 0, 1, 1, 2, 1, 0, 0, 0, 1, 1, 1, 5, 1, 4, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 1, 1, 1, 1, 0, 1, 1,
/*22*/   1, 0, 0, 1, 3, 0, 0, 0, 4, 4, 1, 3, 4, 3, 5, 0, 0, 0, 5, 2, 5, 1, 0, 0, 0, 0, 5, 0, 4,91, 4,91, 4,91, 4,91, 4,91, 4,91, 4, 1, 2, 5,92, 0, 3, 0,92, 1, 0, 4, 1, 0, 5, 0, 0, 0, 0, 1, 3, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
/*23*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    };    
	   //0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99
	
	//game status
    int gameRunning = 1;
    
////////GAME START//////////////////////////////////

	//menu music
	thread t1(playMenu);
    // open menu
    menu();
    PlaySound(nullptr, nullptr, 0);
    //record time
    clock_t startTime = clock();

	//character settings 
	//player
	Position spawnPos = {1, 1};
	Player player(spawnPos, (10 / modeNum), 6, 1);
	map[player.getPos().y][player.getPos().x] = -1;
    //enemy(use array)
	EnemyTeam enemyTeam;

	//first is x, second is y
	enemyTeam.addCEnemy({18, 12}, 20, 2, 4, 91);
	enemyTeam.addCEnemy({17, 20}, 20, 2, 4, 91);
	enemyTeam.addCEnemy({26, 14}, 20, 2, 4, 91);
	enemyTeam.addCEnemy({44, 12}, 20, 7, 4, 91);
	enemyTeam.addCEnemy({44, 16}, 20, 4, 4, 91);
	enemyTeam.addCEnemy({48, 12}, 20, 7, 4, 91);
	enemyTeam.addCEnemy({48, 16}, 20, 4, 4, 91);
	enemyTeam.addCEnemy({96, 6}, 20, 2, 4, 91);
	enemyTeam.addCEnemy({96, 13}, 20, 2, 4, 91);
	enemyTeam.addCEnemy({29, 22}, 20, 2, 4, 91);
	enemyTeam.addCEnemy({31, 22}, 20, 2, 4, 91);
	enemyTeam.addCEnemy({33, 22}, 20, 2, 4, 91);
	enemyTeam.addCEnemy({35, 22}, 20, 2, 4, 91);
	enemyTeam.addCEnemy({37, 22}, 20, 2, 4, 91);
	enemyTeam.addCEnemy({39, 22}, 20, 2, 4, 91);
	enemyTeam.addCEnemy({74, 1}, 5, 7, 4, 91);
	enemyTeam.addCEnemy({76, 1}, 5, 7, 4, 91);
	enemyTeam.addCEnemy({78, 1}, 40, 2, 4, 91);
	enemyTeam.addCEnemy({80, 1}, 40, 2, 4, 91);
	enemyTeam.addCEnemy({82, 1}, 40, 2, 4, 91);
	enemyTeam.addCEnemy({84, 1}, 40, 2, 4, 91);
	enemyTeam.addCEnemy({86, 1}, 5, 7, 4, 91);
	enemyTeam.addCEnemy({88, 1}, 40, 2, 4, 91);
	enemyTeam.addCEnemy({90, 1}, 5, 7, 4, 91);

	enemyTeam.addCCEnemy({11, 10}, 20, 2, 4, 92);
	enemyTeam.addCCEnemy({7, 6}, 70, 8, 1, 92);
	enemyTeam.addCCEnemy({3, 6}, 70, 4, 2, 92);
	enemyTeam.addCCEnemy({3, 2}, 70, 8, 2, 92);
	enemyTeam.addCCEnemy({5, 1}, 70, 4, 3, 92);
	enemyTeam.addCCEnemy({15, 20}, 20, 2, 3, 92);
	enemyTeam.addCCEnemy({14, 10}, 20, 2, 3, 92);
	enemyTeam.addCCEnemy({26, 13}, 20, 2, 3, 92);
	enemyTeam.addCCEnemy({26, 15}, 20, 2, 3, 92);
	enemyTeam.addCCEnemy({30, 9}, 20, 2, 3, 92);
	enemyTeam.addCCEnemy({32, 9}, 20, 2, 3, 92);
	enemyTeam.addCCEnemy({34, 9}, 20, 2, 3, 92);
	enemyTeam.addCCEnemy({36, 9}, 20, 2, 3, 92);
	enemyTeam.addCCEnemy({38, 9}, 20, 2, 3, 92);
	enemyTeam.addCCEnemy({44, 6}, 20, 7, 3, 92);
	enemyTeam.addCCEnemy({44, 10}, 20, 7, 3, 92);
	enemyTeam.addCCEnemy({44, 18}, 20, 4, 3, 92);
	enemyTeam.addCCEnemy({44, 22}, 20, 4, 3, 92);
	enemyTeam.addCCEnemy({48, 6}, 20, 7, 3, 92);
	enemyTeam.addCCEnemy({48, 10}, 20, 7, 3, 92);
	enemyTeam.addCCEnemy({48, 18}, 20, 4, 3, 92);
	enemyTeam.addCCEnemy({48, 22}, 20, 4, 3, 92);
	enemyTeam.addCCEnemy({50, 13}, 20, 2, 3, 92);
	enemyTeam.addCCEnemy({53, 13}, 20, 2, 3, 92);

	enemyTeam.addREnemy({4, 16}, 20, 2, 4, 93);
	enemyTeam.addREnemy({5, 19}, 20, 2, 4, 93);
	enemyTeam.addREnemy({2, 21}, 20, 2, 4, 93);
	enemyTeam.addREnemy({7, 18}, 20, 2, 4, 93);
	enemyTeam.addREnemy({1, 14}, 20, 2, 4, 93);
	enemyTeam.addREnemy({16, 21}, 20, 2, 4, 93);
	enemyTeam.addREnemy({16, 10}, 20, 2, 4, 93);
	enemyTeam.addREnemy({14, 16}, 20, 2, 4, 93);
	enemyTeam.addREnemy({15, 16}, 20, 2, 4, 93);
	enemyTeam.addREnemy({16, 16}, 20, 2, 4, 93);
	enemyTeam.addREnemy({18, 4}, 20, 2, 4, 93);
	enemyTeam.addREnemy({18, 6}, 20, 2, 4, 93);
	enemyTeam.addREnemy({20, 6}, 20, 2, 4, 93);
	enemyTeam.addREnemy({22, 17}, 20, 2, 4, 93);
	enemyTeam.addREnemy({22, 18}, 20, 2, 4, 93);
	enemyTeam.addREnemy({22, 19}, 20, 2, 4, 93);
	enemyTeam.addREnemy({23, 2}, 20, 2, 4, 93);
	enemyTeam.addREnemy({23, 3}, 20, 2, 4, 93);
	enemyTeam.addREnemy({23, 4}, 20, 2, 4, 93);
	enemyTeam.addREnemy({29, 14}, 20, 2, 4, 93);
	enemyTeam.addREnemy({29, 15}, 20, 2, 4, 93);
	enemyTeam.addREnemy({30, 14}, 20, 2, 4, 93);
	enemyTeam.addREnemy({30, 15}, 20, 2, 4, 93);
	enemyTeam.addREnemy({35, 4}, 20, 2, 4, 93);
	enemyTeam.addREnemy({37, 4}, 20, 2, 4, 93);
	enemyTeam.addREnemy({39, 4}, 20, 2, 4, 93);
	enemyTeam.addREnemy({37, 14}, 20, 2, 4, 93);
	enemyTeam.addREnemy({37, 15}, 20, 2, 4, 93);
	enemyTeam.addREnemy({38, 14}, 20, 2, 4, 93);
	enemyTeam.addREnemy({38, 15}, 20, 2, 4, 93);
	enemyTeam.addREnemy({50, 8}, 20, 2, 4, 93);
	enemyTeam.addREnemy({50, 12}, 20, 2, 4, 93);
	enemyTeam.addREnemy({56, 1}, 20, 2, 4, 93);
	enemyTeam.addREnemy({58, 1}, 20, 2, 4, 93);
	enemyTeam.addREnemy({60, 1}, 20, 2, 4, 93);
	enemyTeam.addREnemy({58, 13}, 20, 2, 4, 93);
	enemyTeam.addREnemy({59, 13}, 20, 2, 4, 93);
	enemyTeam.addREnemy({59, 14}, 20, 2, 4, 93);
	enemyTeam.addREnemy({62, 18}, 100, 9, 4, 93);
	enemyTeam.addREnemy({63, 8}, 20, 2, 4, 93);
	enemyTeam.addREnemy({63, 9}, 20, 2, 4, 93);
	enemyTeam.addREnemy({63, 10}, 20, 2, 4, 93);
	enemyTeam.addREnemy({66, 5}, 20, 2, 4, 93);
	enemyTeam.addREnemy({66, 6}, 20, 2, 4, 93);
	enemyTeam.addREnemy({67, 5}, 20, 2, 4, 93);
	enemyTeam.addREnemy({67, 6}, 20, 2, 4, 93);
	enemyTeam.addREnemy({76, 17}, 40, 2, 4, 93);
	enemyTeam.addREnemy({88, 17}, 40, 2, 4, 93);
	enemyTeam.addREnemy({95, 7}, 20, 2, 4, 93);
	enemyTeam.addREnemy({95, 17}, 20, 2, 4, 93);
	enemyTeam.addREnemy({96, 8}, 20, 2, 4, 93);
	enemyTeam.addREnemy({96, 11}, 20, 2, 4, 93);
	enemyTeam.addREnemy({75, 7}, 40, 2, 4, 93);
	enemyTeam.addREnemy({75, 8}, 40, 2, 4, 93);
	enemyTeam.addREnemy({75, 9}, 40, 2, 4, 93);
	enemyTeam.addREnemy({75, 10}, 15, 4, 4, 93);
	enemyTeam.addREnemy({75, 11}, 40, 2, 4, 93);
	enemyTeam.addREnemy({75, 12}, 40, 2, 4, 93);
	enemyTeam.addREnemy({75, 13}, 15, 4, 4, 93);
	enemyTeam.addREnemy({75, 14}, 40, 2, 4, 93);
	enemyTeam.addREnemy({75, 15}, 40, 2, 4, 93);
	enemyTeam.addREnemy({82, 13}, 70, 30, 4, 93);

	//print map and info
    system("cls");
    printMap(player.direction, map);
	//printInfo
    player.printStatus(); 
    printInfo();
    cursorTo(player.getPos().x, player.getPos().y);
    //battle music
	thread t2(playBattle);

	//game resume
    while(gameRunning == 1)
    {  
    	//player HP cost
    	if (attacked > 0)
    	{
    		player.HPcost(attacked);
    		attacked = 0;
    		player.printStatus();
		}
		//enemy move
		enemyTeam.allEnemyMove(map);
		//player move
        if(kbhit())
        {
            int ch = getch();
			//up
            if(ch == 'w')
            {
            	player.direction = 1;
            	Position checkPos = {player.getPos().x, player.getPos().y - 1};
            	player.checkAndMove(checkPos, map);
            }
			//left
            if(ch == 'a')
            {
                player.direction = 3;
            	Position checkPos = {player.getPos().x - 1, player.getPos().y};
            	player.checkAndMove(checkPos, map); 
            }
			//down
            if(ch == 's')
            {
                player.direction = 2;
            	Position checkPos = {player.getPos().x, player.getPos().y + 1};
            	player.checkAndMove(checkPos, map);
            }
			//right
            if(ch == 'd')
            {
                player.direction = 4;
            	Position checkPos = {player.getPos().x + 1, player.getPos().y};
            	player.checkAndMove(checkPos, map);      
            }                
			//shoot
            if(ch == 'e')
            {
                player.playerShoot(map); 
                player.printStatus();
                cursorTo(player.getPos().x, player.getPos().y);
            }
        	//quit
            if(ch == 'q')
            {
            	gameRunning = 0;
                system("cls");
    			cout << "GAME OVER!" << "\n" ;
            }
            if(ch == 'p')//
            {
            	win = true;
            	gameRunning = 0;
                system("cls");
            }//
        }
        //game over
        if (player.getPlayerHP() <= 0)
        {
        	PlaySound(nullptr, nullptr, 0);
        	//lose
        	cursorTo(player.getPos().x, player.getPos().y);
        	Sleep(1000);
        	SetConsoleTextAttribute(hConsole, playerColor);
        	cout << "*";
        	cursorTo(player.getPos().x, player.getPos().y);
        	Sleep(1000);
        	cout << ".";
        	cursorTo(player.getPos().x, player.getPos().y);
        	Sleep(1000);
        	cout << " ";
        	SetConsoleTextAttribute(hConsole, 15);
        	cursorTo(player.getPos().x, player.getPos().y);
        	Sleep(2000);
        	gameRunning = -1;
		}
		else if (win)
		{
			PlaySound(nullptr, nullptr, 0);
			//winning
			gameRunning = 2;
		}
    }
	if (gameRunning == -1)
    {
    	thread t3(playLose);//
    	losing();
    	t3.join();//
    	t1.join();
		t2.join();
		main();
		return 0;
	}
	else if(win)
	{
		if (modeNum == 1)
		{
			thread t4(playWin);//
			winning(startTime);
			t1.join();
			t2.join();
			t4.join();
			return 0;
		}
		else
		{
			thread t4(playWin);//
			winning(startTime);
			t1.join();
			t2.join();
			t4.join();
			return 0;
		}
	}
	t1.join();
	t2.join();
	

    return 0;
}

//body
//other
void cursorTo(int x, int y)
{
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
    return;
}
bool meetWall(Position checkPos, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	if (map[checkPos.y][checkPos.x] == 1 || map[checkPos.y][checkPos.x] == 5)
	{
		return true;
	}
	return false;
}
void delayPrint(string s)
{
	system("cls");
	int i = 0, timeCnt = 0;
	while(i < s.length())
	{
		//if print white space(unsure)
		if (s[i] == ' ')
		{
			cout << s[i];
			i++;
			continue;
		}
		//skip
		if (kbhit())
		{
			int ch = getch();
			if (ch == 32)
			{
				system("cls");
				cout << s;
				Sleep(1000);
				break;
			}
		}
		//wait
		if (timeCnt < 5000)
		{
			timeCnt++;
			continue;
		}
		//print
		cout << s[i];
		timeCnt = 0;
		//last print
		if (i == s.length() - 1)
			Sleep(500);
		i++;
	}
	cout << "\n" << "\n";
	cout << "                            press Space to continue...";
	while(true)
	{
		if (kbhit())
		{
			int ch = getch();
			if (ch == 32)
				break;
		}
	}
	return;
}


//before game
void menu()
{
    system("cls");
    int position = 1;
    bool change = false;

	SetConsoleTextAttribute(hConsole, 13);
	cursorTo(xStart - 13, yStart);
	cout << " _   _ _____ _____  _   _ ________  ___  ___  ______ _____ \n";
	Sleep(300);
	SetConsoleTextAttribute(hConsole, 14);
	cursorTo(xStart - 13, yStart + 1);
	cout << "| \\ | |_   _|  __ \\| | | |_   _|  \\/  | / _ \\ | ___ \\  ___| \n";
	Sleep(300);
	SetConsoleTextAttribute(hConsole, 15);
	cursorTo(xStart - 13, yStart + 2);
	cout << "|  \\| | | | | |  \\/| |_| | | | | .  . |/ /_\\ \\| |_/ / |__ \n";
	Sleep(300);
	SetConsoleTextAttribute(hConsole, 14);
	cursorTo(xStart - 13, yStart + 3);
	cout << "| . ` | | | | | __ |  _  | | | | |\\/| ||  _  ||    /|  __| \n";
	Sleep(300);
	SetConsoleTextAttribute(hConsole, 15);
	cursorTo(xStart - 13, yStart + 4);
	cout << "| |\\  |_| |_| |_\\ \\| | | | | | | |  | || | | || |\\ \\| |___ \n";
	Sleep(300);
	SetConsoleTextAttribute(hConsole, 14);
	cursorTo(xStart - 13, yStart + 5);
	cout << "\\_| \\_/\\___/ \\____/\\_| |_/ \\_/ \\_|  |_/\\_| |_/\\_| \\_\\____/ \n";
	Sleep(2000);
	SetConsoleTextAttribute(hConsole, 15);	

	system("cls");
                                                                                                       
    cursorTo(xStart, yStart);
    cout << "    --> PLAY <--" << "\n";   

    cursorTo(xStart, yStart + yGap);
    cout << "        MODE " << "\n";   

    cursorTo(xStart, yStart + yGap * 2);
    cout << "      SETTINGS" << "\n";           

    while(true)
    {
        if(kbhit())
        {
            int ch = getch();
			
			//recording
            if(ch == 'w' && position != 1)
            {
                position -= 1;
                change = true;
            }
            else if(ch == 's' && position != 3)
            {
                position += 1;
                change = true;
            }
			
			//selecttion
            if(ch == 32 && position == 1)
                return;
            else if(ch == 32 && position == 2)
                mode();
            else if(ch == 32 && position == 3)
                settings();
			
			//selecting
            if(position == 1 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "    --> PLAY <--" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "        MODE" << "\n"; 
                cursorTo(xStart, yStart + yGap * 2);
                cout << "      SETTINGS" << "\n";  
                bool change = false;
            }
            else if(position == 2 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "        PLAY" << "\n"; 
                cursorTo(xStart, yStart + yGap);
                cout << "    --> MODE <--" << "\n"; 
                cursorTo(xStart, yStart + yGap * 2);
                cout << "      SETTINGS" << "\n";       
                bool change = false;
            }
            else if(position == 3 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "        PLAY" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "        MODE" << "\n";   
                cursorTo(xStart, yStart + yGap * 2);
                cout << "  --> SETTINGS <--" << "\n";  
                bool change = false;
            }
        }
    }
}

void mode()
{
    system("cls");
    int position = 1;
    bool change = false;  
    
    if(modeNum == 1)
    {
        cursorTo(xStart, yStart);
        cout << "   --> *EASY* <--" << "\n";   
        cursorTo(xStart, yStart + yGap);
        cout << "        HARD " << "\n";  
        cursorTo(xStart, yStart + yGap * 2);   
        cout << "    BACK TO MENU " << "\n";    
    }
    else if(modeNum == 2)
    {
        position = 2;
        cursorTo(xStart, yStart);
        cout << "        EASY " << "\n";   
        cursorTo(xStart, yStart + yGap);
        cout << "   --> *HARD* <--" << "\n";  
        cursorTo(xStart, yStart + yGap * 2);   
        cout << "    BACK TO MENU " << "\n";            
    }
      
    while(true)
    {
        if(kbhit())
        {
            int ch = getch();

            if(ch == 'w' && position != 1)
            {
                position -= 1;
                change = true;
            }
            else if(ch == 's' && position != 3)
            {
                position += 1;
                change = true;
            }

            if(ch == 32 && position == 1)
            {
                modeNum = 1;
                change = true;
            }
            else if(ch == 32 && position == 2)
            {
                modeNum = 2;
                change = true;
            }
            else if(ch == 32 && position == 3)
            {
                return;
            }

            if(position == 1 && modeNum == 1 && change == true)
            {
                system("cls");
        		cursorTo(xStart, yStart);
                cout << "   --> *EASY* <--" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "        HARD" << "\n";  
                cursorTo(xStart, yStart + yGap * 2);   
                cout << "    BACK TO MENU " << "\n";   
                bool change = false;
            }
            else if(position == 2 && modeNum == 1 && change == true)
            {
                system("cls");
        		cursorTo(xStart, yStart);
                cout << "       *EASY*" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "    --> HARD <--" << "\n";      
                cursorTo(xStart, yStart + yGap * 2);   
                cout << "    BACK TO MENU " << "\n";
                bool change = false;
            }
            else if(position == 1 && modeNum == 2 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "    --> EASY <--" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "       *HARD*" << "\n";   
                cursorTo(xStart, yStart + yGap * 2);   
                cout << "    BACK TO MENU " << "\n";
                bool change = false;
            }
            else if(position == 2 && modeNum == 2 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "        EASY"  << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "   --> *HARD* <--" << "\n";   
                cursorTo(xStart, yStart + yGap * 2);   
                cout << "    BACK TO MENU " << "\n";
                bool change = false;
            }
            else if(position == 3 && modeNum == 1 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "       *EASY* " << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "        HARD" << "\n";  
                cursorTo(xStart, yStart + yGap * 2);   
                cout << "--> BACK TO MENU <--" << "\n";   
                bool change = false;
            }
            else if(position == 3 && modeNum == 2 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "        EASY"  << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "       *HARD*" << "\n";  
                cursorTo(xStart, yStart + yGap * 2);   
                cout << "--> BACK TO MENU <--" << "\n";    
                bool change = false;
            }            
        }
    }    
}

void settings()
{
    system("cls");

    int position = 1;
    bool change = false;
    bool mute = false;
    
    cursorTo(xStart, yStart);
    cout << "  --> SOUND ON <--" << "\n";   

    cursorTo(xStart, yStart + yGap);
    cout << "    BACK TO MENU " << "\n";           

    while(true)
    {
        if(kbhit())
        {
            int ch = getch();

            if(ch == 'w' && position != 1)
            {
                position -= 1;
                change = true;
            }
            else if(ch == 's' && position != 2)
            {
                position += 1;
                change = true;
            }

            if(ch == 32 && position == 1)
            {
                mute = !mute;
                change = true;
            }
            else if(ch == 32 && position == 2)
            {
                return;
            }

            
            if(position == 1 && change == true && mute == false)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "  --> SOUND ON <--" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "    BACK TO MENU" << "\n";     
                bool change = false;
            }
            else if(position == 2 && change == true && mute == false)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "      SOUND ON" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "--> BACK TO MENU <--" << "\n";        
                bool change = false;
            }
            else if(position == 1 && change == true && mute == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "  --> SOUND OFF <--" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "    BACK TO MENU" << "\n";     
                bool change = false;
            }
            else if(position == 2 && change == true && mute == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "      SOUND OFF" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "--> BACK TO MENU <--" << "\n";        
                bool change = false;
            }
        }
    }
}    


//in game
void shoot(Position startPos, Position dir, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH], int range)
{
	bool metItem = false;
    for(int i = 0; i < range; i++)
    {
    	Position temp = {0};
    	temp = startPos + dir;
        //if front is blank
        if(map[temp.y][temp.x] == 0)
        {
        	startPos = temp;
            cursorTo(startPos.x, startPos.y);
            cout << "o";
            if(i != 0 && !metItem)
            {
                Sleep(100);
                cursorTo(startPos.x - dir.x, startPos.y - dir.y);
                cout << " ";                   
            }
            metItem = false;
        }
        //if front is enemy
        else if (map[temp.y][temp.x] > 90)
        {
        	if (i == 0)
        	{
        		map[temp.y][temp.x] = -99;
			}
        	else if(i != 0)
			{
				if (!metItem)
				{
					cursorTo(startPos.x, startPos.y);
	        		cout << " ";
				}
	        	map[temp.y][temp.x] = -99;
			}
        	return;
		}
		else if (map[temp.y][temp.x] <= 4 && map[temp.y][temp.x] >= 2)
		{
			startPos = temp;
            if(i != 0 && !metItem)
            {
        		Sleep(100);
                cursorTo(startPos.x - dir.x, startPos.y - dir.y);
                cout << " ";                   
            }
            metItem = true;
		}
        //if front is wall
        else if(map[temp.y][temp.x] == 1)
            break;
        //if front is movable wall?
        else if(map[temp.y][temp.x] == 5)//
        {
            if (map[startPos.y][startPos.x] != -1 && !metItem)
            {
            	cursorTo(startPos.x, startPos.y);
        		cout << " ";  
			}
        	map[temp.y][temp.x] = 0;
        	startPos = temp;
			cursorTo(startPos.x, startPos.y);
    		cout << " ";
			return;
		}
    }
    //if bullet didn move and it didn't meet item, clear the bullet
    if(map[startPos.y][startPos.x] != -1 && !metItem)
    {
        Sleep(100);
        cursorTo(startPos.x, startPos.y);
        cout << " ";                 
    }
    return;
}

void printInfo()
{
    SetConsoleTextAttribute(hConsole, 15);

    cursorTo(MAP_SIZE_WIDTH + 3, 5);
    cout << "MOVE UP: " << "W" << "\n";

    cursorTo(MAP_SIZE_WIDTH + 3, 6);
    cout << "MOVE LEFT: " << "A" << "\n";

    cursorTo(MAP_SIZE_WIDTH + 3, 7);
    cout << "MOVE DOWN: " << "S" << "\n";

    cursorTo(MAP_SIZE_WIDTH + 3, 8);
    cout << "MOVE RIGHT: " << "D" << "\n";

    cursorTo(MAP_SIZE_WIDTH + 3, 10);
    cout << "SHOOT: " << "E" << "\n"; 

    cursorTo(MAP_SIZE_WIDTH + 3, 12);
    cout << "QUIT: " << "Q" << "\n";   
	
	cursorTo(MAP_SIZE_WIDTH + 3, 14);
	SetConsoleTextAttribute(hConsole, mWallColor);
    cout << "MOVABLE BLOCK: " << "Q";   
	
	cursorTo(MAP_SIZE_WIDTH + 3, 16);
	SetConsoleTextAttribute(hConsole, 9);
    cout << "NOOB ENEMY: " << "@";   
	
	cursorTo(MAP_SIZE_WIDTH + 3, 18);
	SetConsoleTextAttribute(hConsole, 6);
    cout << "NORMAL ENEMY: " << "@";   
	
	cursorTo(MAP_SIZE_WIDTH + 3, 20); 
	SetConsoleTextAttribute(hConsole, 4);      
    cout << "ELITE ENEMY: " << "@";// << "\n"
    
    cursorTo(MAP_SIZE_WIDTH + 3, 2);  
	SetConsoleTextAttribute(hConsole, 13);     
    cout << "Can you wake"; 
    cursorTo(MAP_SIZE_WIDTH + 7, 3);    
    cout << "yourself up?";
    
    SetConsoleTextAttribute(hConsole, 15);

	int gap = 0;

    SetConsoleTextAttribute(hConsole, 13);

	cursorTo(xStart - 13, MAP_SIZE_HEIGHT + gap);
	cout << " _   _ _____ _____  _   _ ________  ___  ___  ______ _____ \n";

	SetConsoleTextAttribute(hConsole, 14);
	cursorTo(xStart - 13, MAP_SIZE_HEIGHT + gap + 1);
	cout << "| \\ | |_   _|  __ \\| | | |_   _|  \\/  | / _ \\ | ___ \\  ___| \n";

	SetConsoleTextAttribute(hConsole, 15);
	cursorTo(xStart - 13, MAP_SIZE_HEIGHT + gap + 2);
	cout << "|  \\| | | | | |  \\/| |_| | | | | .  . |/ /_\\ \\| |_/ / |__ \n";
	
	SetConsoleTextAttribute(hConsole, 14);
	cursorTo(xStart - 13, MAP_SIZE_HEIGHT + gap + 3);
	cout << "| . ` | | | | | __ |  _  | | | | |\\/| ||  _  ||    /|  __| \n";
	
	SetConsoleTextAttribute(hConsole, 15);
	cursorTo(xStart - 13, MAP_SIZE_HEIGHT + gap + 4);
	cout << "| |\\  |_| |_| |_\\ \\| | | | | | | |  | || | | || |\\ \\| |___ \n";
	
	SetConsoleTextAttribute(hConsole, 14);
	cursorTo(xStart - 13, MAP_SIZE_HEIGHT + gap + 5);
	cout << "\\_| \\_/\\___/ \\____/\\_| |_/ \\_/ \\_|  |_/\\_| |_/\\_| \\_\\____/ \n";
	
	SetConsoleTextAttribute(hConsole, 15);	

}

int direction(Position from, Position to)//must be at different place
{
	if (from.x - to.x == 0)
	{
		//go up
		if (from.y - to.y > 0)
			return 1;
		//go down
		else if (from.y - to.y < 0)
			return 2;
	}
	else
	{
		//go left
		if (from.x - to.x > 0)
			return 3;
		//go right
		else if (from.x - to.x < 0)
			return 4;
	}
	return 0;
}

void printMap(int direction, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])//clean screen first? then I can use it in enemyMeetItem
{
    for(int i = 0; i < MAP_SIZE_HEIGHT; i++)
    {
        for(int j = 0; j < MAP_SIZE_WIDTH; j++)
        {
            if(map[i][j] == 0)
                cout << " ";
            else if(map[i][j] == 1)//wall
            {
                SetConsoleTextAttribute(hConsole, wallColor);
                cout << "#";
                SetConsoleTextAttribute(hConsole, 15);
            }
            else if(map[i][j] == 2)//bullet
            {
                SetConsoleTextAttribute(hConsole, BulColor);
                cout << "B";
                SetConsoleTextAttribute(hConsole, 15);
            }
            else if(map[i][j] == 3)//recover
            {   
                SetConsoleTextAttribute(hConsole, hpColor);
                cout << "H";
                SetConsoleTextAttribute(hConsole, 15);
            }
            else if(map[i][j] == 4)//score(money)
            {   
                SetConsoleTextAttribute(hConsole, scoreColor);
                cout << "$";
                SetConsoleTextAttribute(hConsole, 15);
            }
            else if(map[i][j] == 5) //movable object
            {
                SetConsoleTextAttribute(hConsole, mWallColor);
                cout << "#";
                SetConsoleTextAttribute(hConsole, 15);
            }
            else if(map[i][j] == -1)//character direction indication
            {
                SetConsoleTextAttribute(hConsole, playerColor);

                if(direction == 1)//up
                    cout << "^";
                else if(direction == 2)//down
                    cout << "v";
                else if(direction == 3)//left
                    cout << "<";
                else if(direction == 4)//right
                    cout << ">";

                SetConsoleTextAttribute(hConsole, 15);
            }
            else if(map[i][j] == -2) //end
            {
                SetConsoleTextAttribute(hConsole, goalColor);
                cout << "X";
                SetConsoleTextAttribute(hConsole, 15);
            }
			else if(map[i][j] > 90)
			{
				SetConsoleTextAttribute(hConsole, enemyColor);
                cout << "@";
                SetConsoleTextAttribute(hConsole, 15);
			}
        }
        cout << "\n";
    }    
}


//after game
void gameEnding()
{
    system("cls");
    cout << "GAME OVER!" << "\n" ;
    cout << "YOUR SCORE: " << score << "\n";
	return;
}
void winning(int startTime)
{
	clock_t endTime = clock();
	string line1 = " Congratulations! You Won!\n Let's see how much score you got!";
	delayPrint(line1);
	string line2 = " so... the result is...!";
	delayPrint(line2);
	gameEnding();
	cout << "PLAYING TIME: " << (endTime / CLOCKS_PER_SEC) - (startTime / CLOCKS_PER_SEC) << "s" << "\n";
	for (int i = 0; i < 100000; i++)
	{
		if (kbhit())
		{
			int ch = getch();
			if (ch == 32)
				break;
		}
	}
	return;
}
void losing()
{
	//	PlaySound(nullptr, nullptr, 0);
	string line1 = " You lose!\n But that's okay.\n Because you will try again, right?\n";
	delayPrint(line1);
	string line2 = " ......\n (there should be no score board since you have losen.)\n (but you can try again.)";
	delayPrint(line2);
	return;
}















