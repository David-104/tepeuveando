// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include "Networking.h"
#include "../utils/Singleton.h"

class LittleWolf;

class Game : public Singleton<Game> {
public:
	Game();
	virtual ~Game();
	bool init(char* host, Uint16 port);
	void start();


	Networking& get_networking() { return *net_; }

	LittleWolf& get_littlewolf() { return *little_wolf_; }
private:
	LittleWolf *little_wolf_;
	Networking* net_;
};

