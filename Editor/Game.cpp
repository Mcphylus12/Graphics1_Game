#include <Windows.h>
#include <math.h> 
#include <stdio.h>
#include <stdlib.h>

#include "Image_Loading\nvImage.h"
#include "Game.h"


HDC			hDeviceContext;
HGLRC		GLRenderContext;		
HWND		hWindow;		
HINSTANCE	hInstance;

Game* game;

/*
debug test code

			char msgBuf[50];
			sprintf(msgBuf, " %f, %f ", vars);
			OutputDebugString(msgBuf);
*/
LRESULT CALLBACK WindowProc(
					       HWND   hWindow,
					       UINT   message,
					       WPARAM wParam,
					       LPARAM lParam
						   )
{
	switch (message)									// Check For Windows Messages
	{
		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}
		break;

		case WM_SIZE:								// Resize The OpenGL Window
		{
			resize(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
		break;

		case WM_LBUTTONDOWN:
			{
				SetCursor(NULL);
				cycleTile(game, game->mouseXPos, game->mouseYPos);
			}
		break;

		case WM_LBUTTONUP:
			{
			}
		break;

		case WM_MOUSEMOVE:
			{

				game->mouseXPos = (LOWORD(lParam) - windowWidth/2)/camZoom + camX;
				game->mouseYPos = ((windowHeight - HIWORD(lParam)) - windowHeight/2)/camZoom + camY;;
			}
		break;
		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			game->keys[wParam] = true;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}
		break;
		case WM_KEYUP:								// Has A Key Been Released?
		{
			game->keys[wParam] = false;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}
		break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWindow,
					     message,
					     wParam,
					     lParam);
}

