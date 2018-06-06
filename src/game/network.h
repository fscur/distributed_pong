#pragma once
#include "types.h"
#include "world.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct sockaddr Socket_Address;
typedef struct sockaddr_in Socket_Address_In;

#define CMD_SIZE sizeof(u32)
#define CMD_CONNECT 1
#define CMD_WAIT 2
#define CMD_NAME 3
#define CMD_READY 4
#define CMD_GAMEOVER 5

#define CLIENT_STATUS_DISCONNECTED 0
#define CLIENT_STATUS_WAITING 1
#define CLIENT_STATUS_CONNECTED 2

#pragma pack(push, 1) // pack struct bits
typedef struct Game_Input_Packet { i32 player_movement; } Game_Input_Packet;
#pragma pack(pop)

#pragma pack(push, 1) // pack struct bits
typedef struct Game_Packet {
  v2 ball_pos;                             // 8
  f32 ball_radius;                         // 4
  f32 players_positions[MAX_PLAYER_COUNT]; // 32
  i8 players_points[MAX_PLAYER_COUNT];     // 8
  char players_names[MAX_PLAYER_COUNT][MAX_PLAYER_NAME_LENGTH];
} Game_Packet;
#pragma pack(pop)

typedef struct Game_Client {
  i32 id;
  char name[MAX_PLAYER_NAME_LENGTH];
  Socket_Address_In address;
  u32 address_size;
  u32 status;
} Game_Client;

typedef struct Network_Connection {
  char server_ip[15];
  int server_port;
} Network_Connection;

typedef struct Network_State {
  Network_Connection connection;
  i32 server_socket;
  i32 client_socket;
  i32 connected_players;
  Game_Client clients[MAX_PLAYER_COUNT];
  World* world;
  Game_Packet current_packet;
} Network_State;

void network_init(Network_State* state);
void network_init_server(Network_State* state);
void network_init_client(Network_State* state);

bool network_accept_players(Network_State* state);
bool network_connect_to_server(Network_State* state,
                               char name[MAX_PLAYER_NAME_LENGTH],
                               u32* id);

void network_send_game_over_message(Network_State* state, Game_Client* client);
bool network_receive_game_over_message(Network_State* state);

void network_send_ready_message(Network_State* state);
bool network_receive_ready_message(Network_State* state);

void network_send_input(Network_State* state, i32 key);
i32 network_receive_input(Network_State* state, Game_Client* client);

void network_broadcast(Network_State* state);
void network_receive_data(Network_State* state);

void network_clear_input_buffer(i32 socket);