#include <iostream>
#include <cstring>
#include "nodeFactory.h"

using namespace dbserver;

#define NB_POSITION 3

//TODO 1) Add a method in order to transform unit data path string and combi
// so we have the following input:
// c4p6b#w#[1,2,0,3,3,0]b1w2[0,1,2,3,2,1]b3w1[1,1,1,1,1,1]...b6w0[solution]
//
// gives:
// nbColor = 4
// nbPosition = 6
// b#w# ==> root node with no correction
// b1w2 ==> correct black=1 white=2
// ...
//[1,2,0,3,3,0] combinaison 0

int main(int argv, char *args[])
{
	U32 nbColor = 4;
	U32 nbPosition = NB_POSITION;
	char buffer[200];
	memset(buffer,0,200);
	char *pBufferCurrentposition;
	U8 combinaisons[50][NB_POSITION];
	NodeFactory node(nbColor, nbPosition);
	U32 sizeofNodeCombi = sizeof(node_path_unit);
	node_path_unit * unitDataPath;
	pBufferCurrentposition = buffer;

	unitDataPath = reinterpret_cast<node_path_unit*>(pBufferCurrentposition);
	unitDataPath->correction_black = UNUSED_CORRECTION;
	unitDataPath->correction_white = UNUSED_CORRECTION;
	cerr << hex;
	cerr << "address unitDataPath black\t" << reinterpret_cast<int>(&unitDataPath->correction_black) << "\t-->\t" << static_cast<int>(unitDataPath->correction_black) << endl;
	cerr << "address unitDataPath white\t" << reinterpret_cast<int>(&unitDataPath->correction_white) << "\t-->\t" << static_cast<int>(unitDataPath->correction_white) << endl;
	combinaisons[0][0]=1;combinaisons[0][1]=1;combinaisons[0][2]=0;
	cerr << hex <<"address comb00: " << reinterpret_cast<int>(&combinaisons[0][0]) << "\t-->\t" << static_cast<int>(combinaisons[0][0]) << endl;
	cerr << hex <<"address comb01: " << reinterpret_cast<int>(&combinaisons[0][1]) << "\t-->\t" << static_cast<int>(combinaisons[0][1]) << endl;
	cerr << hex <<"address comb02: " << reinterpret_cast<int>(&combinaisons[0][2]) << "\t-->\t" << static_cast<int>(combinaisons[0][2]) << endl;
	pBufferCurrentposition += sizeofNodeCombi;
	
	unitDataPath = reinterpret_cast<node_path_unit*>(pBufferCurrentposition);
	unitDataPath->correction_black = 1;
	unitDataPath->correction_white = 1;
	cerr << hex;
	cerr << "address unitDataPath black\t" << reinterpret_cast<int>(&unitDataPath->correction_black) << "\t-->\t" << static_cast<int>(unitDataPath->correction_black) << endl;
	cerr << "address unitDataPath white\t" << reinterpret_cast<int>(&unitDataPath->correction_white) << "\t-->\t" << static_cast<int>(unitDataPath->correction_white) << endl;
	combinaisons[0][0]=3;combinaisons[0][1]=3;combinaisons[0][2]=3;
	cerr << hex <<"address comb00: " << reinterpret_cast<int>(&combinaisons[0][0]) << "\t-->\t" << static_cast<int>(combinaisons[0][0]) << endl;
	cerr << hex <<"address comb01: " << reinterpret_cast<int>(&combinaisons[0][1]) << "\t-->\t" << static_cast<int>(combinaisons[0][1]) << endl;
	cerr << hex <<"address comb02: " << reinterpret_cast<int>(&combinaisons[0][2]) << "\t-->\t" << static_cast<int>(combinaisons[0][2]) << endl;
	pBufferCurrentposition += sizeofNodeCombi;
	
	unitDataPath = reinterpret_cast<node_path_unit*>(pBufferCurrentposition);
	unitDataPath->correction_black = 0;
	unitDataPath->correction_white = 2;
	cerr << "address unitDataPath black\t" << reinterpret_cast<int>(&unitDataPath->correction_black) << "\t-->\t" << static_cast<int>(unitDataPath->correction_black) << endl;
	cerr << "address unitDataPath white\t" << reinterpret_cast<int>(&unitDataPath->correction_white) << "\t-->\t" << static_cast<int>(unitDataPath->correction_white) << endl;
	combinaisons[1][0]=2;combinaisons[1][1]=1;combinaisons[1][2]=0;
	cerr << hex <<"address comb10: " << reinterpret_cast<int>(&combinaisons[1][0]) << "\t-->\t" << static_cast<int>(combinaisons[1][0]) << endl;
	cerr << hex <<"address comb11: " << reinterpret_cast<int>(&combinaisons[1][1]) << "\t-->\t" << static_cast<int>(combinaisons[1][1]) << endl;
	cerr << hex <<"address comb12: " << reinterpret_cast<int>(&combinaisons[1][2]) << "\t-->\t" << static_cast<int>(combinaisons[1][2]) << endl;
	pBufferCurrentposition += sizeofNodeCombi;
	
	unitDataPath = reinterpret_cast<node_path_unit*>(pBufferCurrentposition);
	unitDataPath->correction_black = 2;
	unitDataPath->correction_white = 0;
	cerr << "address unitDataPath black\t" << reinterpret_cast<int>(&unitDataPath->correction_black) << "\t-->\t" << static_cast<int>(unitDataPath->correction_black) << endl;
	cerr << "address unitDataPath white\t" << reinterpret_cast<int>(&unitDataPath->correction_white) << "\t-->\t" << static_cast<int>(unitDataPath->correction_white) << endl;
	combinaisons[2][0]=1;combinaisons[2][1]=1;combinaisons[2][2]=1;
	cerr << hex <<"address comb20: " << reinterpret_cast<int>(&combinaisons[2][0]) << "\t-->\t" << static_cast<int>(combinaisons[2][0]) << endl;
	cerr << hex <<"address comb21: " << reinterpret_cast<int>(&combinaisons[2][1]) << "\t-->\t" << static_cast<int>(combinaisons[2][1]) << endl;
	cerr << hex <<"address comb22: " << reinterpret_cast<int>(&combinaisons[2][2]) << "\t-->\t" << static_cast<int>(combinaisons[2][2]) << endl;
	pBufferCurrentposition += sizeofNodeCombi;
	
	cerr << "dump unitDataPath\t" << hex;
	for(int i=0;i<200;i++) cerr << ((static_cast<int>(buffer[i]))&0xFF) << " " ;
	cerr << endl;
	
	// cerr << "dump combi\t" << hex;
	// for(int i=0;i<12;i++) cerr << ((static_cast<int>(combinaisons[0][i]))&0xFF) << " " ;
	// cerr << endl;
	
	unitDataPath = reinterpret_cast<node_path_unit*>(buffer);
	if(node.fillNodeInit(4, unitDataPath, reinterpret_cast<U8*>(combinaisons)))
	{
		cerr << "fillNodeInit method returns true" << endl;
	}
	else
	{
		cerr << "fillNodeInit method returns false" << endl;
	}
	
	// cerr << "dump combi\t" << hex;
	// for(int i=0;i<12;i++) cerr << ((static_cast<int>(combinaisons[0][i]))&0xFF) << " " ;
	// cerr << endl;	
	
}