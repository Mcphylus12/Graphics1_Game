#include <Windows.h>
#include <math.h> 
#include <stdio.h>

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
			}
		break;

		case WM_LBUTTONUP:
			{
			}
		break;

		case WM_MOUSEMOVE:
			{
				game->mouseXPos = LOWORD(lParam);
				game->mouseYPos = HIWORD(lParam);
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
	camZoom = 4;
	hInstance = hInstance1;
	LPCTSTR className = "GraphicsGame";
	WNDCLASS wc = {};
	wc.style = CS_OWNDC|CS_VREDRAW|CS_HREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = className;

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

		if(game->keys['W'])
		{
			dy = 1.0f;
		}
		if(game->keys['A'])
		{
			dx = -1.0f;
		}
		if(game->keys['S'])
		{
			dy = -1.0f;
		}
		if(game->keys['D'])
		{
			dx = 1.0f;
		}
		if (game->keys['W'] || game->keys['A'] || game->keys['S'] || game->keys['D'])
		{
			
			if (game->player1.speed <= maxSpeed-accel)
			{
				game->player1.speed += accel;
			}
			else
			{
				game->player1.speed = maxSpeed;
			}
			
		}
		else
		{
			if (game->player1.speed >= accel)
			{
				game->player1.speed -= accel;
			}
			else
			{
				game->player1.speed = 0;
			}
			
		}




		updateGame(game, dx, dy, secsPassed);
		if (game->keys[VK_UP])
		{
			if(game->player1.curCoolDown ==0 )
			{
				addShot(game, game->player1.x, game->player1.y, game->shotTex, entity::UP);
				game->player1.curCoolDown = game->player1.shotCoolDown;
			}
			game->player1.facing = entity::DOWN;
		} 
		else if (game->keys[VK_LEFT])
		{
			if(game->player1.curCoolDown ==0 )
			{
				addShot(game, game->player1.x, game->player1.y, game->shotTex, entity::LEFT);
				game->player1.curCoolDown = game->player1.shotCoolDown;
			}
			game->player1.facing = entity::LEFT;

		}
		else if (game->keys[VK_RIGHT])
		{
			if(game->player1.curCoolDown ==0 )
			{
				addShot(game, game->player1.x, game->player1.y, game->shotTex, entity::RIGHT);
				game->player1.curCoolDown = game->player1.shotCoolDown;
			}
			game->player1.facing = entity::RIGHT;
		}
		else if (game->keys[VK_DOWN])
		{
			if(game->player1.curCoolDown ==0 )
			{
			addShot(game, game->player1.x, game->player1.y, game->shotTex, entity::DOWN);
			game->player1.curCoolDown = game->player1.shotCoolDown;
			}
			game->player1.facing = entity::UP;
		}
		drawScene(game);
		SwapBuffers(hDeviceContext);

	}
	free(game);
	killWindow(className);
	return 0;
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
	g->floorTex = loadPNG("metal_plates.png");
	g->wallTex = loadPNG("wall.png");
	g->shotTex = loadPNG("shot.png");
	g->playerTex[0] = loadPNG("player1.png");
	g->playerTex[1] = loadPNG("player2.png");
	g->playerTex[2] = loadPNG("player3.png");
	g->playerTex[3] = loadPNG("player4.png");
	g->playerTex[4] = loadPNG("player5.png");
	g->playerTex[5] = loadPNG("player6.png");
	g->playerTex[6] = loadPNG("player7.png");
	g->playerTex[7] = loadPNG("player8.png");
	g->enemyTex[0] = loadPNG("enemy1.png");
	g->enemyTex[1] = loadPNG("enemy2.png");
	g->enemyTex[2] = loadPNG("enemy3.png");
	g->enemyTex[3] = loadPNG("enemy4.png");
	g->enemyTex[4] = loadPNG("enemy5.png");
	g->enemyTex[5] = loadPNG("enemy6.png");
	g->enemyTex[6] = loadPNG("enemy7.png");
	g->enemyTex[7] = loadPNG("enemy8.png");
	g->doorTex = loadPNG("door.png");
	g->stairTex = loadPNG("stairs.png");
	g->keyTex = loadPNG("key.png");

	
	loadLevelSet(g, "levels.txt");


}

