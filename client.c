#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>

int main(int argc , char *argv[])
{
	int sock;
	struct sockaddr_in server;
	char message[1000] , server_reply[2000];
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr("192.168.1.61");
	//server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	
	puts("Connected\n");
	
	//keep communicating with server
	while(1)
	{
		printf("Command options:\n1 - Make LED blink\n2 - Get log\n3 - Disconnect\n4 - Disconnect and shut down raspberry pi\n");
		char user_input[1000];
        int cmd_to_execute = -1;
        scanf("%s" , user_input);
        //scanf uses keyboard as input. sscanf uses a string(char[]) variable as input
        //scan user_input for digits/ints and put them into cmd_to_execute
		sscanf(user_input, "%d", &cmd_to_execute);
        if (cmd_to_execute == -1)
        {
            printf("Please enter a valid integer\n");
            continue;
        } 
        else if (cmd_to_execute != 1 && cmd_to_execute != 2 && cmd_to_execute != 3 && cmd_to_execute != 4)
        {
            printf("Please enter 1, 2, 3 or 4!\n");
            continue;
        }
        //string printing 
        sprintf(message, "%d", cmd_to_execute);
        printf("Sending command %d to server\n",cmd_to_execute);
		//Send some data
		if( send(sock , message , strlen(message) , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		
		//Receive a reply from the server
		if( recv(sock , server_reply , 2000 , 0) < 0)
		{
			puts("recv failed");
			break;
		}
		
		puts("Server reply :");
		puts(server_reply);
		//clear reply string (0 is replacing all characters(unasigning) in server_reply up to third parameter)
		memset(server_reply, 0, sizeof server_reply);
	}
	
	close(sock);
	return 0;
}