#include <string.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>
#include <arpa/inet.h> //inet_addr

using namespace std;

void *ftch(void *);
void *snd(void *);
int client_socket;
int getSize;
/*
    TCP Client class
*/
class TcpClient
{
    private: 
        std::string address;
        string response_data = "";
        int port;
        struct sockaddr_in server;
    public:
        int sock;
        TcpClient();
        bool conn(string, int);
        bool send_data(string data);
        string receive(int);
};

/*
    constructor
*/
TcpClient::TcpClient()
{
    sock = -1;
    port = 0;
    address = "";
}
/*
    Connect to a host on a certain port number
*/
bool TcpClient::conn(string address , int port)
{
    // create socket if it is not already created
    if(sock == -1)
    {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
        }

        cout<<"Socket created\n";
    }
    else { /* OK , nothing */ }

    // setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;

        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            //gethostbyname failed
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";

            return false;
        }

        // Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;

        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];

            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;

            break;
        }
    }

    //plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }

    server.sin_family = AF_INET;
    server.sin_port = htons( port );

    //Connect to remote server
    if( connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0 )
    {
        perror("connect failed. Error");
        return false;
    }

    cout<<"Connected\n";
    return true;
}

TcpClient c;
int main(int argc,char *argv[])
{
    pthread_t procThread[3];
    
    string host;

    cout<<"Enter hostname : ";
    cin>>host;

    //connect to host
    c.conn(host , 8086);	

	int thread_no=0; // using as iterator for procthread array
	while(thread_no<3)
	{
		
		//Creating a thread for sending and receiving msg and updating the terminating condition
		pthread_create(&procThread[thread_no], NULL, ftch, NULL); 
        thread_no++;
        pthread_create(&procThread[thread_no], NULL, snd, NULL); 
        thread_no++;
	}	
	// WAITING FOR A THREAD TO TERMINATE...SOMETHING LIKE A QUEUE :)
	for(int idx=0;idx<3;idx++)
	{
		pthread_join(procThread[idx],NULL);
	} 

}

void *snd(void *dummy)
{
	while(true)
	{
		char msg[1024];
		cout<<"\rME> ";
		memset(&msg,0,sizeof msg);   //IF NOT WORK USE bzero(msg,1024)
		cin.getline(msg,1024);
		
		send(c.sock,msg, strlen(msg),0);
		
		string message(msg);
		if(message=="QUIT \\r\\n")
		break;
	}
	cout<<"Closing connection..."<<endl;
	cout<<"Closed!"<<endl;
	close(c.sock);

	exit(0);
}
void *ftch(void *dummy)
{
    char buffer[1024];
    string reply;

	while(true)
	{
        //Receive a reply from the server
        if( getSize = recv(c.sock, buffer, sizeof(buffer) , 0) < 0)
        {
            cout << "\rRecieve Failed" << endl; 
            break; 
        }else
        if ( reply =="-ERR An error occured\r\n")
        {
            break;
        }else

        reply = buffer;
        cout <<  "\rOtherUser> "  <<reply<< endl;
        cout << "Me> ";
        fflush(stdout);

        memset(buffer, 0, sizeof(buffer));
    }
    cout << "\nClosing thread and connection..." << endl;
    cout << "Closed!" << endl;
    close(c.sock);
    exit(0);
}
