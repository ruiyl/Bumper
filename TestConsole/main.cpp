// main.cpp : Defines the entry point for the console application.
//

// may need #include "stdafx.h" in visual studio
#include "stdafx.h"
#include "ServerGame.h"
#include "ClientGame.h"
// used for multi-threading
#include <process.h>

void serverLoop(void *);
void clientLoop(void *);
void client2Loop(void);

ServerGame * server;
ClientGame * client;
ClientGame * client2;

int main()
{

	// initialize the server
	//server = new ServerGame();

	// create thread with arbitrary argument for the run function
    //_beginthread( serverLoop, 0, (void*)12);

    // initialize the client 
    client = new ClientGame();

	clientLoop((void*)12);
}

void clientLoop(void * arg) 
{ 
    while(true) 
    {
        client->update();
    }
}

void client2Loop()
{
    while(true)
    {
        //do game stuff
        client2->update();
    }
}
