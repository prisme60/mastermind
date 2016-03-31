#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include "command_mgr.h"

#define MAX_MSG 255
// 3 caracteres pour les codes ASCII 'cr', 'lf' et '\0'
#define MSG_ARRAY_SIZE (MAX_MSG+3)

using namespace std;

int main() {
    int listenSocket;
    unsigned short int listenPort;
    socklen_t clientAddressLength;
    struct sockaddr_in clientAddress, serverAddress;
    char msg[MSG_ARRAY_SIZE];

    memset(msg, 0x0, MSG_ARRAY_SIZE); // Mise à zéro du tampon

    cout << "Entrez le numero de port utilisé en écoute (entre 1500 et 65000) : ";
    cin >> listenPort;

    // Création de socket en écoute et attente des requetes des clients
    listenSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (listenSocket < 0) {
        cerr << "Impossible de créer le socket en écoute\n";
        exit(1);
    }

    // On relie le socket au port en écoute.
    // On commence par initialiser les champs de la structure serverAddress puis
    // on appelle bind(). Les fonctions htonl() et htons() convertissent
    // respectivement les entiers longs et les entiers courts du rangement hote
    // (sur x86 on trouve l'octet de poids faible en premier) vers le rangement
    // réseau (octet de poids fort en premier).
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(listenPort);

    if (bind(listenSocket,
            (struct sockaddr *) &serverAddress,
            sizeof (serverAddress)) < 0) {
        cerr << "Impossible de lier le socket en écoute\n";
        exit(1);
    }

    // Attente des requetes des clients.
    // C'est un appel non-bloquant ; c'est-à-dire qu'il enregistre ce programme
    // auprès du système comme devant attendre des connexions sur ce socket avec
    // cette tache. Ensuite, l'exécution se poursuit.
    listen(listenSocket, 5);

    cout << "Attente de requete sur le port " << listenPort << "\n";

    while (1) {

        clientAddressLength = sizeof (clientAddress);

        // Mise à zéro du tampon de facon de connaitre le délimiteur
        // de fin de chaine.
        memset(msg, 0x0, MSG_ARRAY_SIZE);

        int nbReceivedByte = recvfrom(listenSocket, msg, MSG_ARRAY_SIZE, 0, (struct sockaddr *) &clientAddress, &clientAddressLength);

        if (nbReceivedByte < 0) {
            cerr << "  Problème de réception du messsage\n";
            exit(1);
        } else {
            msg[nbReceivedByte + 1] = '\0';
        }

        // Affichage de l'adresse IP du client.
        cout << "  Depuis " << inet_ntoa(clientAddress.sin_addr);

        // Affichage du numéro de port du client.
        cout << ":" << ntohs(clientAddress.sin_port) << "\n";

        // Affichage de la ligne reçue
        cout << "  Message reçu : " << msg << "\n";

        string result;
        string sMsg(msg);
        treatCommand(sMsg, result);

        // Renvoi de la ligne convertie au client.
        if (sendto(listenSocket, result.c_str(), result.size() + 1, 0,
                (struct sockaddr *) &clientAddress,
                sizeof (clientAddress)) < 0)
            cerr << "Emission du message modifié impossible\n";
    }
}