int CALLBACK WinMain(
					HINSTANCE hInstance1,
					HINSTANCE hPrevInstance,
					LPSTR     lpCmdLine,
					int       nCmdShow
					)
{
	windowWidth = 800;
	windowHeight = 600;
	camZoom = 2;
	hInstance = hInstance1;
	LPCTSTR className = "GraphicsGame";
	WNDCLASS wc = {};
	wc.style = CS_OWNDC|CS_VREDRAW|CS_HREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = className;
	wc.hCursor = NULL;

	if(!RegisterClass(&wc))
	{
		killWindow(className);
		MessageBox(NULL, "Couldnt Register Class", "Error", MB_OK);
		return 1;
	}

	int windowStatus = createGLWindow(hInstance, className);
	if(!windowStatus)
	{
		return 1;
	}

	

	double dx = 0;
	double dy = 0;
	double maxSpeed = 5;
	double accel = 0.1f;
	MSG message;
	game = (Game*)calloc(1, sizeof(Game));
	initGL();
	initGame(game);
	int counter = 0;
	LARGE_INTEGER cps;
	LARGE_INTEGER curCount;
	LARGE_INTEGER prevCount;
	LONGLONG countDifference;

	QueryPerformanceFrequency(&cps);
	double secsPassed = 0;
	QueryPerformanceCounter(&curCount);
	float pan = 0.2;
	float topPan = 1;
	float lowPan = 0.2;
	while(!game->done)
	{
		prevCount = curCount;
		QueryPerformanceCounter(&curCount);
		countDifference = curCount.QuadPart - prevCount.QuadPart;
		secsPassed = (long double)countDifference / (long double)cps.QuadPart;
		while(PeekMessage(&message, hWindow, 0, 0, PM_REMOVE))
		{
			if(message.message == WM_QUIT)
			{
				game->done = true;
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		dx = 0;
		dy = 0;
		if(game->keys[VK_SHIFT])
		{
			pan = topPan;
		}
		else
		{
			pan = lowPan;
		}
		if(game->keys['W'])
		{
			camY+=pan;
		}
		if(game->keys['A'])
		{
			camX-=pan;
		}
		if(game->keys['S'])
		{
			camY-=pan;
		}
		if(game->keys['D'])
		{
			camX+=pan;
		}
		
		if(game->keys['Q'])
		{
			camZoom += 0.01;
			if(camZoom > 6) camZoom = 6;
			resize(windowWidth, windowHeight);
		}
		if(game->keys['E'])
		{
			camZoom -= 0.01;
			if(camZoom < 1) camZoom = 1;
			resize(windowWidth, windowHeight);
		}
		if(game->keys[VK_SPACE])
		{
			saveFile(game);
			
		}
		
		
		



		updateGame(game, dx, dy, secsPassed);
		
		drawScene(game);
		SwapBuffers(hDeviceContext);

	}
	free(game);
	killWindow(className);
	return 0;
}

void cycleTile(Game* g, float x, float y)
{
	Tile* t;
	if(!(x < 0 || y < 0))
	{
		t = &g->curTileMap->tiles[(int)y/TILESIDE][(int)x/TILESIDE];
		t->tileID++;
		if(t->tileID == 6)
			t->tileID = 0;
		refreshTileMap(g);
	}

}

void saveFile(Game* g)
{
	FILE* savefile;

	fopen_s(&savefile, "newLevel.txt", "w");
	if(savefile != NULL)
	{
		for (int j = 0; j < TILEMAPSIDE; j++)
		{

					fprintf_s(savefile, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", 
					g->curTileMap->tiles[j][0].tileID, g->curTileMap->tiles[j][1].tileID, 
					g->curTileMap->tiles[j][2].tileID, g->curTileMap->tiles[j][3].tileID,
					g->curTileMap->tiles[j][4].tileID, g->curTileMap->tiles[j][5].tileID, 
					g->curTileMap->tiles[j][6].tileID, g->curTileMap->tiles[j][7].tileID, 
					g->curTileMap->tiles[j][8].tileID, g->curTileMap->tiles[j][9].tileID, 
					g->curTileMap->tiles[j][10].tileID, g->curTileMap->tiles[j][11].tileID, 
					g->curTileMap->tiles[j][12].tileID, g->curTileMap->tiles[j][13].tileID, 
					g->curTileMap->tiles[j][14].tileID, g->curTileMap->tiles[j][15].tileID,
					g->curTileMap->tiles[j][16].tileID, g->curTileMap->tiles[j][17].tileID, 
					g->curTileMap->tiles[j][18].tileID, g->curTileMap->tiles[j][19].tileID,
					g->curTileMap->tiles[j][20].tileID, g->curTileMap->tiles[j][21].tileID, 
					g->curTileMap->tiles[j][22].tileID, g->curTileMap->tiles[j][23].tileID, 
					g->curTileMap->tiles[j][24].tileID, g->curTileMap->tiles[j][25].tileID, 
					g->curTileMap->tiles[j][26].tileID, g->curTileMap->tiles[j][27].tileID, 
					g->curTileMap->tiles[j][28].tileID, g->curTileMap->tiles[j][29].tileID, 
					g->curTileMap->tiles[j][30].tileID, g->curTileMap->tiles[j][31].tileID);

			
		
		}
		fclose(savefile);
	}

}

void refreshTileMap(Game* g)
{
	memset(g->doors, 0, g->doorArraySize * sizeof(door));
	memset(g->enemies, 0, g->enemyArraySize * sizeof(enemy));
	memset(g->gameKeys, 0, g->keyArraySize * sizeof(key));
	memset(g->shots, 0, g->shotArraySize * sizeof(shot));
	memset(&g->player1, 0, sizeof(player));
	memset(&g->stairs, 0, sizeof(stair));

	Tile* t;
	for (int i = 0; i < TILEMAPSIDE; i++)
	{
		for (int j = 0; j < TILEMAPSIDE; j++)
		{
			t = &g->tileMaps[0].tiles[i][j];
			switch (t->tileID)
			{

			case Tile::FLOOR:
				t->filled = FALSE;
				t->texID = g->floorTex;
				break;
			case Tile::WALL:
				t->filled = TRUE;
				t->texID = g->wallTex;
				break;
			case Tile::ENEMY_FLOOR:
				t->filled = false;
				t->texID = g->floorTex;
				//enemy spawn
				addEnemy(g, TILESIDE*(j + 0.5), TILESIDE*(i + 0.5), g->enemyTex);
				break;
			case Tile::PLAYER_FLOOR:
				t->filled = false;
				t->texID = g->floorTex;
				//player spawn
				setPlayer(g, TILESIDE*(j + 0.5) , TILESIDE*(i + 0.5), g->playerTex);
				break;
			case Tile::STAIR:
				t->filled = false;
				t->texID = g->floorTex;
				setStair(g, TILESIDE*(j + 0.5), TILESIDE*(i + 0.5), g->stairTex); 
				break;
			case Tile::DOOR:
				t->filled = false;
				t->texID = g->floorTex;
				addDoor(g, TILESIDE*(j + 0.5), TILESIDE*(i + 0.5), g->doorTex);
				break;
			}
		}
	}
	printf("");

}

int createGLWindow(HINSTANCE hInstance, LPCTSTR className)
{
		hWindow = CreateWindow(
							className,
							"GraphicsGame",
							WS_OVERLAPPEDWINDOW|WS_VISIBLE,
							CW_USEDEFAULT, CW_USEDEFAULT,
							windowWidth, windowHeight,	
							NULL,
							NULL,
							hInstance,
							NULL
							);
		if(!hWindow)
		{
		killWindow(className);
			MessageBox(hWindow, "Couldnt Create Window", "Error", MB_OK);
			return false;
		}
		hDeviceContext = GetDC(hWindow);
		if(!hDeviceContext)
		{
			killWindow(className);
			MessageBox(hWindow, "Could Create Device Context", "Error", MB_OK);
			return false;
		}
		PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cDepthBits = 24;
		pfd.iLayerType = PFD_MAIN_PLANE;
		int pixelFormat = ChoosePixelFormat(hDeviceContext, &pfd);
		if(!pixelFormat)
		{
			killWindow(className);
			MessageBox(hWindow, "Couldnt Select Good Pixel Format", "Error", MB_OK);
			return false;
		}
		if(!SetPixelFormat(hDeviceContext, pixelFormat, &pfd))
		{
			killWindow(className);
			MessageBox(hWindow, "Couldnt Select Good Pixel Format", "Error", MB_OK);
			return false;
		}
		GLRenderContext = wglCreateContext(hDeviceContext);
		if(!GLRenderContext)
		{
			killWindow(className);
			MessageBox(hWindow, "Couldnt make opengl context", "Error", MB_OK);
			return false;
		}
		if(!wglMakeCurrent(hDeviceContext, GLRenderContext))
		{
			killWindow(className);
			MessageBox(hWindow, "Couldnt make gl context current", "Error", MB_OK);
			return false;
		}

		return true;
}

void initGL()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-windowWidth/(camZoom*2), windowWidth/(camZoom*2), -windowHeight/(camZoom*2), windowHeight/(camZoom*2));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0, 0, 0, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initGame(Game* g)
{
	camX = 0;
	camY = 0;
	g->floorTex = loadPNG("metal_plates.png");
	g->wallTex = loadPNG("wall.png");
	g->shotTex = loadPNG("shot.png");
	g->playerTex = loadPNG("player1.png");
	g->enemyTex = loadPNG("enemy1.png");
	g->doorTex = loadPNG("door.png");
	g->stairTex = loadPNG("stairs.png");
	g->keyTex = loadPNG("key.png");
	g->curTileMap = g->tileMaps;
	getBlankTileMap(g);




}

void getBlankTileMap(Game* g)
{


	Tile* t;
	for (int i = 0; i < TILEMAPSIDE; i++)
	{
		for (int j = 0; j < TILEMAPSIDE; j++)
		{
			t = &g->tileMaps[0].tiles[i][j];
			if(i == 0 || j ==0 || j == TILEMAPSIDE-1 || i == TILEMAPSIDE-1)
			{
				t->tileID = 1;
				t->filled = true;
				t->texID = g->wallTex;
			}
			else
			{
				t->tileID = 0;
				t->filled = false;
				t->texID = g->floorTex;
			}

			
		}
	}
	printf("");
}


void updateGame(Game* g , double dx, double dy, long double time)
{

}

void resize(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-w/(camZoom*2), w/(camZoom*2), -h/(camZoom*2), h/(camZoom*2));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	windowHeight = h;
	windowWidth = w;
}

