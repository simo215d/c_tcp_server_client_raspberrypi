//these are libraries. 
//if <> the compiler will search the system directory
//if "" the compiler will search the local directory first and then the system directory. (so these are selfmade files in the program)
#include<stdio.h>
#include <stdlib.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<wiringPi.h> //this is the library for accessing the GPIO pins on the pi and turning them on/off
#define LED 18
int blink_count = 0;
void blink()
{
	//This must be called before anything else – it opens the GPIO devices and allows our program to access it.
	wiringPiSetupGpio();
	//This sets the mode of the pin – usually in or out, but there are some other functions too.
	pinMode(LED, OUTPUT);
	//turn on/off
	digitalWrite(LED,1);
	delay(3000);
	digitalWrite(LED,0);
	blink_count++;
}

//main function gets called upon execution of program with arguments if any
int main(/*int argc , char *argv[]*/)
{
	int socket_desc , client_sock_desc , c_addr_len , read_size;
	//both server and client are of struct sockaddr_in. sockaddr_in is a basic structure with info used for functions about internet addresses
	struct sockaddr_in server , client;
	char client_message[2000];
	
	//Create socket
	//socket constructor returns an integer which is a reference to a file descriptor.
	//file descriptors are used by Unix systems to help handle I/O for streams/files.
	//First parameter is communication domain/protocol. AF_INET is just IPv4
	//Second parameter is the connection type. SOCK_STREAM is standard for 2 way byte stream communication
	//Third parameter is protocol. Some connection types have several diffent communication protocols. But if only 1 protocol exists then 0 is default protocol.
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//The address family for the transport address. AF_INET is just IPv4
	server.sin_family = AF_INET;
	//This variable is about what network interfaces to bind to. 
	//Network interfaces can be physical like on a network inteface card. Or virtual like loopback/localhost.
	//INADDR_ANY simply implies that ALL of these interfaces will be bound with this socket, so they all work.
	//Further it also means that all of this machines ip addresses will be bound with this socket, so they all work afaik.
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Binding just assigns the socket with an address (local in this case)
	//& means that we are giving the reference/address of our variable. If no "&" then it means we give the value of the variable.
	//about pointers: We use pointers because it's easier to give someone an address to your home than to give a copy of your home to everyone.
	//Second parameter is a sockaddr. We can cast the sockaddr_in server to a sockaddr and still preserve ip and port. This just gets converted to some byte stuff within the sockaddr afaik.
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	
	//Listen doesnt activate the socket, but it marks it as a socket used as a passive socket to accept requests
	//First parameter is which socket (file descriptor) we mark as a listener
	//Second is backlog which is how many pending clients/connections we allow in the queue. If client tries to connect beyond that point it just refuses
	listen(socket_desc , 3);
	puts("Waiting for incoming connections...");

	//Accept and incoming connection
	//Accept will take the first connection request in queue or will block program until a new connection request apears
	//First parameter is just what socket we are listening with
	//Second parameter is the address of client which will be filled within accept method. Use "0" here and third if you dont want client address
	//Second parameter takes a pointer variable so we use the "&" sign. Our client struct is of type sockaddr_in so we cast it to a sockaddr with the "*" sign, because the parameter requests a pointer
	//Third parameter can be 0 if we dont want client address. This param takes a pointer of a length variable of client address structure which it will fill within the method.
	//If third parameter return size(actual address length of client) is larger than the address structure, then the information within the sockaddr will be truncated(forkortet). 
	//c_addr_len represents how big this struct sockaddr_in is by its individual variables (and a bit more). So if the return size is larger then it probably means that it could not fit inside this structure which means it gets truncated?
	c_addr_len = sizeof(struct sockaddr_in);
	client_sock_desc = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c_addr_len);
	if (client_sock_desc < 0)
	{
		perror("accept failed");
		return 1;
	}
	puts("Connection accepted");
	//we have to reset the value of the message variable so that it a shorter message next loop doesnt include chars from a longer previous loop
	memset(client_message, 0, sizeof(client_message));
	//Receive a message from client
	while( (read_size = recv(client_sock_desc , client_message , 2000 , 0)) > 0 )
	{
		char response_message_1[] = "LED blinked";
		char response_message_2[] = "Log: todo";
		char response_message_3[] = "Goodbuy, i am closing our connection";
		char response_message_4[] = "Goodbuy, i am closing our connection and shutting myself down";
		char response_message_error[] = "Internal server error";
		printf("message received: %s\n", client_message);
		if (strcmp(client_message,"1")==0)
		{
			printf("cmd id 1 detected. Now blinking LED.\n");
			blink();
			write(client_sock_desc , response_message_1 , strlen(response_message_1));
		}
		else if (strcmp(client_message,"2")==0)
		{
			printf("cmd id 2 detected. Sending log to client.\n");
			write(client_sock_desc , response_message_2 , strlen(response_message_2));		
		}
		else if (strcmp(client_message,"3")==0)
		{
			printf("cmd id 3 detected. Now disconnecting client (can be handled by client it self).\n");
			write(client_sock_desc , response_message_3 , strlen(response_message_3));		
		}
		else if (strcmp(client_message,"4")==0)
		{
			printf("cmd id 4 detected. Now disconnecting from client and shutting down the OS!.\n");
			write(client_sock_desc , response_message_4 , strlen(response_message_4));		
		}
		else
		{
			printf("client did not enter a command id\n");
			//Send the message back to client
			write(client_sock_desc , response_message_error , strlen(response_message_error));
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