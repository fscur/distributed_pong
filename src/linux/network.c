#pragma once
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../game/network.h"
#include "../game/world.h"

void
serialize(Game_Packet* packet, i8* data) {
  f32* f = (f32*)data;
  *f = packet->ball_pos.x;
  f++;
  *f = packet->ball_pos.y;
  f++;
  *f = packet->ball_radius;
  f++;

  for (i32 i = 0; i < MAX_PLAYER_COUNT; ++i) {
    *f = packet->players_positions[i];
    f++;
  }

  i8* b = (i8*)f;
  for (i32 i = 0; i < MAX_PLAYER_COUNT; ++i) {
    *b = packet->players_points[i];
    b++;
  }
}

void
deserialize(i8* data, Game_Packet* packet) {
  f32* f = (f32*)data;
  packet->ball_pos.x = *f;
  f++;
  packet->ball_pos.y = *f;
  f++;
  packet->ball_radius = *f;
  f++;

  for (i32 i = 0; i < MAX_PLAYER_COUNT; ++i) {
    packet->players_positions[i] = *f;
    f++;
  }

  i8* p = (i8*)f;
  for (i32 i = 0; i < MAX_PLAYER_COUNT; ++i) {
    packet->players_points[i] = *p;
    p++;
  }
}

void
network_init(Network_State* state) {
  for (int i = 0; i < MAX_PLAYER_COUNT; ++i) {
    state->clients[i].id = -1;
  }
}

void
network_init_client(Network_State* state) {
  i32 client_socket = socket(AF_INET, SOCK_DGRAM, 0);

  struct timeval read_timeout;
  read_timeout.tv_sec = 0;
  read_timeout.tv_usec = 10;
  setsockopt(client_socket,
             SOL_SOCKET,
             SO_RCVTIMEO,
             &read_timeout,
             sizeof(read_timeout));

  state->client_socket = client_socket;
}

void
network_init_server(Network_State* state) {
  i32 server_socket = socket(AF_INET, SOCK_DGRAM, 0);

  state->server_socket = server_socket;

  Socket_Address_In server_address;
  bzero((char*)&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(state->connection.server_port);

  bind(server_socket, (Socket_Address*)&server_address, sizeof(server_address));
}

bool
network_accept_players(Network_State* state) {

  Socket_Address_In client_address;
  socklen_t client_address_size = sizeof(client_address);
  char name[MAX_PLAYER_NAME_LENGTH];
  recvfrom(state->server_socket,
           name,
           MAX_PLAYER_NAME_LENGTH + 1,
           0,
           (Socket_Address*)&client_address,
           &client_address_size);

  for (i32 i = 0; i < MAX_PLAYER_COUNT; ++i) {
    Game_Client client = state->clients[i];
    if (client.address.sin_addr.s_addr == client_address.sin_addr.s_addr &&
        client.address.sin_port == client_address.sin_port) {
      return false;
    }
  }

  printf("%s connected.\n", name);

  char msg[3] = {};
  sprintf(msg, "OK");
  sendto(state->server_socket,
         msg,
         strlen(msg),
         0,
         (Socket_Address*)&client_address,
         sizeof(client_address));

  state->clients[state->connected_players].id = state->connected_players;
  state->clients[state->connected_players].address = client_address;
  state->clients[state->connected_players].address_size =
      sizeof(client_address);

  state->connected_players++;

  return state->connected_players == MAX_PLAYER_COUNT;
}

bool
network_connect_to_server(Network_State* state,
                          char name[MAX_PLAYER_NAME_LENGTH]) {

  Socket_Address_In server_address = {};
  // bzero((char*)&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(state->connection.server_ip);
  server_address.sin_port = htons(state->connection.server_port);

  sendto(state->client_socket,
         name,
         strlen(name) + 1,
         0,
         (Socket_Address*)&server_address,
         sizeof(server_address));

  char msg[3] = {};
  recvfrom(state->client_socket, msg, strlen(name), 0, NULL, NULL);

  if ((strcmp(msg, "OK")) == 0) {
    return true;
  }
  return false;
}

void
network_send_ready_message(Network_State* state) {
  for (int i = 0; i < MAX_PLAYER_COUNT; ++i) {
    Game_Client client = state->clients[i];

    char msg[6] = {};
    sprintf(msg, "READY");
    sendto(state->server_socket,
           msg,
           strlen(msg),
           0,
           (Socket_Address*)&client.address,
           client.address_size);
  }
}

bool
network_receive_ready_message(Network_State* state) {
  char msg[6] = {};
  recvfrom(state->client_socket, msg, 6, 0, NULL, NULL);

  if ((strcmp(msg, "READY")) == 0) {
    return true;
  }
  return false;
}

void
network_send_input(Network_State* state, i32 key) {

  Socket_Address_In server_address = {};
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(state->connection.server_ip);
  server_address.sin_port = htons(state->connection.server_port);

  u32 nvalue = htonl(key);

  sendto(state->client_socket,
         &nvalue,
         sizeof(u32),
         0,
         (Socket_Address*)&server_address,
         sizeof(server_address));
}

i32
network_receive_input(Network_State* state, Game_Client* client) {
  u32 key = -1;

  recvfrom(state->server_socket,
           &key,
           sizeof(u32),
           0,
           (Socket_Address*)&client->address,
           client->address_size);

  return ntohl(key);
}

void
network_broadcast(Network_State* state) {
  World* world = (World*)state->world;

  f32 players_positions[MAX_PLAYER_COUNT] = {};
  i8 players_points[MAX_PLAYER_COUNT] = {};

  players_points[0] = world->player_1.points;
  players_points[1] = world->player_2.points;

  players_positions[0] = world->player_1.y;
  players_positions[1] = world->player_2.y;

  for (int i = 0; i < MAX_PLAYER_COUNT; ++i) {
    Game_Client client = state->clients[i];

    if (client.id == -1)
      continue;

    Game_Packet packet = {};

    for (int i = 0; i < MAX_PLAYER_COUNT; ++i) {
      packet.players_positions[i] = players_positions[i];
      packet.players_points[i] = players_points[i];
    }

    packet.ball_radius = world->ball.radius;
    packet.ball_pos = world->ball.pos;

    sendto(state->server_socket,
           &packet,
           sizeof(Game_Packet),
           0,
           (Socket_Address*)&client.address,
           client.address_size);
  }
}

void
network_receive_data(Network_State* state) {

  Socket_Address_In server_address;

  bzero((char*)&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(state->connection.server_ip);
  server_address.sin_port = htons(state->connection.server_port);

  recvfrom(state->client_socket,
           &state->current_packet,
           sizeof(Game_Packet) + 1,
           0,
           NULL,
           NULL);

  // deserialize(&buffer, &state->current_packet);

  // i8 buffer[sizeof(Game_Packet)] = {};

  // recvfrom(
  //     state->client_socket, buffer, sizeof(Game_Packet) + 1, 0, NULL, NULL);

  // deserialize(&buffer, &state->current_packet);
}