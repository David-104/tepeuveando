// This file is part of the course TPV2@UCM - Samir Genaim

#include "Networking.h"

#include <iostream>
#include "Game.h"
#include "LittleWolf.h"
#include "netwrok_messages.h"
#include "../sdlutils/SDLNetUtils.h"
#include "..\utils/Vector2D.h"

Networking::Networking() :
		sock_(), //
		socketSet_(), //
		p_(), //
		srvadd_(), //
		clientId_(), //
		masterId_() {
}

Networking::~Networking() {
}

bool Networking::init(char *host, Uint16 port) {

	if (SDLNet_ResolveHost(&srvadd_, host, port) < 0) {
		SDLNetUtils::print_SDLNet_error();
		return false;
	}

	sock_ = SDLNet_UDP_Open(0);
	if (!sock_) {
		SDLNetUtils::print_SDLNet_error();
		return false;
	}

	p_ = SDLNet_AllocPacket(512);
	if (!p_) {
		SDLNetUtils::print_SDLNet_error();
		return false;
	}

	socketSet_ = SDLNet_AllocSocketSet(1);
	SDLNet_UDP_AddSocket(socketSet_, sock_);

	Msg m0;
	MsgWithMasterId m1;

	// request to connect
	m0._type = _CONNECTION_REQUEST;
	SDLNetUtils::serializedSend(m0, p_, sock_, srvadd_);

	bool connected = false;
	// wait 3sec for the response, if we don't get we stop (since done will be 'true')
	if (SDLNet_CheckSockets(socketSet_, 3000) > 0) {
		if (SDLNet_SocketReady(sock_)) {
			if (SDLNetUtils::deserializedReceive(m0, p_, sock_) > 0) {
				switch (m0._type) {
				case _CONNECTION_ACCEPTED:
					m1.deserialize(p_->data);
					clientId_ = m1._client_id;
					masterId_ = m1._master_id;
					connected = true;
					break;
				case _CONNECTION_REJECTED:
					break;
				}
			}
		}
	}

	if (!connected) {
		std::cerr << "Failed to get a client id" << std::endl;
		return false;
	}

#ifdef _DEBUG
	std::cout << "Connected with id " << (int) clientId_ << std::endl;
#endif

	return true;
}

bool Networking::disconnect() {
	MsgWithId m;
	m._type = _DISCONNECTED;
	m._client_id = clientId_;
	return (SDLNetUtils::serializedSend(m, p_, sock_, srvadd_) > 0);

}

void Networking::update() {
	Msg m0;
	MsgWithMasterId m1;
	PlayerStateMsg m2;
	ShootMsg m3;
	MsgWithId m4;
	PlayerInfoMsg m5;
	MsgTimeLeft m6;

	while (SDLNetUtils::deserializedReceive(m0, p_, sock_) > 0) {
		switch (m0._type) {
		case _NEW_CLIENT_CONNECTED:
			//std::cout << "new client";
			m1.deserialize(p_->data);
			masterId_ = m1._master_id;
			handle_new_client(m1._client_id);
			break;

		case _DISCONNECTED:
			//std::cout << "disconnected";
			m1.deserialize(p_->data);
			masterId_ = m1._master_id;
			handle_disconnet(m1._client_id);
			break;

		/*case _PLAYER_STATE:
			//std::cout << "player state";
			m2.deserialize(p_->data);
			handle_player_state(m2);
			break;*/

		case _PLAYER_INFO:
			m5.deserialize(p_->data);
			handle_player_info(m5);
			break;

		case _SHOOT:
			m3.deserialize(p_->data);
			handle_shoot(m3);
			break;

		case _DEAD:
			m4.deserialize(p_->data);
			handle_dead(m4);
			break;
		case _RESTART:
			handle_restart();
			break;
		case _START_WAIT_TIME:
			Game::instance()->get_littlewolf().startWaitTime();
			break;
		default:
			break;
		}
	}
}

void Networking::handle_new_client(Uint8 id) {
	if (id != clientId_)
		Game::instance()->get_littlewolf().send_my_info();
}

void Networking::handle_disconnet(Uint8 id) {
	Game::instance()->get_littlewolf().removePlayer(id);
}