void setWorld(Game* g, int worldNumber)
{
	g->curTileMap = &g->tileMaps[worldNumber];
	g->curLevel = worldNumber;
	memset(g->doors, 0, sizeof(g->doorArraySize));
	memset(g->enemies, 0, sizeof(g->enemyArraySize));
	memset(g->gameKeys, 0, sizeof(g->keyArraySize));
	memset(g->shots, 0, sizeof(g->shotArraySize));


	Tile* t;
	for (int i = 0; i < TILEMAPSIDE; i++)
	{
		for (int j = 0; j < TILEMAPSIDE; j++)
		{
			t = &g->tileMaps[worldNumber].tiles[i][j];
			switch (t->tileID)
			{
			case Tile::ENEMY_FLOOR:
				//enemy spawn
				addEnemy(g, TILESIDE*(j + 0.5), TILESIDE*(i + 0.5), g->enemyTex[0]);
				break;
			case Tile::PLAYER_FLOOR:
				//player spawn
				setPlayer(g, TILESIDE*(j + 0.5) , TILESIDE*(i + 0.5), g->playerTex[0]);
				break;
			case Tile::STAIR:
				setStair(g, TILESIDE*(j + 0.5), TILESIDE*(i + 0.5), g->stairTex); 
				break;
			case Tile::DOOR:
				addDoor(g, TILESIDE*(j + 0.5), TILESIDE*(i + 0.5), g->doorTex);
				break;
			}
		}
	}
	printf("");
}

void readTileMap(Game* g, char* fileName, int worldNumber)
{
	FILE* tilemap;
	char buffer[255]; 
	int tileIDs[TILEMAPSIDE][TILEMAPSIDE];
	fopen_s(&tilemap, fileName, "r");
	if(tilemap != NULL)
	{
		for(int j = 0; j < TILEMAPSIDE; j++)
		{
			fgets(buffer, 255, tilemap);
			printf("");
			sscanf_s(buffer, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", 
				&tileIDs[j][0], &tileIDs[j][1], 
				&tileIDs[j][2], &tileIDs[j][3],
				&tileIDs[j][4], &tileIDs[j][5], 
				&tileIDs[j][6], &tileIDs[j][7], 
				&tileIDs[j][8], &tileIDs[j][9], 
				&tileIDs[j][10], &tileIDs[j][11], 
				&tileIDs[j][12], &tileIDs[j][13], 
				&tileIDs[j][14], &tileIDs[j][15],
				&tileIDs[j][16], &tileIDs[j][17], 
				&tileIDs[j][18], &tileIDs[j][19],
				&tileIDs[j][20], &tileIDs[j][21], 
				&tileIDs[j][22], &tileIDs[j][23], 
				&tileIDs[j][24], &tileIDs[j][25], 
				&tileIDs[j][26], &tileIDs[j][27], 
				&tileIDs[j][28], &tileIDs[j][29], 
				&tileIDs[j][30], &tileIDs[j][31]);
		}
		
		Tile* t;
		for(int i = 0; i < TILEMAPSIDE; i++)
		{
			for(int j = 0; j < TILEMAPSIDE; j++)
			{
				t = &g->tileMaps[worldNumber].tiles[i][j];
				t->tileID = tileIDs[i][j];
				switch(tileIDs[i][j])
				{
				case 0:
					t->filled = FALSE;
					t->texID = g->floorTex;
					break;
				case 1:
					t->filled = TRUE;
					t->texID = g->wallTex;
					break;
				case 2:
					//enemy spawn
					t->filled = false;
					t->texID = g->floorTex;
					break;
				case 3:
					//player spawn
					t->filled = false;
					t->texID = g->floorTex;
					break;
				case 4:
					//door
					t->filled = false;
					t->texID = g->floorTex;
					break;
				case 5:
					//stair
					t->filled = false;
					t->texID = g->floorTex;
					break;
				}
			}
		}
		fclose(tilemap);
	} else {
		OutputDebugString("Couldnt load File");
	}
}

