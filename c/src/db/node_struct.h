#include "typedefinition.h"

#ifndef  __NODE_STRUCT_H__
#define  __NODE_STRUCT_H__

typedef struct{
U32 nbColor;
U32 nbPosition;
} node_header;

typedef struct{
U32 node_id;
U32 parentnode_id;
U32 correction_black;
U32 correction_white;
U8 combinaison[];
} node_combi;

#define UNUSED_CORRECTION (~0)

typedef struct{
U32 correction_black;
U32 correction_white;
} node_path_unit;

typedef struct{
void * pathPosition;
U32 nbRemainingElement;
U32 nbTotalElement;
} extra_info_struct;

#define mGetSizeOf_node_combi( nbPosition) (sizeof(node_combi) + nbPosition)

#endif//__NODE_STRUCT_H__