void killWindow(LPCTSTR className)
{
	if (GLRenderContext)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(GLRenderContext))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		GLRenderContext=NULL;										// Set RC To NULL
	}

	if (hDeviceContext && !ReleaseDC(hWindow, hDeviceContext))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDeviceContext=NULL;										// Set DC To NULL
	}

	if (hWindow && !DestroyWindow(hWindow))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWindow=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass(className, hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}


}

GLuint loadPNG(char* name)
{
	// Texture loading object
	nv::Image img;

	GLuint myTextureID;

	// Return true on success
	if(img.loadImageFromFile(name))
	{
		glGenTextures(1, &myTextureID);
		glBindTexture(GL_TEXTURE_2D, myTextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, img.getInternalFormat(), img.getWidth(), img.getHeight(), 0, img.getFormat(), img.getType(), img.getLevel(0));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	}

	else
		MessageBox(NULL, "Failed to load texture", "End of the world", MB_OK | MB_ICONINFORMATION);

	return myTextureID;
}

void drawScene(Game* g)
{
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
		entity* drawing = &g->player1;
		glTranslatef(-camX, -camY, 0);
			//draw world
		glPushMatrix();
			Tile* t;
			for(int i = 0; i < TILEMAPSIDE; i++)
			{
				glPushMatrix();
					for(int j = 0; j < TILEMAPSIDE; j++)
					{
						
						t = &g->curTileMap->tiles[i][j];
						glBindTexture(GL_TEXTURE_2D, t->texID);
						glLineWidth(2);
						fillRectangleFromTopLeft(TILESIDE, TILESIDE);
						if((int)g->mouseXPos / (int)TILESIDE == j && (int)g->mouseYPos / (int)TILESIDE == i)
						{
							glBindTexture(GL_TEXTURE_2D, 0);
							glBegin(GL_LINE_LOOP);
							glVertex2f(0, 0);
							glVertex2f(TILESIDE, 0);
							glVertex2f(TILESIDE, TILESIDE);
							glVertex2f(0, TILESIDE);
							glEnd();
						}
						glTranslatef(TILESIDE, 0, 0);
					}
				glPopMatrix();
				glTranslatef(0, TILESIDE, 0);
			}
		glPopMatrix();


		//draw shots
		for(int i = 0; i <= g->shotArraySize; i++)
		{
			glPushMatrix();
				drawing = &g->shots[i];
				if(&g->shots[i] != 0)
				{
					glTranslatef(drawing->x, drawing->y, 0);
					glBindTexture(GL_TEXTURE_2D, g->shotTex);
 					fillRectangleFromCentre(drawing->width, drawing->height);
				}
			glPopMatrix();
		}
		//draw doors
		for(int i = 0; i < g->doorArraySize; i++)
		{
			glPushMatrix();
				drawing = &g->doors[i];
				if(&g->doors[i] != 0)
				{

					glTranslatef(drawing->x, drawing->y, 0);
					glBindTexture(GL_TEXTURE_2D, g->doorTex);
					fillRectangleFromCentre(drawing->width, drawing->height);
				}
			glPopMatrix();
		}

		//draw stairs
		glPushMatrix();
			glTranslatef(g->stairs.x, g->stairs.y, 0);
			glBindTexture(GL_TEXTURE_2D, g->stairTex);
			fillRectangleFromCentre(g->stairs.width, g->stairs.height);
		glPopMatrix();

		// draw keys
		for(int i = 0; i <= g->keyArraySize; i++)
		{
			glPushMatrix();
				drawing = &g->gameKeys[i];
				if(&g->gameKeys[i] != 0)
				{
					glTranslatef(drawing->x, drawing->y, 0);
					glBindTexture(GL_TEXTURE_2D, g->keyTex);
					fillRectangleFromCentre(drawing->width, drawing->height);
				}
			glPopMatrix();
		}

		
		//draw enemies(push and pop per enemy)
		for(int i = 0; i <= g->enemyArraySize; i++)
		{
			glPushMatrix();
				drawing = &g->enemies[i];
				if(&g->enemies[i] != 0)
				{
					glTranslatef(drawing->x, drawing->y, 0);
					glBindTexture(GL_TEXTURE_2D, g->enemyTex);
 					fillRectangleFromCentre(drawing->width, drawing->height);
				}
			glPopMatrix();
		}

		drawing = &g->player1;
		glTranslatef(drawing->x, drawing->y, 0);
		
		glPushMatrix();
		//draw player
			glRotatef(-90 * drawing->facing, 0, 0, 1);
			glBindTexture(GL_TEXTURE_2D, g->playerTex);
			fillRectangleFromCentre(drawing->width, drawing->height);
		glPopMatrix();
		



	glPopMatrix();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glTranslatef(g->mouseXPos - camX, g->mouseYPos - camY, 0);
	fillRectangleFromCentre(2, 2);
}

