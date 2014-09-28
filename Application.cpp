#include "Application.h"

Application::Application() {
	Init();
}

Application::~Application() {
	// check to make sure no memory leak
	for (int i = 0; i < MAX_PIKACHU - 1; ++i) { // clear memory for array of pikachu
		if (pikachuArray[i] != NULL) {
			delete pikachuArray[i];
			pikachuArray[i] = NULL;
		}
	}
	for (int j = 0; j < MAX_ONSCREEN - 1; ++j) { // clear memory for array of pokeballs
		if (proj[j] != NULL) {
			delete proj[j];
			proj[j] = NULL;
		}
	}
	for (int i = 0; i < MAX_ONSCREEN - 1; ++i) { // clear memory for array of lightningbolts
		if (lightningboltArray[i] != NULL) {
			delete lightningboltArray[i];
			lightningboltArray[i] = NULL;
		}
	}
	SDL_Quit();
}

void Application::Init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	srand(time(NULL));

	lastFrameTicks = 0.0f;
	mainMenuElapsed = 0.0f;
	mainMenuFPS = 3.0f;
	mainMenuCurrentIndex = 0;
	shootCD = 0.0;
	lightningboltCD = 0.0;
	projectileIndex = 0;
	lightningboltIndex = 0;
	playerScore = 0;
	state = STATE_MAIN_MENU;
	gameCondition = PRE_GAME;
	start = false;
	keys = SDL_GetKeyboardState(NULL);
	fontTextureID = LoadTexture("pixel_font.png", GL_RGBA);
	ssTextureID = LoadTexture("sprites.png", GL_RGBA);

	// setting up main menu animation
	Spritesheet mainScreen1(ssTextureID, 53.0f / 128.0f, 0.0f / 128.0f, 26.0f / 128.0f, 30.0f / 128.0f);
	Spritesheet mainScreen2(ssTextureID, 34.0f / 128.0f, 52.0f / 128.0f, 26.0f / 128.0f, 32.0f / 128.0f);
	Spritesheet mainScreen3(ssTextureID, 30.0f / 128.0f, 86.0f / 128.0f, 26.0f / 128.0f, 30.0f / 128.0f);
	Spritesheet mainScreen4(ssTextureID, 58.0f / 128.0f, 86.0f / 128.0f, 26.0f / 128.0f, 32.0f / 128.0f);

	// animation on the right
	Entity temp1(mainScreen1, 0.35f, 0.0f);
	Entity temp2(mainScreen2, 0.35f, 0.0f);
	mainMenuEntity1.push_back(temp1);
	mainMenuEntity1.push_back(temp2);

	// animation on the left
	Entity temp3(mainScreen3, -0.35f, 0.0f);
	Entity temp4(mainScreen4, -0.35f, 0.0f);
	mainMenuEntity2.push_back(temp3);
	mainMenuEntity2.push_back(temp4);
}

void Application::RenderMainMenu() {
	// side animations
	mainMenuEntity1[mainMenuCurrentIndex].ssDraw(0.2f);
	mainMenuEntity2[mainMenuCurrentIndex].ssDraw(0.2f);

	// start button
	glLoadIdentity();
	GLfloat startBox[] = { -0.25f, 0.1f, -0.25f, -0.1f, 0.25f, -0.1f, 0.25f, 0.1f };
	glVertexPointer(2, GL_FLOAT, 0, startBox);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat boxColor[] = { 0.6f, 0.6f, 0.6f, 1.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.6f, 0.6f, 0.6f, 1.0f };
	glColorPointer(4, GL_FLOAT, 0, boxColor);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_COLOR_ARRAY);

	glLoadIdentity();
	glTranslatef(-0.1f, 0.0f, 0.0f);
	drawText(fontTextureID, "START", 0.05f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	//game title
	glLoadIdentity();
	glTranslatef(-0.3f, 0.75f, 0.0f);
	drawText(fontTextureID, "Pikachu", 0.1f, 0.0f, 1.0f, 1.0f, 0.2f, 1.0f);
	glLoadIdentity();
	glTranslatef(-0.3f, 0.55f, 0.0f);
	drawText(fontTextureID, "Catcher", 0.1f, 0.0f, 1.0f, 1.0f, 0.2f, 1.0f);
}

