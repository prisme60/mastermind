#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>

#include "node_struct.h"
#include "nodeFactory.h"

using namespace std;

#define TEST() cerr << dec << __LINE__ << hex << " : " << file_stream.tellp() << "\tfail:" << file_stream.fail() << "\tbad:" << file_stream.bad() << endl;

namespace dbserver
{
	NodeFactory::NodeFactory(U32 nbColor, U32 nbPosition):nbColor(nbColor),nbPosition(nbPosition)
	{
		stringstream filename;
		filename << "p" << nbPosition << "c" << nbColor << ".db";
		cerr << "Open file:" << filename.str() << endl;
		file_stream.open(filename.str().c_str(), ios::binary | ios_base::out | ios_base::in);
		//check if the file exists
		if(file_stream.fail())
		{
			cerr << "CAN'T open the file (try to create it)" << endl;
			file_stream.open (filename.str().c_str(), ios::trunc | ios::binary | ios_base::out | ios_base::in);
			if(file_stream.fail())
				cerr << "CAN'T create the file" << endl;
			else
				cerr << "file has been created" << endl;
		}
		else
			cerr << "file has been open" << endl;
		sizeOf_node_combi = mGetSizeOf_node_combi(nbPosition);
	}
	
	bool NodeFactory::findNodeInit(int nbOfElement, node_path_unit unitDataPath[], node_combi &nodeCombi_result)
	{
		cerr << "findNodeInit(nbOfElement=" << nbOfElement << endl;
		extra_info_struct extra;
		return findNode(UNKNOWN_NODE_ID, nbOfElement, unitDataPath, nodeCombi_result, extra);
	}
	
	bool NodeFactory::findNode(U32 parentnode_id, int nbOfElement, node_path_unit unitDataPath[], node_combi &nodeCombi_result, extra_info_struct &extra)
	{
		cerr << "findNode(parentNode=" << parentnode_id << " ,nbOfElement=" << nbOfElement << endl;
		bool bResult = false;
		if(parentnode_id == UNKNOWN_NODE_ID)
		{
			file_stream.seekg(0,ios::beg);TEST()
			extra.nbTotalElement = nbOfElement;
		}
		if(nbOfElement>0)
		{
			while(file_stream.good())
			{
				cerr << "read" << endl;TEST() 
				file_stream.read(reinterpret_cast<char*>(&nodeCombi_result),sizeOf_node_combi);TEST() 
				if(file_stream.gcount()==sizeOf_node_combi)
				{//we should never read other size than sizeOf_node_combi or 0 (eof), otherwise it means that the file is corrupted
					if((nodeCombi_result.parentnode_id == parentnode_id) &&
						(nodeCombi_result.correction_black == unitDataPath[0].correction_black) &&
						(nodeCombi_result.correction_white == unitDataPath[0].correction_white))
					{// node with the id has been found
						bResult=true;
						cerr << "FIND NODE ==> node_id=" << nodeCombi_result.node_id << "\tparentnode_id=" << nodeCombi_result.parentnode_id << "\tcorrection_black=" << nodeCombi_result.correction_black << "\tcorrection_white=" << nodeCombi_result.correction_white <<endl;
						for(U32 i=0;i<nbPosition;i++) {cerr << "[" << hex << static_cast<int>(nodeCombi_result.combinaison[i]) << "]";}
						cerr << endl;
						break;
					}
				}
			}
			file_stream.clear();
		}
		if(bResult)
		{//if the parent node has been found, we can try to search a child node!
			if(nbOfElement>1) //if there is still at least one node in the path, we continue searching.
				bResult = findNode(nodeCombi_result.node_id, nbOfElement - 1, unitDataPath+1, nodeCombi_result, extra);
		}
		else
		{//here the searched node, that it is not present in the database
			nodeCombi_result.node_id = UNKNOWN_NODE_ID;//node_id is unknown
			nodeCombi_result.parentnode_id = parentnode_id;
			nodeCombi_result.correction_black = unitDataPath[0].correction_black;
			nodeCombi_result.correction_white = unitDataPath[0].correction_white;
			//nodeCombi_result.combinaison //combinaison is wrong, but we don't need it
			extra.pathPosition = unitDataPath;//save the position where the find operation has failed
			extra.nbRemainingElement = nbOfElement;
		}
		return bResult;
	}
	
