#include <iostream>
#include <cstring>
#include <string>
#include "nodeFactory.h"

using namespace dbserver;
using namespace std;

//#define NB_POSITION 4
#define SET_COMMAND "set"
#define GET_COMMAND "get"

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

//TODO
// 3) Add interactive command line for testing dynamically! (allow the modification of the number of color and position)

//int main(int argv, char *args[])

bool treatCommand(string inputString, string &result) noexcept {
    // set p4c5  [0,1,2,3]b#w#  [1,1,1,1]b2w0  [2,2,2,2]b3w1  [4,3,2,1]b2w2
    // set p4c5  [0,1,2,3]b#w#  [0,0,0,0]b2w3
    // get p4c5 b#w#
    // get p4c5 b#w# b2w0
    // get p4c5 b#w# b2w3
    // get p4c5 b#w# b2w0
    // get p4c5 b#w# b2w3 b3w1 <---Can't be found
    // get p4c5 b#w# b2w0 b3w1
    // get p4c5 b#w# b2w3 b3w2 <---Can't be found
    if (inputString.find(SET_COMMAND) != string::npos) {
        cerr << inputString << endl;
        string subInputString = inputString.substr(sizeof (SET_COMMAND));
        cerr << subInputString << endl;
        NodeFactory nodeLocal(subInputString.c_str());
        return nodeLocal.fillNodeInit(subInputString.c_str());
    } else if (inputString.find(GET_COMMAND) != string::npos) {
        cerr << inputString << endl;
        string subInputString = inputString.substr(sizeof (GET_COMMAND));
        cerr << subInputString << endl;
        NodeFactory nodeLocal(subInputString.c_str());
        return nodeLocal.findNodeInit(subInputString.c_str(), result);
    } else {
        cout << "unknown command" << endl;
        return false;
    }
}