void Application::RenderGameLevel() {
	// render "SCORE"
	glLoadIdentity();
	glTranslatef(-0.1f, 0.95f, 0.0f);
	drawText(fontTextureID, "SCORE", 0.05f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	// render actual score
	glLoadIdentity();
	glTranslatef(0.2f, 0.95f, 0.0f);
	drawText(fontTextureID, std::to_string(playerScore), 0.05f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	red.ssDraw(0.5f);
	shootProj();
	renderLightningbolt();
	renderPikachu();

	// message to inform player how to start game
	if (start != true) {
		glLoadIdentity();
		glTranslatef(-0.4f, -0.45f, 0.0f);
		drawText(fontTextureID, "Press 'S' to start!", 0.05f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	}
}
void Application::RenderGameOver() {
	if (gameCondition == WON) {
		glLoadIdentity();
		glTranslatef(-0.2f, 0.3f, 0.0f);
		drawText(fontTextureID, "YOU WIN!", 0.05f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	}
	if (gameCondition == LOST) {
		glLoadIdentity();
		glTranslatef(-0.2f, 0.3f, 0.0f);
		drawText(fontTextureID, "YOU LOST!", 0.05f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	}
	// draw grey box behind text
	glLoadIdentity();
	GLfloat startBox[] = { -0.25f, 0.1f, -0.25f, -0.1f, 0.25f, -0.1f, 0.25f, 0.1f };
	glVertexPointer(2, GL_FLOAT, 0, startBox);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat boxColor[] = { 0.6f, 0.6f, 0.6f, 1.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.6f, 0.6f, 0.6f, 1.0f };
	glColorPointer(4, GL_FLOAT, 0, boxColor);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	// draw text "MAIN MENU"
	glLoadIdentity();
	glTranslatef(-0.2f, 0.0f, 0.0f);
	drawText(fontTextureID, "MAIN MENU", 0.05f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

void Application::render() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	switch (state) {
	case STATE_MAIN_MENU:
		RenderMainMenu();
		break;
	case STATE_GAME_LEVEL:
		RenderGameLevel();
		break;
	case STATE_GAME_OVER:
		RenderGameOver();
		break;
	}

	SDL_GL_SwapWindow(displayWindow);
}

void Application::update(float elapsed) {
	float redMoveAmt = 0.0;
	switch (state){
	case STATE_MAIN_MENU:
		mainMenuElapsed += elapsed;
		if (mainMenuElapsed > 1.0 / mainMenuFPS) {
			mainMenuCurrentIndex++;
			mainMenuElapsed = 0.0f;

			if (mainMenuCurrentIndex > mainMenuEntity1.size() - 1) {
				mainMenuCurrentIndex = 0;
			}
		}
		break;
	case STATE_GAME_LEVEL:
		if (start == true) { // press s to start the game
			// player movement logic
			if (keys[SDL_SCANCODE_LEFT]) {
				redMoveAmt = -1.5;
			}
			if (keys[SDL_SCANCODE_RIGHT]) {
				redMoveAmt = 1.5;
			}
			if (red.getX() + (redMoveAmt * elapsed) < 1.22 && red.getX() + (redMoveAmt * elapsed) > -1.22) {
				red.setX((elapsed * redMoveAmt) + red.getX());
			}
			// shooting logic
			if (keys[SDL_SCANCODE_SPACE]) {
				if (projectileIndex < MAX_ONSCREEN - 1) {
					if (shootCD > 0.5f) { // can shoot every .5 seconds
						if (proj[projectileIndex] != NULL)
							delete proj[projectileIndex]; // prevent memory leak
						proj[projectileIndex] = new Projectile(red.getX(), red.getY() + 0.12f, 1.0f, ssTextureID, 0.0f, 52.0f, 32.0f, 32.0f);
						projectileIndex++;
						shootCD = 0.0f;
					}
				}
				else {
					if (shootCD > 0.5f) { // can shoot every .5 seconds
						projectileIndex = 0;
						delete proj[projectileIndex];
						proj[projectileIndex] = new Projectile(red.getX(), red.getY() + 0.12f, 1.0f, ssTextureID, 0.0f, 52.0f, 32.0f, 32.0f);
						projectileIndex++;
						shootCD = 0.0f;
					}
				}
			}
			// updates pokeball projectile in array
			for (int i = 0; i < MAX_ONSCREEN - 1; ++i) {
				if (proj[i] != NULL) {
					if (proj[i]->getY() < 0.95f) {
						proj[i]->updatePos(elapsed);
					}
					else if (proj[i]->getY() > 0.95f) {
						delete proj[i];
						proj[i] = NULL;
					}
				}
			}
			// pikachu movements
			for (int i = 0; i < MAX_PIKACHU - 1; ++i) {
				if (pikachuArray[i] != NULL) {
					if (pikachuArray[i]->getX() + (PIKACHU_SPEED * elapsed) > 1.23f) { // pikachu collide with right border
						if (pikachuArray[i]->getDirection() == direction::RIGHT) {
							pikachuArray[i]->setX(pikachuArray[i]->getX() + (-PIKACHU_SPEED * elapsed));
							pikachuArray[i]->setY(pikachuArray[i]->getY() - 0.2f);
							pikachuArray[i]->setDirection(direction::LEFT);
						}
					}
					else if (pikachuArray[i]->getX() + (PIKACHU_SPEED * elapsed) < -1.23f) { // pikachu collide with left border
						if (pikachuArray[i]->getDirection() == direction::LEFT) {
							pikachuArray[i]->setX(pikachuArray[i]->getX() + (-PIKACHU_SPEED * elapsed));
							pikachuArray[i]->setY(pikachuArray[i]->getY() - 0.2f);
							pikachuArray[i]->setDirection(direction::RIGHT);
						}
					}
					if (pikachuArray[i]->getDirection() == direction::RIGHT) {
						pikachuArray[i]->setX(pikachuArray[i]->getX() + PIKACHU_SPEED * elapsed);
					}
					else if (pikachuArray[i]->getDirection() == direction::LEFT) {
						pikachuArray[i]->setX(pikachuArray[i]->getX() + (-PIKACHU_SPEED * elapsed));
					}
				}
			}
			// pokeball collide with pikachu detection
			for (int i = 0; i < MAX_ONSCREEN - 1; ++i) {
				if (proj[i] != NULL) {
					for (int j = 0; j < MAX_PIKACHU - 1; ++j) {
						if (pikachuArray[j] != NULL && proj[i] != NULL) {
							if (proj[i]->getX() + 0.025f > pikachuArray[j]->getX() - 0.05f && proj[i]->getX() - 0.025f < pikachuArray[j]->getX() + 0.05f
								&& proj[i]->getY() + 0.025f > pikachuArray[j]->getY() - 0.05f && proj[i]->getY() - 0.025f < pikachuArray[j]->getY() + 0.05f) {
								delete pikachuArray[j];
								pikachuArray[j] = NULL;
								delete proj[i];
								proj[i] = NULL;
								playerScore += 100;
							}
						}
					}
				}

			}
			// lightningbolt logic
			if (lightningboltCD > 0.5f) { // change value to make pikachu drop lightning bolts more often
				if (lightningboltIndex < MAX_ONSCREEN - 1) {
					int randomPikachuIndex = rand() % (MAX_PIKACHU - 1);
					while (pikachuArray[randomPikachuIndex] == NULL)
						randomPikachuIndex = rand() % (MAX_PIKACHU - 1);
					if (lightningboltArray[lightningboltIndex] != NULL) // prevent memory leak
						delete lightningboltArray[lightningboltIndex];
					lightningboltArray[lightningboltIndex] = new Projectile(pikachuArray[randomPikachuIndex]->getX(), pikachuArray[randomPikachuIndex]->getY() - 0.05f,
						0.5f, ssTextureID, 62.0f, 32.0f, 21.0f, 24.0f);
					lightningboltIndex++;
					lightningboltCD = 0.0f;
				}
				else {
					lightningboltIndex = 0;
					int randomPikachuIndex = rand() % (MAX_PIKACHU - 1);
					while (pikachuArray[randomPikachuIndex] == NULL)
						randomPikachuIndex = rand() % (MAX_PIKACHU - 1);
					if (lightningboltArray[lightningboltIndex] != NULL) // prevent memory leak
						delete lightningboltArray[lightningboltIndex];
					lightningboltArray[lightningboltIndex] = new Projectile(pikachuArray[randomPikachuIndex]->getX(), pikachuArray[randomPikachuIndex]->getY() - 0.05f,
						0.5f, ssTextureID, 62.0f, 32.0f, 21.0f, 24.0f);
					lightningboltIndex++;
					lightningboltCD = 0.0f;
				}
			}
			// updating lightningbolt array
			for (int i = 0; i < MAX_ONSCREEN - 1; ++i) {
				if (lightningboltArray[i] != NULL) {
					if (lightningboltArray[i]->getY() > -0.95f) {
						lightningboltArray[i]->updatePos(-elapsed);
					}
					else if (lightningboltArray[i]->getY() < -0.95f) {
						delete lightningboltArray[i];
						lightningboltArray[i] = NULL;
					}
				}
			}

			bool over = true;
			// lightningbolt collide with player detection 
			for (int i = 0; i < MAX_ONSCREEN - 1; ++i) {
				if (lightningboltArray[i] != NULL) {
					if (lightningboltArray[i]->getX() - 0.025f > red.getX() - 0.11f && lightningboltArray[i]->getX() + 0.025f < red.getX() + 0.11f &&
						lightningboltArray[i]->getY() - 0.025f < red.getY() + 0.125f) {
						delete lightningboltArray[i];
						lightningboltArray[i] = NULL;
						over = true;
						gameCondition = LOST;
						break;
					}
				}
			}
			// collision detection for pikachu and losing
			for (int i = 0; i < MAX_PIKACHU - 1; ++i) {
				if (pikachuArray[i] != NULL) {
					// check if pikachu collide with the player or bottom of the screen
					if ((pikachuArray[i]->getX() - 0.05f < red.getX() + 0.1f && pikachuArray[i]->getX() + 0.05f > red.getX() - 0.1f)
						&& pikachuArray[i]->getY() - 0.05f < red.getY() + 0.125f) {
						over = true;
						gameCondition = LOST;
						break;
					}
					else if (pikachuArray[i]->getY() - 0.05f < -0.95f) { // in case the pikachu somehow manages to get past the player without triggering defeat
						over = true;
						gameCondition = LOST;
						break;
					}
				}
			}
			// check if game is over
			for (int i = 0; i < MAX_PIKACHU - 1; ++i) {
				// check if array is empty
				if (pikachuArray[i] != NULL) {
					over = false;
					break;
				}
			}
			// clearing memory when game is won/lost
			if (over || gameCondition == LOST) {
				for (int i = 0; i < MAX_PIKACHU - 1; ++i) { // clear memory for array of pikachu
					if (pikachuArray[i] != NULL) {
						delete pikachuArray[i];
						pikachuArray[i] = NULL;
					}
				}
				for (int j = 0; j < MAX_ONSCREEN - 1; ++j) { // clear memory for array of pokeballs
					if (proj[j] != NULL) {
						delete proj[j];
						proj[j] = NULL;
					}
				}
				for (int i = 0; i < MAX_ONSCREEN - 1; ++i) { // clear memory for array of lightningbolts
					if (lightningboltArray[i] != NULL) {
						delete lightningboltArray[i];
						lightningboltArray[i] = NULL;
					}
				}
				if (gameCondition != LOST)
					gameCondition = WON;
				state = STATE_GAME_OVER;
			}
		}
		shootCD += elapsed;
		lightningboltCD += elapsed;
		break;
	case STATE_GAME_OVER:
		break;
	}
}

bool Application::updateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			return true;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN && state == STATE_MAIN_MENU) { // load into game level
			if (event.button.button == 1) {
				float xPos = (((float)event.button.x / 800.0f) * 2.666f) - 1.333f;
				float yPos = (((float)(600 - event.button.y) / 600.0f) * 2.0f) - 1.0f;
				if (xPos > -0.25f && xPos < 0.25f && yPos > -0.1f && yPos < 0.1f) {
					state = STATE_GAME_LEVEL;
					initializeGame();
				}
			}
		}
		else if (event.type == SDL_KEYDOWN && state == STATE_GAME_LEVEL && start == false) { // start game
			if (event.key.keysym.scancode == SDL_SCANCODE_S) {
				start = true;
			}
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN && state == STATE_GAME_OVER) { // back to main menu
			if (event.button.button == 1) {
				float xPos = (((float)event.button.x / 800.0f) * 2.666f) - 1.333f;
				float yPos = (((float)(600 - event.button.y) / 600.0f) * 2.0f) - 1.0f;
				if (xPos > -0.25f && xPos < 0.25f && yPos > -0.1f && yPos < 0.1f) {
					state = STATE_MAIN_MENU;
				}
			}
		}
	}
	update(elapsed);
	render();
	return false;
}

void Application::shootProj() {
	for (int i = 0; i < MAX_ONSCREEN - 1; ++i) {
		if (proj[i] != NULL) {
			if (proj[i]->getY() < 0.95f) {
				proj[i]->setVis(true);
				proj[i]->draw();
			}
		}
	}
}

void Application::renderPikachu() {
	for (int i = 0; i < MAX_PIKACHU - 1; ++i) {
		if (pikachuArray[i] != NULL) {
			pikachuArray[i]->ssDraw(0.25f);
		}
	}
}

void Application::renderLightningbolt() {
	for (int i = 0; i < MAX_ONSCREEN - 1; ++i) {
		if (lightningboltArray[i] != NULL) {
			if (lightningboltArray[i]->getY() > -0.95f) {
				lightningboltArray[i]->setVis(true);
				lightningboltArray[i]->draw();
			}
		}
	}
}

void Application::initializeGame() {
	// initialize varibles for the game level
	shootCD = 0.0;
	lightningboltCD = 0.0;
	projectileIndex = 0;
	lightningboltIndex = 0;
	playerScore = 0;
	start = false;
	gameCondition = IN_PROGRESS;

	// initializing shooter
	Spritesheet redSprite(ssTextureID, 0.0f / 128.0f, 86.0f / 128.0f, 28.0f / 128.0f, 32.0f / 128.0f);
	red.setSprite(redSprite);
	red.setX(0.0f);
	red.setY(-0.875f);

	// initializing pikachu array
	float startX = -1.2f;
	float startY = 0.8f;
	float counter = 0.0f;
	float increment = 0.2f;
	Spritesheet test(ssTextureID, 0.0f / 128.0f, 0.0f / 128.0f, 51.0f / 128.0f, 50.0f / 128.0f);
	for (int i = 0; i < MAX_PIKACHU - 1; ++i) {
		if (startX + counter < 1.2) {
			pikachuArray[i] = new Entity(test, startX + counter, startY);
			counter += increment;
		}
		else {
			counter = 0.0f;
			startY -= 0.4f;
			pikachuArray[i] = new Entity(test, startX + counter, startY);
			counter += increment;
		}
	}
}