void fillRectangleFromCentre(double w, double h)
{
	glBegin(GL_QUADS);
		glVertex2f( -w/2, -h/2);glTexCoord2f(1, 0);
		glVertex2f( w/2, -h/2);glTexCoord2f(1, 1);
		glVertex2f( w/2, h/2);glTexCoord2f(0, 1);
		glVertex2f( -w/2, h/2);glTexCoord2f(0, 0);
	glEnd();
}

void fillRectangleFromTopLeft(double w, double h)
{
	glBegin(GL_QUADS);
		glVertex2f(0, 0);glTexCoord2f(1, 0);
		glVertex2f( w, 0);glTexCoord2f(1, 1);
		glVertex2f( w, h);glTexCoord2f(0, 1);
		glVertex2f( 0, h);glTexCoord2f(0, 0);;
	glEnd();
}




void setPlayer(Game* g, double x, double y, int texID)
{
	g->player1.facing = entity::DOWN;
	g->player1.x = x;
	g->player1.y = y;
	g->player1.height = 10.0f;
	g->player1.width = 10.0f;
	g->player1.texID[0] = texID;
	g->player1.isValid = 1;
	g->player1.speed = 0;
	g->player1.shotCoolDown = 3;
	g->player1.curCoolDown = 0;
	g->player1.keyCount = 0;
}
void addEnemy(Game* g, double x, double y, int texID)
{
	int nextFree = 0;
	enemy* e = g->enemies;
	for (int i = 0; i < g->enemyArraySize; i++)
	{
			if (!e->isValid)
			{
				nextFree = i;
				break;
			}
		e++;
	}
	e = &g->enemies[nextFree];
	e->facing = entity::DOWN;
	e->x = x;
	e->y = y;
	e->height = 10.0f;
	e->width = 10.0f;
	e->texID[0] = texID;
	e->isValid = 1;
	e->speed = 3;

}
void addKey(Game* g, double x, double y, int texID)
{
	int nextFree = 0;
	key* e = g->gameKeys;
	for (int i = 0; i < g->keyArraySize; i++)
	{
			if (!e->isValid)
			{
				nextFree = i;
			}
		e++;
	}
	e = &g->gameKeys[nextFree];
	e->facing = entity::DOWN;
	e->x = x;
	e->y = y;
	e->height = 10.0f;
	e->width = 10.0f;
	e->texID[0] = texID;
	e->isValid = 1;
	e->speed = 0;
}
void addShot(Game* g, double x, double y, int texID, int facing)
{
	int nextFree = 0;
	shot* e = g->shots;
	for (int i = 0; i < g->shotArraySize; i++)
	{
			if (!e->isValid)
			{
				nextFree = i;
			}
		e++;
	}
	e = &g->shots[nextFree];
	e->facing = facing;
	e->x = x;
	e->y = y;
	e->height = 5.0f;
	e->width = 5.0f;
	e->texID[0] = texID;
	e->isValid = 1;
	e->speed = 10;
}
void removeEnemy(Game* g, enemy* e)
{
	memset(e, 0, sizeof(enemy));
}
void removeKey(Game* g, key* k)
{
	memset(k, 0, sizeof(key));
}
void removeShot(Game* g, shot* s)
{
	memset(s, 0, sizeof(shot));
}