void updateGame(Game* g , double dx, double dy, long double time)
{
	time *= 20;
	if(game->player1.speed != 0)
	{
		game->player1.move(game, dx, dy, time);
	}

	enemy* enemytest = g->enemies;
	for(int i = 0; i < g->enemyArraySize; i++)
	{
		if(enemytest->isValid)
		{
			enemytest->move(g, g->player1.x - enemytest->x, g->player1.y - enemytest->y, time);
		}
		enemytest++;

	}

	shot* shotTemp = g->shots;
	for(int i = 0; i < g->shotArraySize; i++)
	{
		if(shotTemp->isValid)
		{
			shotTemp->move(g, time);
		}
		shotTemp++;
	}



	if(g->player1.curCoolDown < 0)
	{
		g->player1.curCoolDown = 0;
	}
	if(g->player1.curCoolDown > 0)
	{
		g->player1.curCoolDown -= time;
	}

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
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
		entity* drawing = &g->player1;
		glTranslatef(-drawing->x, -drawing->y, 0);
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
						fillRectangleFromTopLeft(TILESIDE, TILESIDE);
					
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
					glRotatef(90 * drawing->facing, 0, 0, 1);
					glBindTexture(GL_TEXTURE_2D, g->enemyTex[(int)drawing->curAnimFrame]);
 					fillRectangleFromCentre(drawing->width, drawing->height);
				}
			glPopMatrix();
		}

		drawing = &g->player1;
		glTranslatef(drawing->x, drawing->y, 0);
		
		glPushMatrix();
		//draw player
			glRotatef(90 * drawing->facing, 0, 0, 1);
			glBindTexture(GL_TEXTURE_2D, g->playerTex[(int)drawing->curAnimFrame]);
			fillRectangleFromCentre(drawing->width, drawing->height);
		glPopMatrix();



	glPopMatrix();
}

