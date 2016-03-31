#include <fstream>
#include "typedefinition.h"

#include "node_struct.h"

#ifndef  __NODE_FACTORY_H__
#define  __NODE_FACTORY_H__

#define ROOT_NODE_ID 1
#define UNKNOWN_NODE_ID 0 //can be used also by the root node that doesn't have a parent!
#define STATIC_NODE_COMBI_SIZE 100
namespace dbserver {
    using namespace std;

    class NodeFactory {
    public:
        //constructor: open a file (read / write), if it exists, otherwise create it!
        NodeFactory(U32 nbColor, U32 nbPosition) noexcept;
        NodeFactory(const char path[]) noexcept;

        //command READ path : PiCj/BkNl/BkNl/BkNl...
        //here the PiCi is already consumed (Position, Color) by the constructor
        bool findNodeInit(const char path[], string &nodeCombi_result) noexcept;
        bool findNodeInit(const char path[], node_combi &nodeCombi_result) noexcept;
        bool findNodeInit(int nbOfElement, node_path_unit unitDataPath[], node_combi &nodeCombi_result) noexcept;

        //command STORE
        //hypothesis: when a path is searched, if it is not found in the file, we need to add it in the file
        bool fillNodeInit(const char path[]) noexcept;
        bool fillNodeInit(int nbOfElement, node_path_unit unitDataPath[], U8 combinaisons[]) noexcept;

        U32 getSizeOf_node_combi() const noexcept {
            return sizeOf_node_combi;
        }

    private:
        void initNodeFactory(U32 _nbColor, U32 _nbPosition) noexcept;

        bool findNode(U32 parentnode_id, int nbOfElement, node_path_unit unitDataPath[], node_combi &nodeCombi_result, extra_info_struct &extra) noexcept;
        bool fillNode(U32 parentnode_id, int nbOfElement, node_path_unit unitDataPath[], U8 combinaisons[], node_combi &nodeCombi_result) noexcept;

        static bool combinaisonToString(U32 maxPosition, node_combi &nodeCombi_result, string &sCombi) noexcept;
        static bool readCombinaison(stringstream &sPath, U32 maxPosition, U32 &position, U8* &pCombinaisons) noexcept;
        static bool readCorrectionColor(stringstream &sPath, char id, U32 &correction) noexcept;

    private:
        U32 nbColor;
        U32 nbPosition;
        S32 sizeOf_node_combi; //signed because, it is more readable because it avoids to cast signed/ unsigned with using gcount
        fstream file_stream;
    };
}
#endif//__NODE_FACTORY_H__