#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

int screen_x = 1129;
int screen_y = 900;

const int MAX_ENEMIES = 12; 
const int Ghost = 4;
const int Skeleton = 8;
const int GHOST_H = 30; 
const int GHOST_W = 34;
const int SKELETON_H = 41;
const int SKELETON_W = 24;
const float ENEMY_SCALE = 1.4f;

struct EnemyData 
{
    float x;
    float y;
    int width;
    int height;
    bool is_skeleton;
    sf::Sprite sprite;
};
   
EnemyData enemies[MAX_ENEMIES];
Texture GhostTexture;
Texture SkeletonTexture;

bool check_player_enemy_collision(float p_x, float p_y, float e_x, float e_y, int Ewidth, int Eheight, int Pwidth, int Pheight) 
{
    return false;
}

void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, const int height, const int width, const int cell_size)
{
	window.draw(bgSprite);

	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{
			if (lvl[i][j] == '#')
			{
				blockSprite.setPosition(j * cell_size, i * cell_size);
				window.draw(blockSprite);
			}
		}
	}
}

void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{ 
	offset_y = player_y;
	offset_y += velocityY;

	char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x ) / cell_size];
	char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
	char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth /2) / cell_size];

	if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')
	{
		onGround = true;
	}
	else
	{
		player_y = offset_y;
		onGround = false;
	}

	if (!onGround)
	{
		velocityY += gravity / 60.0f ;
		if (velocityY >= terminal_Velocity) {
			velocityY = terminal_Velocity;
		}
	}
	else
	{
		velocityY = 0;
	}
}

bool collision_check_right(char** lvl ,float& player_x ,float& player_y , int &Pwidth ,int &Pheight , int cell_size , float speed)
{
	float next_x = player_x + speed;

	char top_right = lvl[(int)(player_y) / cell_size][(int)(next_x + Pwidth) / cell_size];
	char mid_right = lvl[(int)(player_y + Pheight / 2) / cell_size][(int)(next_x + Pwidth) / cell_size];
	char bottom_right = lvl[(int)(player_y + Pheight) / cell_size][(int)(next_x + Pwidth) / cell_size];

	if (top_right == '#' || mid_right =='#' || bottom_right == '#')
	{
		return false;
	}
	return true;
}

bool collision_check_left(char** lvl , float player_x,float player_y ,int Pheight , int Pwidth , int cell_size , float speed)
{
	float next_x = player_x - speed;
 
	char top_left = lvl[(int)(player_y)/ cell_size][(int)(next_x) / cell_size];
	char left_mid = lvl[(int)(player_y + Pheight / 2) / cell_size][(int)(next_x) / cell_size];
	char bottom_left = lvl[(int)(player_y + Pheight) / cell_size][(int)(next_x) / cell_size];
  
	if (top_left == '#' || left_mid == '#' || bottom_left == '#')
	{
		return false;
	}
	return true;
}

bool can_move_up(char** lvl, float player_x, float player_y, int Pheight, int Pwidth, int cell_size, float speed)
{
	float next_y = player_y - speed;
    
	char top_left_up = lvl[(int)(next_y) / cell_size][(int)(player_x) / cell_size];
	char top_mid_up = lvl[(int)(next_y) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];
	char top_right_up = lvl[(int)(next_y) / cell_size][(int)(player_x + Pwidth) / cell_size];
    
	if ((top_left_up == '#' || top_mid_up == '#' || top_right_up == '#'))
	{
		return false;
	}
	return true;
}

