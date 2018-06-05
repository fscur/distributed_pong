#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

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
network_init(Network_State* state) {}

void
set_recvfrom_timeout(i32 socket, i32 seconds, i32 microseconds) {
  struct timeval read_timeout;
  read_timeout.tv_sec = seconds;
  read_timeout.tv_usec = microseconds;
  setsockopt(
      socket, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout));
}

void
network_init_client(Network_State* state) {
  i32 client_socket = socket(AF_INET, SOCK_DGRAM, 0);
  state->client_socket = client_socket;
  set_recvfrom_timeout(state->client_socket, 0, 10);
}

void
network_init_server(Network_State* state) {
  for (int i = 0; i < MAX_PLAYER_COUNT; ++i) {
    state->clients[i].id = -1;
  }

  i32 server_socket = socket(AF_INET, SOCK_DGRAM, 0);

  state->server_socket = server_socket;

  Socket_Address_In server_address = {};
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(state->connection.server_port);

  bind(server_socket, (Socket_Address*)&server_address, sizeof(server_address));
}

u32
read_cmd(i32 socket, Socket_Address* address, socklen_t* address_size) {
  u32 n_cmd = 0;
  recvfrom(socket, &n_cmd, CMD_SIZE, 0, address, address_size);
  return ntohl(n_cmd);
}

void
send_cmd(i32 socket, u32 cmd, Socket_Address* address, socklen_t address_size) {
  u32 n_cmd = htonl(cmd);
  sendto(socket, &n_cmd, CMD_SIZE, 0, address, address_size);
}

void
network_clear_input_buffer(i32 socket) {
  set_recvfrom_timeout(socket, 0, 10);

  Socket_Address_In address = {};
  socklen_t address_size = sizeof(address);

  do {
    bzero(&address, address_size);
    read_cmd(socket, (Socket_Address*)&address, &address_size);
  } while (address.sin_addr.s_addr != 0 && address.sin_port != 0);

  set_recvfrom_timeout(socket, 0, 0);
}

bool
network_accept_players(Network_State* state) {

  while (state->connected_players < MAX_PLAYER_COUNT) {

    Socket_Address_In client_address = {};
    socklen_t client_address_size = sizeof(client_address);

    set_recvfrom_timeout(state->server_socket, 0, 10);

    u32 cmd = read_cmd(state->server_socket,
                       (Socket_Address*)&client_address,
                       &client_address_size);

    Game_Client* current_client = NULL;

    for (i32 i = 0; i < MAX_PLAYER_COUNT; ++i) {
      Game_Client client = state->clients[i];
      if (client.status == CLIENT_STATUS_CONNECTED)
        continue;

      if (client_address.sin_addr.s_addr == 0 && client_address.sin_port == 0) {
        current_client = &state->clients[i];
        break;
      } else if (client.address.sin_addr.s_addr == 0 &&
                 client.address.sin_port == 0) {
        current_client = &state->clients[state->connected_players];
        current_client->id = state->connected_players;
        current_client->address = client_address;
        current_client->address_size = client_address_size;
        break;
      } else if (client.address.sin_addr.s_addr ==
                     client_address.sin_addr.s_addr &&
                 client.address.sin_port == client_address.sin_port) {
        current_client = &state->clients[i];
        break;
      }
    }

    if (current_client == NULL || current_client->id == -1)
      continue;

    switch (cmd) {
    case 0: {
      if (current_client->status == CLIENT_STATUS_WAITING) {
        send_cmd(state->server_socket,
                 CMD_NAME,
                 (Socket_Address*)&current_client->address,
                 current_client->address_size);

        char name[MAX_PLAYER_NAME_LENGTH] = {};
        recvfrom(state->server_socket,
                 name,
                 MAX_PLAYER_NAME_LENGTH + 1,
                 0,
                 NULL,
                 NULL);

        printf("%s connected.\n", name);
        memcpy(current_client->name, name, strlen(name));

        u32 n_id = htonl(state->connected_players);
        sendto(state->server_socket,
               &n_id,
               sizeof(u32),
               0,
               (Socket_Address*)&current_client->address,
               current_client->address_size);

        current_client->status = CLIENT_STATUS_CONNECTED;
        state->connected_players++;
      }
      continue;
    }
    case CMD_CONNECT: {

      if (current_client->status != CLIENT_STATUS_DISCONNECTED)
        continue;
      else {
        send_cmd(state->server_socket,
                 CMD_WAIT,
                 (Socket_Address*)&current_client->address,
                 current_client->address_size);
        current_client->status = CLIENT_STATUS_WAITING;
      }
      continue;
    }
    }
  }
  set_recvfrom_timeout(state->server_socket, 0, 0);
  return true;
}

bool
network_connect_to_server(Network_State* state,
                          char name[MAX_PLAYER_NAME_LENGTH],
                          u32* id) {
  Socket_Address_In server_address = {};
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(state->connection.server_ip);
  server_address.sin_port = htons(state->connection.server_port);

  send_cmd(state->client_socket,
           CMD_CONNECT,
           (Socket_Address*)&server_address,
           sizeof(server_address));

  u32 cmd = read_cmd(state->client_socket, NULL, NULL);

  if (cmd == CMD_WAIT) {
    set_recvfrom_timeout(state->client_socket, 0, 0);

    cmd = read_cmd(state->client_socket, NULL, NULL);
    sendto(state->client_socket,
           name,
           strlen(name) + 1,
           0,
           (Socket_Address*)&server_address,
           sizeof(server_address));

    u32 tmp_id;
    recvfrom(state->client_socket, &tmp_id, sizeof(u32), 0, NULL, NULL);
    *id = ntohl(tmp_id);

    set_recvfrom_timeout(state->client_socket, 0, 10);

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
           6,
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
network_send_game_over_message(Network_State* state, Game_Client* client) {
  for (int i = 0; i < MAX_PLAYER_COUNT; ++i) {
    Game_Client client = state->clients[i];

    char msg[9] = {};
    sprintf(msg, "GAMEOVER");
    sendto(state->server_socket,
           msg,
           9,
           0,
           (Socket_Address*)&client.address,
           client.address_size);
  }
}

bool
network_receive_game_over_message(Network_State* state) {
  char msg[9] = {};
  recvfrom(state->client_socket, msg, 9, 0, NULL, NULL);

  if ((strcmp(msg, "GAMEOVER")) == 0) {
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
           &client->address_size);

  return ntohl(key);
}

void
network_broadcast(Network_State* state) {
  World* world = (World*)state->world;

  for (int i = 0; i < MAX_PLAYER_COUNT; ++i) {
    Game_Client client = state->clients[i];

    if (client.id == -1)
      continue;

    Game_Packet packet = {};

    for (int i = 0; i < MAX_PLAYER_COUNT; ++i) {
      memcpy(packet.players_names[i],
             world->players[i].name,
             MAX_PLAYER_NAME_LENGTH);
      packet.players_positions[i] = world->players[i].y;
      packet.players_points[i] = world->players[i].points;
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

  recvfrom(state->client_socket,
           &state->current_packet,
           sizeof(Game_Packet) + 1,
           0,
           NULL,
           NULL);
}