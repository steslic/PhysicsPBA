/* 
Physics PBA 
Sebastian Teslic
6/15 - 6/18/2019

All credits to their respective owners for the Game Engine
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <chrono>
#include <thread>

#include "olcConsoleGameEngine.h"


using namespace std;


class STGame : public olcConsoleGameEngine // inheriting all from class olc... for constructor
{


public:
	STGame()
	{
		m_sAppName = L"ST Physics PBA Game";
	}




private:
	struct object
	{
		float x; // position
		float y;

		//float randx = 5.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (50.0 - 5.0))); // generate random float from 5 to 50 
		//float randy = 5.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (50.0 - 5.0))); // generate random float from 5 to 50 


		float xvel; // velocity
		float yvel;

		float angle;

		int size;
	};

	int score = 0;

	bool alive = true;

	vector<pair<float, float>> vUser; // x and y pos
	vector<pair<float, float>> vEnemy;

	vector<object> vBullets; // store all in one place
	vector<object> vEnemies; 
	//vector<object> vEnemiesNew;

	object user;



protected:

	void coordMgmt(float x2, float y2, float &x3, float &y3) // so it does not go off screen, sends to other edge 
	{
		x2 = x3;
		y2 = y3; 

		if (x2 < 0.0f)
		{
			x3 = x2 +(float)ScreenWidth();
		}

		if (x2 >= (float)ScreenWidth())
		{
			x3 = x2 - (float)ScreenWidth();
		}

		if (y2 < 0.0f)
		{
			y3 = y2 + (float)ScreenHeight();
		}

		if (y2 >= (float)ScreenHeight())
		{
			y3 = y2 - (float)ScreenHeight();
		}
	}

	void restartGame()
	{
		// initial vars
		user.x = ScreenWidth() / 2.0f;
		user.y = ScreenHeight() / 2.0f;

		user.xvel = 0.0f;
		user.yvel = 0.0f;

		user.angle = 0.0f;

		// clearing all elements of vector
		vBullets.clear();
		vEnemies.clear();

		score = 0;
		alive = true;

		// create new enemies
		vEnemies.push_back({ 15.0f, 20.0f, 5.0f, -5.0f, 0.0f, (int)16 });
		vEnemies.push_back({ 50.0f, 50.0f, -8.0f, 5.0f, 0.0f, (int)16 });
		vEnemies.push_back({ 75.0f, 15.0f, 3.0f, 2.0f, 0.0f, (int)16 });
	}


	

	

	bool enemyCollision(float x, float y, float x1, float y1, float radius)
	{
		return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1)) < radius; // use distance formula to check 
	}




	virtual bool OnUserCreate() // called when game launches, from header
	{
		vUser = { { -3.0f, 3.0f }, { 0.0f, -3.0f }, { 3.0f, 3.0f } }; // create user model 

		int angs = 50;


		// credits to OLC for function - creates randomization in "circle" edges 
		for (int i = 0; i < angs; i++)
		{
			float noise = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
			vEnemy.push_back(make_pair(noise * sinf(((float)i / (float)angs) * 6.28318f),
				noise * cosf(((float)i / (float)angs) * 6.28318f)));
		}


		restartGame();

		return true;
	}







	virtual bool OnUserUpdate(float fElapsedTime) // updates screen, from header
	{


		// changing position based on time and velocity
		user.x += fElapsedTime * user.xvel;
		user.y += fElapsedTime * user.yvel;



		if (alive == false)
		{
			restartGame();
		}

		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);

		if (m_keys[VK_SPACE].bReleased) // shoot 
		{
			vBullets.push_back({ user.x, user.y, 50.0f * sinf(user.angle), -50.0f * cosf(user.angle), 90.0f, 1 });
		} 

		for (auto &a : vBullets)
		{
			a.x += fElapsedTime * a.yvel;
			a.y += fElapsedTime * a.yvel;
			a.angle -= fElapsedTime * 1.0f;
		}

		
		if (m_keys['W'].bHeld) // acceleration straight
		{
			user.xvel += fElapsedTime * 25.0f * sin(user.angle);
			user.yvel += fElapsedTime * 25.0f * -cos(user.angle);
		}

		if (m_keys['S'].bHeld) // accelerate backwards
		{
			user.xvel -= fElapsedTime * 25.0f * sin(user.angle);
			user.yvel -= fElapsedTime * 25.0f * -cos(user.angle);
		}

		if (m_keys['A'].bHeld) // rotate player left or right by decreasing or increasing angle
		{
			user.angle -= fElapsedTime * 3.0f;
		}

		if (m_keys['D'].bHeld)
		{
			user.angle += fElapsedTime * 3.0f;
		}

		// draw player
		DrawWireFrameModel(vUser, user.x, user.y, user.angle);

		coordMgmt(user.x, user.y, user.x, user.y);

		// draw bullet
		for (auto a : vBullets)
		{
			Draw(a.x, a.y);
		}

		
		vector<object> vEnemiesNew; // new enemies after original ones killed
		

		// update position of bullet
		for (auto &a : vBullets)
		{
			coordMgmt(a.x, a.y, a.x, a.y);
			a.angle -= fElapsedTime * 1.0f;
			a.x += fElapsedTime * a.xvel;
			a.y += fElapsedTime * a.yvel;

			for (auto &b : vEnemies)
			{
				if (enemyCollision(b.x, b.y, a.x, a.y, b.size))
				{
					score += 1; // increase score for hit

					a.x = 999; // bullet off the map

					b.x = 999;
					

					//vEnemies.push_back({ b.x, b.y, 10.0f * sinf(2.5), 10.0f * cosf(1.4), 0.0f, int(a.size) });
					
					// create new enemies
					if (b.size > 4)
					{
						// create at random angles
						vEnemiesNew.push_back({ b.x, b.y, 10.0f * sinf(2.5), 10.0f * cosf(1.4), 0.0f, (int)a.size >> 100000 });
						vEnemiesNew.push_back({ b.x, b.y, 10.0f * sinf(3.141), 10.0f * cosf(3.2), 0.0f, (int)a.size >> 100000 });
						vEnemiesNew.push_back({ b.x, b.y, 10.0f * sinf(2.2), 10.0f * cosf(1.8), 0.0f, (int)a.size >> 100000 });
					}

					
					

					

				}
			}
		}

		for (auto a : vEnemiesNew)
		{
			vEnemies.push_back(a);
		}

		// stop lag by removing enemies that are off map 
		if (vEnemies.size() > 0)
		{
			auto i = remove_if(vEnemies.begin(), vEnemies.end(), [&](object o) { return (o.x < 0); });
			if (i != vEnemies.end())
				vEnemies.erase(i);
		}

		

		// remove bullets off map
		if (vBullets.size() > 0)
		{
			auto i = remove_if(vBullets.begin(), vBullets.end(), [&](object o) { return (o.x < 1 || o.y < 1 || o.x >= ScreenWidth() - 1 || o.y >= ScreenHeight() - 1); });
			if (i != vBullets.end())
				vBullets.erase(i);
		}


		// draw enemies
		for (auto &a : vEnemies)
		{
			DrawWireFrameModel(vEnemy, a.x, a.y, a.angle, (float)a.size, FG_DARK_RED); // draw
			
			coordMgmt(a.x, a.y, a.x, a.y);

			// change position
			a.angle += fElapsedTime;
			a.x += fElapsedTime * a.xvel;
			a.y += fElapsedTime * a.yvel;

		}



		// check player collision with circle by distance formula
		for (auto &a : vEnemies)
		{
			if (enemyCollision(a.x, a.y, user.x, user.y, a.size)) 
			{
				alive = false;
			}
		}


		// draw info on screen
		DrawString(1, 1, L"S_Teslic Physics PBA"); // from header - position (1, 1)
		DrawString(1, 5, L"Score = " + to_wstring(score)); // draw score on screen
		



		return true;
	}



	// Credits to their respective owners for draw model function
	void DrawWireFrameModel(const vector<pair<float, float>> &vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE)
	{
		// pair.first = x coordinate
		// pair.second = y coordinate

		// Create translated model vector of coordinate pairs
		vector<pair<float, float>> vecTransformedCoordinates;
		int angs = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(angs);

		// Rotate
		for (int i = 0; i < angs; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < angs; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < angs; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i < angs + 1; i++)
		{
			int j = (i + 1);
			DrawLine(vecTransformedCoordinates[i % angs].first, vecTransformedCoordinates[i % angs].second,
				vecTransformedCoordinates[j % angs].first, vecTransformedCoordinates[j % angs].second, PIXEL_SOLID, col);
		} 
	} 

};			





int main()
{
	STGame game;

	game.ConstructConsole(160, 100, 8, 8); // from header
	game.Start();


	return 0;
} 