bool can_move_down(char** lvl, float player_x, float player_y, int Pheight, int Pwidth, int cell_size, float speed)
{
	float next_y = player_y + speed;
    
	char bottom_left_down = lvl[(int)(next_y + Pheight) / cell_size][(int)(player_x) / cell_size];
	char bottom_mid_down = lvl[(int)(next_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];
	char bottom_right_down = lvl[(int)(next_y + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
    
	if (bottom_left_down == '#'||bottom_mid_down == '#'|| bottom_right_down == '#')
	{
		return false;
	}
	return true;
}

void start_screen(RenderWindow& window , int& current_state , Event& ev)
{
	if (ev.type == Event::Closed) {
		window.close();
	}
	if (ev.type==Event::KeyPressed && ev.key.code == Keyboard::Enter) {
		current_state = 1;
	}
}
    
void select_player(RenderWindow& window, int& current_state ,int& choice , Sprite& yellow_sprite , Sprite& green_sprite, Event& ev)
{
	if (ev.type == Event::Closed) {
		window.close();
	}
    
	if (ev.type == Event::KeyPressed) {
		if (ev.key.code == Keyboard::Right){
			yellow_sprite.setScale(2,2);
			green_sprite.setScale(4,4);
			choice = 0; // yellow selected
		}
		if (ev.key.code == Keyboard::Left){
			green_sprite.setScale(2,2);
			yellow_sprite.setScale(4,4);
			choice = 1; // green selected
		}
		if (ev.key.code == Keyboard::Enter ){
			current_state = 2;
		}
	}
}

void level_select(RenderWindow& window ,Sprite& level1SPrite,Sprite& level2Sprite, Sprite& level3Sprite, int& current_state , int& level_choice, Event& ev)
{
	if (ev.type == Event::Closed) {
		window.close();
	}
    
	if (ev.type == Event::KeyPressed) {
		if (ev.key.code == Keyboard::Right){
			level_choice+=1;
			if(level_choice>3){
				level_choice =1;
			}
		}
		if (ev.key.code == Keyboard::Left) {
			level_choice-=1;
			if(level_choice<1){
				level_choice=3;
			}
		}
		if (ev.key.code == Keyboard::Enter ){
			current_state = 3;
		}
	}
}

int main()
{
	RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);


	int current_state = 0; 
	int selected_player = 1;
	int level_choice = 1;
	int choice = 1;
	bool playerSpriteInitialized = false;

	const int cell_size = 33;
	const int height = 20;
	const int width = 34;
	char** lvl;

	Texture bgTex;
	Sprite bgSprite;
	Texture blockTexture;
	Sprite blockSprite;

	bgTex.loadFromFile("Data/bg.png");
	bgSprite.setTexture(bgTex);
	bgSprite.setPosition(0,0);

	blockTexture.loadFromFile("Data/block1.png");
	blockSprite.setTexture(blockTexture);
	blockSprite.setScale(0.6,0.6);

	Texture titleTex;
	Sprite titleSprite;
	Texture startTex;
	Sprite startSprite;
	
	titleTex.loadFromFile("Data/Startbg.png");
	titleSprite.setTexture(titleTex);
	titleSprite.setPosition(0,0);
	startTex.loadFromFile("Data/Start.png");
	startSprite.setTexture(startTex);
	startSprite.setScale(0.25,0.25);
	startSprite.setPosition(435,465);
	
	Texture yellowTexture;
	Sprite yellowSprite;
	Texture greenTexture;
	Sprite greenSprite;
	
	yellowTexture.loadFromFile("Data/player2.png");
	yellowSprite.setTexture(yellowTexture);
	yellowSprite.setScale(1.5,1.5);
	yellowSprite.setPosition(225, 330);
	
	greenTexture.loadFromFile("Data/player1.png");
	greenSprite.setTexture(greenTexture);
	greenSprite.setScale(1.5,1.5);
	greenSprite.setPosition(840, 330);
	
	Texture playerTexture;
	Sprite playerSprite;
	playerTexture.loadFromFile("Data/player2.png");
	playerSprite.setTexture(playerTexture);
	playerSprite.setScale(1.5, 1.5);
       
       Texture level1Texture;
       Sprite level1Sprite;
       Texture level2Texture;
       Sprite level2Sprite;
       Texture level3Texture;
       Sprite level3Sprite;
       
       level1Texture.loadFromFile("Data/Level1.png");
       level1Sprite.setTexture(level1Texture);
       level1Sprite.setScale(0.15,0.15);
       level1Sprite.setPosition(485,50);
       
       level2Texture.loadFromFile("Data/Level2.png");
        level2Sprite.setTexture(level2Texture);
        level2Sprite.setScale(0.15,0.15);
        level2Sprite.setPosition(485,240);
       
       level3Texture.loadFromFile("Data/Level3.png");
        level3Sprite.setTexture(level3Texture); 
        level3Sprite.setScale(0.15,0.15);
        level3Sprite.setPosition(485,430);
       
       
       
       
	Music lvlMusic;
	lvlMusic.openFromFile("Data/mus.ogg");
	lvlMusic.setVolume(20);
	lvlMusic.play();
	lvlMusic.setLoop(true);

	float player_x = 565;
	float player_y = 0;
	float speed = 3;

	const float jumpStrength = 0;
	const float gravity = 9.8;

	bool isJumping = false;
	bool up_collide = false;
	bool left_collide = false;
	bool right_collide = false;

	Texture PlayerTexture;
	Sprite PlayerSprite;

	bool onGround = false;

	float offset_x = 0;
	float offset_y = 0;
	float velocityY = 0;

	float terminal_Velocity = 10;

	int PlayerHeight = 48;
	int PlayerWidth = 36;

	bool up_button = false;

	char top_left = '\0';
	char top_right = '\0';
	char top_mid = '\0';

	char left_mid = '\0';
	char right_mid = '\0';

	char bottom_left = '\0';
	char bottom_right = '\0';
	char bottom_mid = '\0';

	char bottom_left_down = '\0';
	char bottom_right_down = '\0';
	char bottom_mid_down = '\0';

	char top_right_up = '\0';
	char top_mid_up = '\0';
	char top_left_up = '\0';

	PlayerTexture.loadFromFile("Data/player1.png");
	PlayerSprite.setTexture(PlayerTexture);
	PlayerSprite.setScale(1.5,1.5);
	PlayerSprite.setPosition(player_x, player_y);
	const int PLAYER_TEXTURE_WIDTH = PlayerTexture.getSize().x;
	const int PLAYER_TEXTURE_HEIGHT = PlayerTexture.getSize().y;
	PlayerSprite.setTexture(PlayerTexture);

	lvl = new char* [height];
	for (int i = 0; i < height; i ++)
	{
		lvl[i] = new char[width];
		for (int j = 0; j < width; j++)
		{
			lvl[i][j] = '.';
		}
	}
    
	lvl [16][5] ='#';
	lvl [16][6] ='#';
	lvl [16][7] ='#';
	lvl [16][8] ='#';
	lvl [16][9] ='#';
	lvl [16][10] ='#';
	lvl [16][11] ='#';
	lvl [16][12] ='#';
	lvl [16][13] ='#';
	lvl [16][14] ='#';
	lvl [16][15] ='#';
	lvl [13][15] ='#';
	lvl [12][15] ='#';
	lvl [11][15] ='#';
	lvl [10][15] ='#';
	lvl [9][15] ='#';
	lvl [8][15] ='#';
	lvl [7][15] ='#';
	lvl [16][16] ='#'; lvl [15][16] ='#'; lvl [14][16] ='#'; 
	lvl [13][16] ='#';
	lvl [12][16] ='#';
	lvl [11][16] ='#';
	lvl [10][16] ='#';
	lvl [9][16] ='#';
	lvl [8][16] ='#';
	lvl [7][16] ='#';
	lvl [6][16] ='#';
	lvl [5][16] ='#';
	lvl [4][16] ='#';
	lvl [16][17] ='#'; lvl [15][17] ='#'; lvl [14][17] ='#';
	lvl [13][17] ='#';
	lvl [12][17] ='#';
	lvl [11][17] ='#';
	lvl [10][17] ='#';
	lvl [9][17] ='#';
	lvl [8][17] ='#';
	lvl [7][17] ='#';
	lvl [6][17] ='#';
	lvl [5][17] ='#';
	lvl [4][17] ='#';
	lvl [16][18] ='#';
	lvl [13][18] ='#';
	lvl [12][18] ='#';
	lvl [11][18] ='#';
	lvl [10][18] ='#';
	lvl [9][18] ='#';
	lvl [8][18] ='#';
	lvl [7][18] ='#';
	lvl [16][19] ='#';
	lvl [16][20] ='#';
	lvl [16][21] ='#';
	lvl [16][22] ='#';
	lvl [16][23] ='#';
	lvl [16][24] ='#';
	lvl [16][25] ='#';
	lvl [16][26] ='#';
	lvl [16][27] ='#';
	lvl [16][28] ='#';
          
	lvl [10][5] ='#';
	lvl [10][6] ='#';
	lvl [10][7] ='#';
	lvl [10][8] ='#';
	lvl [10][9] ='#';
	lvl [10][10] ='#';
	lvl [10][11] ='#';
	lvl [10][12] ='#';
	lvl [10][13] ='#';
	lvl [10][14] ='#';
	lvl [10][19] ='#';
	lvl [10][20] ='#';
	lvl [10][21] ='#';
	lvl [10][22] ='#';
	lvl [10][23] ='#';
	lvl [10][24] ='#';
	lvl [10][25] ='#';
	lvl [10][26] ='#';
	lvl [10][27] ='#';
	lvl [10][28] ='#';
          
	lvl [4][5] ='#';
	lvl [4][6] ='#';
	lvl [4][7] ='#';
	lvl [4][8] ='#';
	lvl [4][9] ='#';
	lvl [4][10] ='#';
	lvl [4][11] ='#';
	lvl [4][12] ='#';
	lvl [4][13] ='#';
	lvl [4][14] ='#';
	lvl [4][15] ='#';
	lvl [4][16] ='#';
	lvl [4][17] ='#';
	lvl [4][18] ='#';
	lvl [4][19] ='#';
	lvl [4][20] ='#';
	lvl [4][21] ='#';
	lvl [4][22] ='#';
	lvl [4][23] ='#';
	lvl [4][24] ='#';
	lvl [4][25] ='#';
	lvl [4][26] ='#';
	lvl [4][27] ='#';
	lvl [4][28] ='#';
          
	lvl [7][1] ='#';
	lvl [7][2] ='#';
	lvl [7][3] ='#';
	lvl [7][4] ='#';
	lvl [7][5] ='#';
	lvl [7][6] ='#';
	lvl [7][7] ='#';
	lvl [7][8] ='#';
	lvl [7][9] ='#';
	lvl [7][10] ='#';
	lvl [7][23] ='#';
	lvl [7][24] ='#';
	lvl [7][25] ='#';
	lvl [7][26] ='#';
	lvl [7][27] ='#';
	lvl [7][28] ='#';
	lvl [7][29] ='#';
	lvl [7][30] ='#';
	lvl [7][31] ='#';
	lvl [7][32] ='#';
	lvl [7][33] ='#';
          
	lvl [13][1] ='#';
	lvl [13][2] ='#';
	lvl [13][3] ='#';
	lvl [13][4] ='#';
	lvl [13][5] ='#';
	lvl [13][6] ='#';
	lvl [13][7] ='#';
	lvl [13][8] ='#';
	lvl [13][9] ='#';
	lvl [13][10] ='#';
	lvl [13][23] ='#';
	lvl [13][24] ='#';
	lvl [13][25] ='#';
	lvl [13][26] ='#';
	lvl [13][27] ='#';
	lvl [13][28] ='#';
	lvl [13][29] ='#';
	lvl [13][30] ='#';
	lvl [13][31] ='#';
	lvl [13][32] ='#';
	lvl [13][33] ='#';
          
	GhostTexture.loadFromFile("Data/Ghost.png");
	SkeletonTexture.loadFromFile("Data/Skeleton.png"); 
	int skeleton_index = 0;
	int ghost_index = 0;

	for (int i = 0; i < MAX_ENEMIES; ++i) {
		EnemyData& e = enemies[i];
		e.sprite.setScale(ENEMY_SCALE, ENEMY_SCALE);

		if (i < Skeleton) {
			e.is_skeleton = true;
			e.width = static_cast<int>(SKELETON_W * ENEMY_SCALE);
			e.height = static_cast<int>(SKELETON_H * ENEMY_SCALE);
			e.sprite.setTexture(SkeletonTexture);
        
	if (skeleton_index < 2) {
		e.y = (4 * cell_size) - e.height; 
		e.x = (5 * cell_size) + skeleton_index * (23 * cell_size);
	} else if (skeleton_index < 4) {
		e.y = (10 * cell_size) - e.height;
		e.x = (14 * cell_size) + (skeleton_index-2) * ((5 * cell_size) + 3);
	} else if (skeleton_index < 6) {
		e.y = (16 * cell_size) - e.height;
		e.x = (15* cell_size) + (skeleton_index-4) * ((3 * cell_size) + 3);
	} else {
		e.y = (19 * cell_size) - e.height;
		e.x = (cell_size + 3) + (skeleton_index-6) * (31 * cell_size);
		}
	skeleton_index++;
	}
        else {
	e.is_skeleton = false;
	e.width = static_cast<int>(GHOST_W * ENEMY_SCALE);
	e.height = static_cast<int>(GHOST_H * ENEMY_SCALE);
	e.sprite.setTexture(GhostTexture);
        
	if (ghost_index < 2) {
		e.y = (8 * cell_size) - (e.height * 2);
		e.x = (10 * cell_size) + ghost_index * ((21 * cell_size) + 21);
	} else {
		e.y = (14 * cell_size) - (e.height * 2);
		e.x = (cell_size + 3) + (ghost_index - 2) * ((21 * cell_size) + 21);
	 	}
		ghost_index++;
	}
}  
          
	int i=0;
	while(i<height){
		int j=0;
		while(j<width){
			if(i==0 || j==0 || i==height-1 || j==width-1 ){
				lvl[i][j]='#';
			}
			j++;
		}
		i++;
	}
          
	Event ev;

	while (window.isOpen())
	{
		while (window.pollEvent(ev))
		{
	if (current_state == 0) {
	start_screen(window, current_state, ev);
			}
	else if (current_state == 1) {
	select_player(window, current_state, choice, yellowSprite, greenSprite, ev);
			}
else if (current_state == 2) {
	level_select(window,level1Sprite,level2Sprite,level3Sprite, current_state ,level_choice, ev);
			}
    else if (current_state == 3) {
		if (ev.type == Event::Closed) {
			window.close();
		}
	}
}

		if (current_state == 3 && !playerSpriteInitialized) {
			if(choice == 1){
				playerSprite.setTexture(yellowTexture); // Yellow
				speed = 5;
			} else {
				playerSprite.setTexture(greenTexture); // Green
			}
			playerSprite.setScale(1.5, 1.5); 
			playerSprite.setOrigin(0, 0); 
			playerSpriteInitialized = true; 
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

if (current_state == 2) {
            level1Sprite.setScale(0.15, 0.15);
            level2Sprite.setScale(0.15, 0.15);
            level3Sprite.setScale(0.15, 0.15);
            
            if (level_choice == 1) {
                level1Sprite.setScale(0.3, 0.3); 
            } else if (level_choice == 2) {
                level2Sprite.setScale(0.3, 0.3); 
            } else if (level_choice == 3) {
                level3Sprite.setScale(0.3, 0.3); 
            }
        }






		if (current_state == 3)
		{
			if (Keyboard::isKeyPressed(Keyboard::Left)){
				if(collision_check_left(lvl, player_x, player_y, PlayerHeight, PlayerWidth, cell_size, speed)){
					player_x -=speed;
					playerSprite.setScale(1.5, 1.5);
					playerSprite.setOrigin(0, 0);
				}
			}
			
			if (Keyboard::isKeyPressed(Keyboard::Right)){
				if(collision_check_right(lvl, player_x, player_y, PlayerHeight, PlayerWidth, cell_size, speed)){
					player_x +=speed;
					playerSprite.setScale(-1.5, 1.5); 
					playerSprite.setOrigin(playerSprite.getLocalBounds().width, 0);
				}
			}
			
			if (Keyboard::isKeyPressed(Keyboard::Up)){
				if (can_move_up(lvl, player_x, player_y, PlayerHeight, PlayerWidth, cell_size, speed)&&onGround)
				{
					player_y -= 3*23;
					onGround = false;
				}
			}

			player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
		}

		window.clear();
		if (current_state == 0) {
			window.draw(bgSprite);
			window.draw(titleSprite);
			window.draw(startSprite);
		}
		else if (current_state == 1) {
			window.draw(bgSprite);
			window.draw(yellowSprite);
			window.draw(greenSprite);
		}
		else if (current_state == 2) {
			window.draw(bgSprite);
			window.draw(level1Sprite);
          		window.draw(level2Sprite);
         		window.draw(level3Sprite);
		}
		else if (current_state == 3) {
			display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
			
			playerSprite.setPosition(player_x, player_y);
			window.draw(playerSprite);
			
			for (int i = 0; i < MAX_ENEMIES; ++i) {
				EnemyData& current_enemy = enemies[i];
				current_enemy.sprite.setPosition(current_enemy.x, current_enemy.y);
				window.draw(current_enemy.sprite);
			}
		}

		window.display();
	}

	lvlMusic.stop();
	for (int i = 0; i < height; i++)
	{
		delete[] lvl[i];
	}
	delete[] lvl;

	return 0;
}//mainclose
