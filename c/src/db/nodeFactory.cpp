#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>

#include "node_struct.h"
#include "nodeFactory.h"

using namespace std;

#define TEST() cerr << dec << __LINE__ << hex << " : " << file_stream.tellp() << "\tfail:" << file_stream.fail() << "\tbad:" << file_stream.bad() << endl;

namespace dbserver
{
	//Never call a constructor from the constructor of the same class! It is not valid in C++ (it compiles, but it doesn't work!)
	NodeFactory::NodeFactory(const char path[])
	{
		unsigned int _nbPosition, _nbColor;
		cerr << path << endl;
		if(sscanf(path,"p%uc%u",&_nbPosition,&_nbColor)==2)
		{
			initNodeFactory(_nbColor,_nbPosition);
		}
		else
		{
			cerr << "Bad args in the NodeFactory" << endl;
			exit(__LINE__);
		}
	}

	NodeFactory::NodeFactory(U32 _nbColor, U32 _nbPosition)
	{
		initNodeFactory(_nbColor,_nbPosition);
	}
	
	void NodeFactory::initNodeFactory(U32 _nbColor, U32 _nbPosition)
	{
		nbColor = _nbColor;
		nbPosition = _nbPosition;
		
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
			{
				cerr << "CAN'T create the file" << endl;
				exit(__LINE__);
			}
			else
				cerr << "file has been created" << endl;
		}
		else
			cerr << "file has been open" << endl;
		sizeOf_node_combi = mGetSizeOf_node_combi(nbPosition);
	}
	
	bool NodeFactory::findNodeInit(const char path[], string &nodeCombi_result)
	{
		U8 buffer[STATIC_NODE_COMBI_SIZE];
		bool bResult = NodeFactory::findNodeInit(path, *reinterpret_cast<node_combi*>(buffer));

		if(bResult)
			bResult = combinaisonToString(nbPosition, *reinterpret_cast<node_combi*>(buffer), nodeCombi_result);
		else
			nodeCombi_result += "NO RESULT";
		
		return bResult;
	}
	
	bool NodeFactory::combinaisonToString(U32 maxPosition, node_combi &nodeCombi_result, string &sCombi)
	{
		stringstream ssCombi;
		ssCombi << "[";
		for(U32 position=0; position<maxPosition; position++)
		{
			cerr << "combinaisonToString" << endl;
			ssCombi << static_cast<U32>(nodeCombi_result.combinaison[position]);
			if(position < maxPosition -1)
				ssCombi << ',';
		}
		ssCombi << "]";
		sCombi = ssCombi.str();
		return true;
	}
	
	bool NodeFactory::findNodeInit(const char path[], node_combi &nodeCombi_result)
	{
		node_path_unit unitDataPath[STATIC_NODE_COMBI_SIZE];
		node_path_unit * pNodePathUnit = unitDataPath;
		U32 nbElements = 0;
		stringstream sPath(path);
		
		size_t position = sPath.str().find_first_of('b');
		cerr << "DISPLAY sPath: " << path << endl;
		if(position!=string::npos)
		{
			sPath.seekg(position,ios::beg);

			while(readCorrectionColor(sPath, 'b', pNodePathUnit->correction_black) && sPath.good())
			{
				if(readCorrectionColor(sPath, 'w', pNodePathUnit->correction_white))
				{
					cerr << "b" << pNodePathUnit->correction_black << "/w" << pNodePathUnit->correction_white << endl;
					nbElements++;
					pNodePathUnit++;
				}
				else
					exit(__LINE__);
			}
		}

		return findNodeInit(nbElements, unitDataPath, nodeCombi_result);
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
					cerr << "nodeCombi_result.parentnode_id=" << nodeCombi_result.parentnode_id << "/" << parentnode_id
						<< "\nnodeCombi_result.correction_black=" << nodeCombi_result.correction_black << "/" << unitDataPath[0].correction_black
						<< "\nnodeCombi_result.correction_white=" << nodeCombi_result.correction_white << "/" << unitDataPath[0].correction_white << endl; 
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
			{
				cerr << "there is still at least one node in the path, we continue searching.(" << nbOfElement << ")"<<endl;
				bResult = findNode(nodeCombi_result.node_id, nbOfElement - 1, unitDataPath+1, nodeCombi_result, extra);
			}
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
		cerr << "findNode method returns bResult=" << bResult << endl;
		return bResult;
	}
	
	bool NodeFactory::fillNodeInit(const char path[])
	{
		node_path_unit unitDataPath[STATIC_NODE_COMBI_SIZE];
		node_path_unit * pNodePathUnit = unitDataPath;
		U8 combinaisons [STATIC_NODE_COMBI_SIZE];
		U8 * pCombi = combinaisons;
		U32 nbElements = 0;
		stringstream sPath(path);
		U32 pos=0;

		cout << "nbPosition=" << nbPosition << endl;
		while(readCombinaison(sPath, nbPosition, pos, pCombi))
		{
			if(readCorrectionColor(sPath, 'b', pNodePathUnit->correction_black)
			&& readCorrectionColor(sPath, 'w', pNodePathUnit->correction_white))
			{
				nbElements++;
				pNodePathUnit++;
				cerr << "nbElements PLUS_PLUS" << endl;
			}
			else
				exit(__LINE__);
		}

		return fillNodeInit(nbElements, unitDataPath, combinaisons);
	}
	
	bool NodeFactory::readCombinaison(stringstream &sPath, U32 maxPosition, U32 &position, U8* &pCombinaisons)
	{
		bool bResult=true;
		if(position == 0)//if there was no previous search
			position = sPath.str().find_first_of('[');
		else
			position = sPath.str().find_first_of('[',position+1);
		if(static_cast<size_t>(position)!=string::npos)
		{
			char separator = '[';
			cerr << "fillNodeInit: found [" << endl;
			sPath.seekg(static_cast<size_t>(position+1),ios::beg);
			for(U32 combiItem = 0; combiItem < maxPosition; combiItem++)
			{
				if(sPath.good())
				{
					if(separator != ']')
					{//retrieve another item of the combinaison
						U32 temp;
						sPath >> temp;
						*pCombinaisons = static_cast<U8>(temp);
						pCombinaisons++;
						
						cerr << "position:" << position << endl;
						cerr << "[" << temp << "]";
						cerr << "°"<< combiItem << "," << maxPosition << "°" << endl;
					}					
					else //separator == ']'
					{//combinaison completed too soon, all items of the combinaison are not retrieved
						bResult=false;
						break;
					}
				}
				else
				{//incomplete combinaison
					bResult=false;
					break;
				}
				
				do
				{
					position = sPath.tellg();
					sPath >> separator;
					cerr << "lecture separator position: " << position << " contenu: " << separator << endl;
				}
				while(sPath.good() && (separator == ' '));
				if((separator>= '0') && (separator <= '9'))
				{//we have aread a digit, we need to go back to the previous position (in order to the chaarcater to be read by the U32 variable)
					sPath.seekg(static_cast<size_t>(position),ios::beg);
				}
			}
		}
		else
			bResult=false;
		cerr << "return bool: " << bResult << endl;
		return bResult;
	}
	
	bool NodeFactory::readCorrectionColor(stringstream &sPath, char id, U32 &correction)
	{
		cerr << "readCorrectionColor" << endl;
		size_t pos=sPath.tellg();
		char c;
		sPath >> c;
		cerr << "position: "<< pos <<"readchar:'" << c << "'" << endl;
		if(c == id)
		{
			cerr << "Read pigs (" << id << ")" << endl;
			pos=sPath.tellg();//save the current stream position in order to restore it in case of a digit
			sPath >> c;
			if((c >= '0') && (c <= '9'))
			{
				sPath.seekg(pos,ios::beg);//restored the stream position in order to retrieve the full number with it s first digit
				cerr << "digit detected" << endl;
				sPath >> correction;
				cerr << "read number: " << correction << endl; 
			}
			else if(c == '#')
			{
				cerr << "Sharp detected" << endl;
				correction = UNUSED_CORRECTION;
				cerr << "read number: " << correction << endl; 
			}
			else
			{
				cerr << "unexpected char" << endl;
				return false;
			}
		}
		return true;
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
			//for(int i =0 ; i<10 ;i++) cerr << static_cast<int>(combinaisons[i]) << ", ";
			//cerr << endl;
			bResult = fillNode(reinterpret_cast<node_combi*>(buffer)->parentnode_id, extra.nbRemainingElement, static_cast<node_path_unit*>(extra.pathPosition),
			combinaisons + (extra.nbTotalElement-extra.nbRemainingElement) * nbPosition, *reinterpret_cast<node_combi*>(buffer));
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