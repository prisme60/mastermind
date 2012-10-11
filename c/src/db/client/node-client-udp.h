#ifndef __NODE_CLIENT_UDP_H__
#define __NODE_CLIENT_UDP_H__

#include <netdb.h>

#include <string>

#define MAX_MSG 100
// 3 caractères pour les codes ASCII 'cr', 'lf' et '\0'
#define MSG_ARRAY_SIZE (MAX_MSG+3)

namespace dbclient
{
	class nodeClientUdp
	{
		public:
		nodeClientUdp(std::string _host="localhost", int _serverPort=1500);
		virtual ~nodeClientUdp();
		
		bool sendCommand(std::string command, std::string &result);
		
		private:
		std::string host;

		int socketDescriptor;
		unsigned short int serverPort;
		struct sockaddr_in serverAddress;
	};
}
#endif