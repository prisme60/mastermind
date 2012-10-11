#include <netinet/in.h>
#include <unistd.h>
#include <limits>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include "node-client-udp.h"

using namespace std;
namespace dbclient
{

	// int main()
	// {
		// nodeClientUdp cli("localhost", 1500);
		// string result;
		// string request[] = {
		// "set p4c5  [0, 1,2,3]b#w#  [1,1, 1  , 1]b2w0  [2,2,2,2]b3w1  [4,3,2,1]b2w2",
		// "set p4c5  [0,1,2,3]b#w#  [0,0,0,0]b2w3",
		// "get p4c5 b#w#",
		// "get p4c5 b#w# b2w0",
		// "get p4c5 b#w# b2w3",
		// "get p4c5 b#w# b2w0",
		// "get p4c5 b#w# b2w3 b3w1",// <---Can't be found
		// "get p4c5 b#w# b2w0 b3w1",
		// "get p4c5 b#w# b2w3 b3w2",// <---Can't be found
		// "get p4c5 b#w# b2w0 b3w1 b2w2",
		// "get p4c5 b#w# b2w0 b3w1",
		// "set p5c5  [0,1,2,3,4]b#w#  [1,1,1,1,1]b2w0  [2,2,2,2,2]b3w1  [4,3,2,1,0]b2w2",
		// "set p5c5  [0,1,2,3,4]b#w#  [0,0,0,0,0]b2w3",
		// "get p5c5 b#w#",
		// "get p5c5 b#w# b2w0",
		// "get p5c5 b#w# b2w3",
		// "get p5c5 b#w# b2w0",
		// "get p5c5 b#w# b2w3 b3w1",// <---Can't be found
		// "get p5c5 b#w# b2w0 b3w1",
		// "get p5c5 b#w# b2w3 b3w2",// <---Can't be found
		// "get p5c5 b#w# b2w0 b3w1 b2w2",
		// "get p5c5 b#w# b2w0 b3w1",
		// };
		
		// for(int i=0; i<(sizeof(request)/sizeof(string)) ; i++)
		// {
			// cout << request[i] ;
			// if(cli.sendCommand(request[i], result))
			// {
				// cerr << "//" << result << endl;
			// }
		// }
		// return 0;
	// }

	nodeClientUdp::nodeClientUdp(std::string _host, int _serverPort):host(_host),serverPort(_serverPort)
	{
		struct hostent *hostInfo;
		// gethostbyname() re�oit un nom d'h�te ou une adresse IP en notation
		// standard 4 octets en d�cimal s�par�s par des points puis renvoie un
		// pointeur sur une structure hostent. Nous avons besoin de cette structure
		// plus loin. La composition de cette structure n'est pas importante pour
		// l'instant.
		hostInfo = gethostbyname(_host.c_str());
		if (hostInfo == NULL) {
			cerr << "Probl�me dans l'interpr�tation des informations d'h�te : " << host << "\n";
			exit(1);
		}
		
		// Cr�ation de socket. "AF_INET" correspond � l'utilisation du protocole IPv4
		// au niveau r�seau. "SOCK_DGRAM" correspond � l'utilisation du protocole UDP
		// au niveau transport. La valeur 0 indique qu'un seul protocole sera utilis�
		// avec ce socket.
		socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
		if (socketDescriptor < 0) {
			cerr << "Impossible de cr�er le socket\n";
			exit(1);
		}
		
		// Initialisation des champs de la structure serverAddress
		serverAddress.sin_family = hostInfo->h_addrtype;
		memcpy((char *) &serverAddress.sin_addr.s_addr,	hostInfo->h_addr_list[0], hostInfo->h_length);
		serverAddress.sin_port = htons(serverPort);

	}

	nodeClientUdp::~nodeClientUdp()
	{
		close(socketDescriptor);
	}

	bool nodeClientUdp::sendCommand(string command, string &result)
	{
		bool bResult = false;
		struct timeval timeVal;
		fd_set readSet;
		char msg[MSG_ARRAY_SIZE];
		int length = command.size();
		
		assert(length<MSG_ARRAY_SIZE);
		if(length<MSG_ARRAY_SIZE)
		{
			memcpy(msg, command.c_str(),length);
			// Envoi de la ligne au serveur
			if (sendto(socketDescriptor, msg, length, 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
				cerr << "�mission du message impossible\n";
				close(socketDescriptor);
				exit(1);
			}

			// Attente de la r�ponse pendant une seconde.
			FD_ZERO(&readSet);
			FD_SET(socketDescriptor, &readSet);
			timeVal.tv_sec = 1;
			timeVal.tv_usec = 0;

			if (select(socketDescriptor+1, &readSet, NULL, NULL, &timeVal)) {
				// Lecture de la ligne modifi�e par le serveur.
				memset(msg, 0x0, MSG_ARRAY_SIZE);  // Mise � z�ro du tampon
				int numRead = recv(socketDescriptor, msg, MAX_MSG, 0);
				if (numRead < 0) {
					cerr << "Aucune r�ponse du serveur ?\n";
					close(socketDescriptor);
					exit(1);
				}
				else {
					//cout << "Message trait� : " << msg << "\n";
					result = msg;
					bResult = true;
				}
			}
			else {
			  cout << "** Le serveur n'a r�pondu dans la seconde.\n";
			}
		}
		return bResult;
	}
}