#include<stdio.h>
#include <stdlib.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write

int blink_count = 0;
void blink()
{
    blink_count++;
    //todo more
}

int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , read_size;
	struct sockaddr_in server , client;
	char client_message[2000];
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	
	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	//accept connection from an incoming client
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	puts("Connection accepted");
	
	//Receive a message from client
	while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
	{
		char response_message_1[] = "LED blinked";
		char response_message_2[] = "Log: todo";
		char response_message_3[] = "Goodbuy, i am closing our connection";
		char response_message_4[] = "Goodbuy, i am closing our connection and shitting myself down";
		char response_message_error[] = "Internal server error";
		printf("message received: %s\n", client_message);
		if (strcmp(client_message,"1")==0)
		{
			printf("cmd id 1 detected. Now blinking LED.\n");
			write(client_sock , response_message_1 , strlen(response_message_1));
		}
		else if (strcmp(client_message,"2")==0)
		{
			printf("cmd id 2 detected. Sending log to client.\n");
			write(client_sock , response_message_2 , strlen(response_message_2));		
		}
		else if (strcmp(client_message,"3")==0)
		{
			printf("cmd id 3 detected. Now disconnecting client (can be handled by client it self).\n");
			write(client_sock , response_message_3 , strlen(response_message_3));		
		}
		else if (strcmp(client_message,"4")==0)
		{
			printf("cmd id 4 detected. Now disconnecting from client and shutting down the OS!.\n");
			write(client_sock , response_message_4 , strlen(response_message_4));		
		}
		else
		{
			printf("client did not enter a command id");
			//Send the message back to client
			write(client_sock , response_message_error , strlen(response_message_error));
		}
		memset(client_message, 0, sizeof(client_message));
	}
	
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
	
	return 0;
}