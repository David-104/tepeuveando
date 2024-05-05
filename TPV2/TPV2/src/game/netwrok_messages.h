// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

#include <SDL_stdinc.h>
#include "../sdlutils/SDLNetUtils.h"

enum MsgType : Uint8 {
	_NONE = 0, //
	_CONNECTION_REQUEST, //
	_CONNECTION_ACCEPTED, //
	_CONNECTION_REJECTED, //
	_DISCONNECTED,
	_NEW_CLIENT_CONNECTED, //
	_PLAYER_STATE, //
	_PLAYER_INFO, //
	_SHOOT, //
	_DEAD, //
	_RESTART
};

struct Msg {
	Uint8 _type;

	_IMPL_SERIALIAZION_(_type)
};

struct MsgWithId: Msg {
	Uint8 _client_id;

	_IMPL_SERIALIAZION_WITH_BASE_(Msg,_client_id)
};

struct MsgWithMasterId: MsgWithId {
	Uint8 _master_id;

	_IMPL_SERIALIAZION_WITH_BASE_(MsgWithId,_master_id)
};

struct PlayerStateMsg: MsgWithId {

	uint8_t id;     // the id
	float ax;       //point a x 
	float ay;       //point a y
	float bx;       // point b x
	float by;       //point b y
	float whx;		//pos x
	float why;		//pos y
	float theta;   // rotation (in rad)

	_IMPL_SERIALIAZION_WITH_BASE_(MsgWithId, ax, ay, bx, by, whx, why)

};

struct PlayerInfoMsg: MsgWithId {

	float ax;
	float ay;
	float bx;
	float by;

	float pos_x;
	float pos_y;
	float vel_x;
	float vel_y;
	float speed;
	float a;
	float rot;
	Uint8 state;

	_IMPL_SERIALIAZION_WITH_BASE_(MsgWithId,ax, ay, bx, by, pos_x, pos_y, vel_x, vel_y, speed, a, rot, state)

};
struct ShootMsg: MsgWithId {

	float x;
	float y;
	float vx;
	float vy;
	int w;
	int h;
	float rot;

	_IMPL_SERIALIAZION_WITH_BASE_(MsgWithId, x,y,vx,vy,w,h,rot)

};
