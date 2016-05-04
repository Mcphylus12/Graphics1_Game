#define TILESIDE 16
#define TILEMAPSIDE 32

int windowWidth;
int windowHeight;
double camZoom;
struct Game;
float camX, camY;

class entity
{
public:
	int facing;
	const static int UP = 0;
	const static int LEFT = 3;
	const static int DOWN = 2;
	const static int RIGHT = 1;

	double x;
	double y;
	double height;
	double width;
	int texID[6];
	double speed;
	bool isValid;

};

class stair
{
public:
	double x;
	double y;
	double height;
	double width;
	int texID;
};

class door : public entity
{
	bool isOpen;
};

class shot : public entity
{
public:

};

class player : public entity
{
public:
	int keyCount;
	double shotCoolDown, curCoolDown;

};

class enemy : public entity
{

};

class key : public entity
{
	public:
};



struct Tile
{
	int tileID;
	const static int FLOOR = 0;
	const static int PLAYER_FLOOR = 3;
	const static int ENEMY_FLOOR = 2;
	const static int WALL = 1;
	const static int STAIR = 5;
	const static int DOOR = 4;
	static const int width = TILESIDE;
	static const int height = TILESIDE;
	bool filled;
	int texID;
};

struct Tilemap
{
	Tile tiles[TILEMAPSIDE][TILEMAPSIDE];
};

struct Game
{
	int floorTex;
	int wallTex;
	int shotTex;
	int playerTex;
	int enemyTex;
	int doorTex;
	int stairTex;
	int keyTex;
	bool done;
	bool keys[256];
	double mouseXPos;
	double mouseYPos;
	
	Tilemap tileMaps[16];
	Tilemap* curTileMap;
	int levelCount;
	int curLevel;

	stair stairs;

	door doors[200];
	const static int doorArraySize = 200;

	player player1;

	enemy enemies[200];
	const static int enemyArraySize = 200;

	key gameKeys[200];
	const static int keyArraySize = 200;

	shot shots[200];
	const static int shotArraySize = 200;


	
};

void saveFile(Game* g);
void refreshTileMap(Game* g);
void cycleTile(Game* g, float x, float y);
GLuint loadPNG(char* name);
void getBlankTileMap(Game* g);
void setStair(Game* g, double x, double y, int texID);
void setPlayer(Game* g, double x, double y, int texID);
void addDoor(Game* g, double x, double y, int texID);
void addEnemy(Game* g, double x, double y, int texID);
void addKey(Game* g, double x, double y, int texID);
void addShot(Game* g, double x, double y, int texID, int facing);
void removeEnemy(Game* g, enemy* e);
void removeKey(Game* g, key* k);
void removeShot(Game* g, shot* s);
void removeDoor(Game* g, door* d);
void fillRectangleFromTopLeft(double w, double h);
void fillRectangleFromCentre(double w, double h);
void killWindow(LPCTSTR className);
int createGLWindow(HINSTANCE hInstance, LPCTSTR className);
LRESULT CALLBACK WindowProc(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam);
void drawScene(Game* g);
void initGL();
void resize(int w, int h);
void initGame(Game* g);
void updateGame(Game* g, double dx, double dy, long double time);
door* collideDoors(Game* g, double x, double y, double w, double h);
bool collideStairs(Game* g, double x, double y, double w, double h);
enemy* collideEnemies(Game* g, double x, double y, double w, double h);
bool collideShots(Game* g, double x, double y, double w, double h);
bool collideMap(Game* g, double x, double y, double w, double h);
bool collidePlayer(Game* g, double x, double y, double w, double h);
key* collideKeys(Game* g, double x, double y, double w, double h);




