#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <limits>
#include <iostream>
#include <cstdlib>
#include <cstring>

#define MAX_MSG 100
// 3 caract�res pour les codes ASCII 'cr', 'lf' et '\0'
#define MSG_ARRAY_SIZE (MAX_MSG+3)

using namespace std;

int main()
{
  int socketDescriptor;
  int numRead;
  unsigned short int serverPort;
  struct sockaddr_in serverAddress;
  struct hostent *hostInfo;
  struct timeval timeVal;
  fd_set readSet;
  char msg[MSG_ARRAY_SIZE];

  cout << "Entrez le nom du serveur ou son adresse IP : ";

  memset(msg, 0x0, MSG_ARRAY_SIZE);  // Mise � z�ro du tampon
  cin.get(msg, MAX_MSG, '\n');

  // gethostbyname() re�oit un nom d'h�te ou une adresse IP en notation
  // standard 4 octets en d�cimal s�par�s par des points puis renvoie un
  // pointeur sur une structure hostent. Nous avons besoin de cette structure
  // plus loin. La composition de cette structure n'est pas importante pour
  // l'instant.
  hostInfo = gethostbyname(msg);
  if (hostInfo == NULL) {
    cerr << "Probl�me dans l'interpr�tation des informations d'h�te : " << msg << "\n";
    exit(1);
  }

  cout << "Entrez le num�ro de port du serveur : ";
  cin >> serverPort;
  cin.ignore(1,'\n'); // suppression du saut de ligne

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
  memcpy((char *) &serverAddress.sin_addr.s_addr,
         hostInfo->h_addr_list[0], hostInfo->h_length);
  serverAddress.sin_port = htons(serverPort);

  cout << "\nEntrez quelques caract�res au clavier.\n";
  cout << "Le serveur les modfiera et les renverra.\n";
  cout << "Pour sortir, entrez une ligne avec le caract�re '.' uniquement\n";
  cout << "Si une ligne d�passe " << MAX_MSG << " caract�res,\n";
  cout << "seuls les " << MAX_MSG << " premiers caract�res seront utilis�s.\n\n";

  // Invite de commande pour l'utilisateur et lecture des caract�res jusqu'� la
  // limite MAX_MSG. Puis suppression du saut de ligne.
  cout << "Saisie du message : ";
  memset(msg, 0x0, MSG_ARRAY_SIZE);  // Mise � z�ro du tampon
  cin.get(msg, MAX_MSG, '\n');
  // suppression des caract�res suppl�mentaires et du saut de ligne
  cin.ignore(numeric_limits<streamsize>::max(),'\n'); 

  // Arr�t lorsque l'utilisateur saisit une ligne ne contenant qu'un point
  while (strcmp(msg, ".")) {
    // Envoi de la ligne au serveur
    if (sendto(socketDescriptor, msg, strlen(msg), 0,
               (struct sockaddr *) &serverAddress,
               sizeof(serverAddress)) < 0) {
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
      numRead = recv(socketDescriptor, msg, MAX_MSG, 0);
      if (numRead < 0) {
        cerr << "Aucune r�ponse du serveur ?\n";
        close(socketDescriptor);
        exit(1);
      }

      cout << "Message trait� : " << msg << "\n";
    }
    else {
      cout << "** Le serveur n'a r�pondu dans la seconde.\n";
    }

    // Invite de commande pour l'utilisateur et lecture des caract�res jusqu'� la
    // limite MAX_MSG. Puis suppression du saut de ligne. Comme ci-dessus.
    cout << "Saisie du message : ";
    memset(msg, 0x0, MSG_ARRAY_SIZE);  // Mise � z�ro du tampon
    cin.get(msg, MAX_MSG, '\n');
    // suppression des caract�res suppl�mentaires et du saut de ligne
    cin.ignore(numeric_limits<streamsize>::max(),'\n'); 
  }

  close(socketDescriptor);
  return 0;
}
