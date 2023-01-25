#include "process.h"
using namespace std;


int thread_count = 0; // counts number of logged in users
vector<mailAccounts>listUser; //stores list of users email
sem_t mutex;

//this function loads user details into a vector called list users. 
void loadMailserver()
{
	int counter = 0;
	string directory;
	/**DOMAIN_ROOT_PATH contains all mail folders. 
	In it are other folders labelled emails of users ,
	which we would store in the list user vector **/
    for (auto const& dir_entry : std::filesystem::directory_iterator{DOMAIN_ROOT_PATH}) 
    {
		if(fs::is_directory(dir_entry.path()))
		{
			directory = dir_entry.path();
			listUser.push_back({counter, 0, false, directory.substr(directory.find('/') + 1)});
		}
    }
}
void *connection_handler(void *socket_desc)
{
	int newSock = *((int *)socket_desc);
    int request;
    char client_message[client_message_SIZE];

	//create a pop3user object for the current user, and send a greeting back.
	pop3User User(newSock);
	char greetings[] ="Welcome to the Pop3 webmail";
	User.SendResponse(POP3_DEFAULT_AFFERMATIVE_RESPONSE, greetings);
	/** Global variable thread count, stores number of signed users.
	we increment it once a client connects. we also place it in a mutex lock,
	to prevent race condition. **/

	sem_wait(&mutex);
	thread_count++; 
	printf(" %d Users currently online \n",thread_count);

	//optional, we can limit the number of logged in users.
	if(thread_count > 20)
	{
		char error[] ="Reached the max limit of connected users";
		User.SendResponse(POP3_DEFAULT_NEGATIVE_RESPONSE, error);
		thread_count--; 
		sem_post(&mutex);
		close(newSock);
		pthread_exit(NULL);
	}
	sem_post(&mutex);
	while(request = recv(User.get_clientSocket(), client_message, sizeof(client_message), 0) > 0)
	{
		if(ProcessCMD(User, client_message) == -1)
		{
			printf("Connection thread closing...\n");
			break;
		}
		memset(client_message, 0, sizeof(client_message));
	}
	if (request < 0) 
	{
		puts("Receive failed");
	}
	else if (request == 0)
	{
		puts("Client disconnected unexpectedly.");
	}		
	thread_count--; 
	close(newSock);
	pthread_exit(NULL);
}

//the server class
class connectServer
{
	private:
		int randomV = POP3_PORT; //the server port
		struct sockaddr_in server_address, client_address;
		int server_socket, client_socket, recvData, *thread_sock;
		char ip4[INET_ADDRSTRLEN]; // holds the client ip address
		socklen_t len; //length of the ip address

	public:
		connectServer();
		void threadHandler();
};

connectServer::connectServer()
{
    server_socket = socket(AF_INET, SOCK_STREAM, 0); //call the socket object
    if (server_socket <= 0)
    {
        perror("In sockets");
        exit(EXIT_FAILURE);
    }
	memset(&server_address, 0, sizeof server_address); 
	server_address.sin_family=AF_INET;
	server_address.sin_addr.s_addr=htonl(INADDR_ANY);
	server_address.sin_port = htons(POP3_PORT);

	//In a case the tcp port is used by another application, use a while loop to generate random 
	//port numbers until, the socket binds to the random port. This is used during 
	//development for testing, since the standard tcp client connects to port :110.
	while (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address))<0)
	{   randomV = 8080 + (rand() % 10);
        memset(&server_address, 0, sizeof server_address); 
        server_address.sin_family=AF_INET;
        server_address.sin_addr.s_addr=htonl(INADDR_ANY);
        server_address.sin_port = htons(randomV);
        
	}
}
void connectServer::threadHandler()
{
    if (listen(server_socket, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
	//Use the infinite loop to accept new connections .
    while(1)
    { 
		len= sizeof(client_address);
		printf("Listening on TCP POP3_PORT 110: %d \n", randomV);
		
		client_socket = accept(server_socket,(struct sockaddr *)&client_address,&len);
		if(client_socket<0)
		{
			perror("Unable to accept connection");
		}
		else
		{
            inet_ntop(AF_INET, &(client_address.sin_addr), ip4, INET_ADDRSTRLEN);
			printf("Connected to ipaddress: %s\n", ip4);
		}
        pthread_t multi_thread;
        thread_sock = new int(); 
        *thread_sock = client_socket;  
		//call the thread handler
        if (pthread_create(&multi_thread, NULL, connection_handler, (void *)thread_sock) > 0) 
        {
            perror("Could not create thread");
        }           
	}
}
int main(int argc, char const *argv[])
{
	sem_init(&mutex, 0, 1); //initialize the thread mutex lock
	connectServer sserver; //declare the server object
	loadMailserver(); //loads details of our pop3 server on startup
	sserver.threadHandler(); //call the thread handler
}
