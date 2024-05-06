// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <SDL_stdinc.h>
#include "../utils/Singleton.h"

class Networking;
class LittleWolf;

class Game : public Singleton<Game> {
	friend Singleton<Game>;
	Game();
public:
	virtual ~Game();
	bool init(char* host, Uint16 port);
	void start();

	Networking& get_networking() { return *net_; }

	LittleWolf& get_littlewolf() { return *little_wolf_; }

	Uint32 get_startTime() { return startTime; }
	Uint32 get_frameTime() { return frameTime; }

private:
	LittleWolf *little_wolf_;
	Networking* net_;

	Uint32 startTime;
	Uint32 frameTime;
};