void fillRectangleFromCentre(double w, double h, int animFrame)
{
	if(animFrame == 0)
	{
	glBegin(GL_QUADS);
		glVertex2f( -w/2, -h/2);glTexCoord2f(1, 0);
		glVertex2f( w/2, -h/2);glTexCoord2f(1, 1);
		glVertex2f( w/2, h/2);glTexCoord2f(0, 1);
		glVertex2f( -w/2, h/2);glTexCoord2f(0, 0);
	glEnd();
	}
	else
	{
	glBegin(GL_QUADS);
		glVertex2f( -w/2, -h/2);glTexCoord2f(1, 0);
		glVertex2f( w/2, -h/2);glTexCoord2f(1, 1);
		glVertex2f( w/2, h/2);glTexCoord2f(0, 1);
		glVertex2f( -w/2, h/2);glTexCoord2f(0, 0);
	glEnd();
	}


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
	g->player1.texID = texID;
	g->player1.isValid = 1;
	g->player1.speed = 0;
	g->player1.shotCoolDown = 3;
	g->player1.curCoolDown = 0;
	g->player1.keyCount = 0;
	g->player1.curAnimFrame = 0;
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
	e->texID = texID;
	e->isValid = 1;
	e->speed = 3;
	e->curAnimFrame = 0;

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
	e->texID = texID;
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
	e->texID = texID;
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

void player::move(Game* g, double dx, double dy,  double time)
{
	bool yMoveGood = false;
	bool xMoveGood = false;
	double newX;
	double newY;
	double length = sqrt(dx*dx + dy*dy);
	if(length){
		newX = x + (dx*speed*time/length);
		newY = y + (dy*speed*time/length);
	} 
	else 
	{
		newX = x;
		newY = y;
	}


	if (!collideMap(g, newX, y, width, height))
	{
		xMoveGood = true;

	}

	if (!collideMap(g, x, newY, width, height))
	{
		yMoveGood = true;

	}



	if(door* d = collideDoors(g, newX, y, width, height))
	{
		
		if(keyCount >= 5)
		{
			keyCount -= 5;
			removeDoor(g, d);
		}
		else 
		{
			xMoveGood = false;
		}
		
	}

	if(door* d = collideDoors(g, x, newY, width, height))
	{
		
		if(keyCount >= 5)
		{
			keyCount -= 5;
			removeDoor(g, d);
		}
		else 
		{
			yMoveGood = false;
		}
		
	}





	if(key* k = collideKeys(g, x, y, width, height))
	{
		keyCount++;
		removeKey(g, k);
	}

	if(yMoveGood)
	{
		if (newY - y > 0)
		{
			facing = entity::DOWN;
		}
		else if (newY - y < 0)
		{
			facing = entity::UP;
		}
		y = newY;
	}

	if(xMoveGood)
	{
		if (newX - x > 0)
		{
			facing = entity::RIGHT;
		} 
		else if (newX - x < 0)
		{
			facing = entity::LEFT;
		}
		x = newX;
		
	}

	if(xMoveGood || yMoveGood)
	{
		curAnimFrame += speed*time/4;
	}
	if(curAnimFrame > 8)
	{
		curAnimFrame = (int)curAnimFrame % 8;
	}
	if(collideStairs(g, x, y, width, height))
	{
		if(g->curLevel == g->levelCount)
		{
			MessageBox(NULL, "you win", "WINNNNNNERNERENRNERNERN", MB_OK);
			g->done = true;
		} 
		else
		{
			setWorld(g, g->curLevel + 1);
		}
	}


}


void enemy::move(Game* g, double dx, double dy,  double time)
{
	double newX;
	double newY;
	double length = sqrt(dx*dx + dy*dy);
	bool xMoveGood = false;
	bool yMoveGood = false;
	if(length){
		newX = x + (dx*speed*time/length);
		newY = y + (dy*speed*time/length);
	} 
	else 
	{
		newX = x;
		newY = y;
	}


	if (!collideMap(g, newX, y, width, height))
	{
		xMoveGood = true;

	}

	if (!collideMap(g, x, newY, width, height))
	{
		yMoveGood = true;

	}

	if (collideDoors(g, newX, y, width, height))
	{
		xMoveGood = false;

	}

	if (collideDoors(g, x, newY, width, height))
	{
		yMoveGood = false;

	}


	if (collideEnemies(g, newX, y, width, height))
	{
		xMoveGood = false;

	}

	if (collideEnemies(g, x, newY, width, height))
	{
		yMoveGood = false;

	}

	if(xMoveGood || yMoveGood)
	{
		if(abs(newX - x) > abs(newY - y))
		{
			if (newX - x > 0)
			{
				facing = entity::RIGHT;
			} 
			else if (newX - x < 0)
			{
				facing = entity::LEFT;
			}
		}
		else
		{
			if (newY - y > 0)
			{
				facing = entity::DOWN;
			}
			else if (newY - y < 0)
			{
				facing = entity::UP;
			}
		}
		curAnimFrame += speed*time/4;
	}

	if(yMoveGood)
	{

		y = newY;
	}

	if(xMoveGood)
	{

		x = newX;
		
	}


	if(curAnimFrame > 8)
	{
		curAnimFrame = (int)curAnimFrame % 8;
	}

	if(collidePlayer(g, x, y, width, height))
	{
		loseGame(g);
	}


}

void shot::move(Game* g, double time)
{
	double newX = x;
	double newY = y;
	if(facing == entity::UP)
	{
		newY = y + speed*time;
	}
	if(facing == entity::DOWN)
	{
		newY = y - speed*time;
	}
	if(facing == entity::LEFT)
	{
		newX = x - speed*time;
	}
	if(facing == entity::RIGHT)
	{
		newX = x + speed*time;
	}



	if (!collideMap(g, newX, newY, width, height))
	{
		if (newX - x > 0)
		{
			facing = entity::RIGHT;
		} 
		else if (newX - x < 0)
		{
			facing = entity::LEFT;
		}
		x = newX;

		if (newY - y > 0)
		{
			facing = entity::UP;
		}
		else if (newY - y < 0)
		{
			facing = entity::DOWN;
		}
		y = newY;

	}
	else
	{
		removeShot(g, this);
		return;
	}



		if (enemy* e = collideEnemies(g, newX, newY, width, height))
		{
			addKey(g, e->x, e->y, g->keyTex);
			removeEnemy(g, e);
			removeShot(g, this);
			return;
		}


}

void loadLevelSet(Game* g, char* fileName)
{
	memset(g->keys, 0, sizeof(g->keys));
	g->levelCount = 0;
	FILE* levelsetFile;
	char* nextToken;
	fopen_s(&levelsetFile, fileName, "r");
	if(levelsetFile)
	{
		char buffer[100];
		while(fgets(buffer, 100, levelsetFile))
		{
			readTileMap(g, strtok_s(buffer, "\n", &nextToken), g->levelCount);
			g->levelCount++;
		}
		setWorld(g, 0);
		fclose(levelsetFile);
	}
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
	e->texID = texID;
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
		if(!(abs(x - test->x) < 1 && abs(y - test->y) < 1) && test->isValid)
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
	return 0 ;
}
key* collideKeys(Game* g, double x, double y, double w, double h)
{
	key* test = g->gameKeys;
	for (int i = 0; i < g->keyArraySize; i++)
	{
		if(!(x == test->x && y == test->y))
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

void loseGame(Game* g)
{

	
	
	MessageBox(NULL, "you got hit son. unlucky. Game over", "things have not gone so well for you", MB_OK);
		g->done = true;
}