	bool NodeFactory::fillNodeInit(int nbOfElement, node_path_unit unitDataPath[], U8 combinaisons[])
	{
		bool bResult = true;
		cerr << "fillNodeInit(nbOfElement=" << nbOfElement << endl;
		//first we must find which part of the beginning of the path is is the file.
		U8 buffer[STATIC_NODE_COMBI_SIZE];
		extra_info_struct extra;
		if(!findNode(UNKNOWN_NODE_ID, nbOfElement, unitDataPath, *reinterpret_cast<node_combi*>(buffer),extra))
		{//node need to be filled, because is is not present in the db
			cerr << "extra.nbTotalElement=" << extra.nbTotalElement << "\textra.nbRemainingElement=" << extra.nbRemainingElement << endl;
			bResult = fillNode(reinterpret_cast<node_combi*>(buffer)->parentnode_id, extra.nbRemainingElement, static_cast<node_path_unit*>(extra.pathPosition),
			combinaisons + (extra.nbTotalElement-extra.nbRemainingElement), *reinterpret_cast<node_combi*>(buffer));
		}
		return bResult;
	}

	bool NodeFactory::fillNode(U32 parentnode_id, int nbOfElement, node_path_unit unitDataPath[], U8 combinaisons[], node_combi &nodeCombi_result)
	{
		cerr << "fillNode(parentNode=" << parentnode_id << " ,nbOfElement=" << nbOfElement << endl;
		bool bResult = false;
		if(nbOfElement>0)
		{
			file_stream.seekp(0,ios::end);TEST() 
			S32 nbBytes = file_stream.tellp();TEST() 
			if(nbBytes == -1)
				nbBytes=0;
			cerr << "nbBytes=" << nbBytes << endl;
			U32 nbElements = nbBytes / sizeOf_node_combi;
			cerr << "sizeOf_node_combi=" << sizeOf_node_combi << endl;
			nodeCombi_result.node_id = nbElements + 1;
			nodeCombi_result.parentnode_id = parentnode_id;
			nodeCombi_result.correction_black = unitDataPath[0].correction_black;
			nodeCombi_result.correction_white = unitDataPath[0].correction_white;
			cerr << "node_id=" << nodeCombi_result.node_id << "\tparentnode_id=" << nodeCombi_result.parentnode_id << "\tcorrection_black=" << nodeCombi_result.correction_black << "\tcorrection_white=" << nodeCombi_result.correction_white <<endl;
			cerr << "address unitDataPath black\t" << reinterpret_cast<int>(&unitDataPath[0].correction_black) << "\t-->\t" << static_cast<int>(unitDataPath[0].correction_black) << endl;
			cerr << "address unitDataPath white\t" << reinterpret_cast<int>(&unitDataPath[0].correction_white) << "\t-->\t" << static_cast<int>(unitDataPath[0].correction_white) << endl;
			memcpy(nodeCombi_result.combinaison, combinaisons,sizeOf_node_combi);TEST()
			for(U32 i=0;i<nbPosition;i++) {cerr << "[" << hex << static_cast<int>(nodeCombi_result.combinaison[i]) << "]";}
			cerr << endl;
			file_stream.write(reinterpret_cast<char*>(&nodeCombi_result),sizeOf_node_combi); file_stream.flush();TEST() 
		
			bResult=true;
		}
		if(nbOfElement>1)
		{
			bResult = fillNode(nodeCombi_result.node_id, nbOfElement - 1, unitDataPath + 1, combinaisons + nbPosition, nodeCombi_result);
		}
		return bResult;
	}
	
}