/*void Networking::send_state(LittleWolf::Line& fov, LittleWolf::Point& where, float theta) {
	PlayerInfoMsg m;
	m._type = _PLAYER_STATE;
	m._client_id = clientId_;
	m.ax = fov.a.x;
	m.ay = fov.a.y;
	m.bx = fov.b.x;
	m.by = fov.b.y;
	m.whx = where.x;
	m.why = where.y;
	m.theta = theta;
	SDLNetUtils::serializedSend(m, p_, sock_, srvadd_);
}*/

/*void Networking::handle_player_state(const PlayerStateMsg &m) {

	if (m._client_id != clientId_) {
		Game::instance()->get_littlewolf().update_player_state(m.id, m.ax, m.ay, m.bx, m.by, m.whx, m.why, m.theta);
	}
}*/

void Networking::send_shoot(Uint8 clientId_, LittleWolf::Line fov, LittleWolf::Point where, float theta) {


	ShootMsg m;
	m._type = _SHOOT;
	m._client_id = clientId_;
	m.ax = fov.a.x;
	m.ay = fov.a.y;
	m.bx = fov.b.x;
	m.by = fov.b.y;
	m.whx = where.x;
	m.why = where.y;
	m.theta = theta;
	SDLNetUtils::serializedSend(m, p_, sock_, srvadd_);
}

void Networking::handle_shoot(const ShootMsg &m) {
	//se reproduce el sonido (a la distancia adecuada)
	Game::instance()->get_littlewolf().playShoot(m._client_id);

	//y solo si es master hace colisiones
	if (is_master())
	{
		Uint8 id = m._client_id;
		float theta = m.theta;
		LittleWolf::Line fov;
		fov.a.x = m.ax;
		fov.a.y = m.ay;
		fov.b.x = m.bx;
		fov.b.y = m.by;
		LittleWolf::Point where;
		where.x = m.whx;
		where.y = m.why;

		Game::instance()->get_littlewolf().checkForCollision(id, fov, where, theta);
	}
}

void Networking::send_dead(Uint8 id) {
	MsgWithId m;
	m._type = _DEAD;
	m._client_id = id;
	SDLNetUtils::serializedSend(m, p_, sock_, srvadd_);
}

void Networking::handle_dead(const MsgWithId &m) {
	Game::instance()->get_littlewolf().killPlayer(m._client_id);
	
}

void Networking::send_my_info(const LittleWolf::Point& a, const LittleWolf::Point& b, const LittleWolf::Point& pos, const LittleWolf::Point& vel, float s, float acc, float rot, Uint8 state) {
	PlayerInfoMsg m;
	m._type = _PLAYER_INFO;
	m._client_id = clientId_;
	m.ax = a.x;
	m.ay = a.y;
	m.bx = b.x;
	m.by = b.y;
	m.pos_x = pos.x;
	m.pos_y = pos.y;
	m.vel_x = vel.x;
	m.vel_y = vel.y;
	m.speed = s;
	m.a = acc;
	m.rot = rot;
	m.state = state;
	SDLNetUtils::serializedSend(m, p_, sock_, srvadd_);
}

void Networking::handle_player_info(const PlayerInfoMsg& m) {
	if (m._client_id != clientId_) {
		Game::instance()->get_littlewolf().update_player_info(m._client_id, m.ax, m.ay, m.bx, m.by, m.pos_x,m.pos_y, m.vel_x, m.vel_y, m.speed, m.a, m.rot, (LittleWolf::PlayerState)m.state);
	}
}

void Networking::send_restart() {
	Msg m;
	m._type = _RESTART;
	SDLNetUtils::serializedSend(m, p_, sock_, srvadd_);
}

void Networking::start_wait_time()
{
	Msg m;
	m._type = _START_WAIT_TIME;
	printf("wait time started \n");

	SDLNetUtils::serializedSend(m, p_, sock_, srvadd_);
}

void Networking::send_time_left(int time)
{
	MsgTimeLeft m;
	m._type = _TIME_LEFT_RESTART;
	m.time = time;

	SDLNetUtils::serializedSend(m, p_, sock_, srvadd_);
}

void Networking::handle_restart() {
	Game::instance()->get_littlewolf().bringAllToLife();
}

void Networking::handle_time_left(const MsgTimeLeft& m) {
	Game::instance()->get_littlewolf().renderWithTime(m.time);
}