void removeDoor(Game* g, door* d)
{
	memset(d, 0, sizeof(door));
}



void setStair(Game* g, double x, double y, int texID)
{
	g->stairs.x = x;
	g->stairs.y = y;
	g->stairs.texID = texID;
	g->stairs.width = TILESIDE;
	g->stairs.height = TILESIDE;
}

void addDoor(Game* g, double x, double y, int texID)
{
	int nextFree = 0;
	door* e = g->doors;
	for (int i = 0; i < g->doorArraySize; i++)
	{
			if (!e->isValid)
			{
				nextFree = i;
				break;
			}
		e++;
	}
	e = &g->doors[nextFree];
	e->facing = entity::DOWN;
	e->x = x;
	e->y = y;
	e->height = 15;
	e->width = 15;
	e->texID[0] = texID;
	e->isValid = 1;
	e->speed = 0;
}

door* collideDoors(Game* g, double x, double y, double w, double h)
{
	door* test = g->doors;
	for (int i = 0; i < g->doorArraySize; i++)
	{
		if(test->isValid)
		{
			if (!(y > test->y + test->height ||
				y + h < test->y ||
				x > test->x + test->width ||
				x + w < test->x))
			{
				return test;
			}
		}
		test++;
	}
	return 0;
}
bool collideStairs(Game* g, double x, double y, double w, double h)
{
	stair* test = &g->stairs;
	if (!(y > test->y + test->height ||
		y + h < test->y ||
		x > test->x + test->width ||
		x + w < test->x))
		{
			return true;
		}
	return false;
}
enemy* collideEnemies(Game* g, double x, double y, double w, double h)
{
	enemy* test = g->enemies;
	for (int i = 0; i < g->enemyArraySize; i++)
	{
		if(!(x == test->x || y == test->y))
		{
			if(test->isValid)
			{
				if (!(y > test->y + test->height ||
					y + h < test->y ||
					x > test->x + test->width ||
					x + w < test->x))
				{
					return test;
				}
			}
		}
		test++;
	}
	return 0;
}
bool collideShots(Game* g, double x, double y, double w, double h);	
bool collideMap(Game* g, double x, double y, double w, double h)
{
	return !((!g->curTileMap->tiles[(int)(y - h / 2) / TILESIDE][(int)(x - w / 2) / TILESIDE].filled) &&
		(!g->curTileMap->tiles[(int)(y + h / 2) / TILESIDE][(int)(x + w / 2) / TILESIDE].filled) &&
		(!g->curTileMap->tiles[(int)(y + h / 2) / TILESIDE][(int)(x - w / 2) / TILESIDE].filled) &&
			(!g->curTileMap->tiles[(int)(y - h / 2) / TILESIDE][(int)(x + w / 2) / TILESIDE].filled));
}
bool collidePlayer(Game* g, double x, double y, double w, double h)
{
	player* test = &g->player1;
	if(!(x == test->x || y == test->y))
	{
		if(test->isValid)
		{
			if (!(y > test->y + test->height ||
				y + h < test->y ||
				x > test->x + test->width ||
				x + w < test->x))
			{
				return true;
			}
		}
	}
	return 0 ;
}
key* collideKeys(Game* g, double x, double y, double w, double h)
{
	key* test = g->gameKeys;
	for (int i = 0; i < g->keyArraySize; i++)
	{
		if(!(x == test->x || y == test->y))
		{
			if(test->isValid)
			{
				if (!(y > test->y + test->height ||
					y + h < test->y ||
					x > test->x + test->width ||
					x + w < test->x))
				{
					return test;
				}
			}
		}
		test++;
	}
	return 0;
}



