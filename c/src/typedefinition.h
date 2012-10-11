/***************************************************************************
 *   Copyright (C) 2005 by christian Foucher   *
 *   christian@localhost   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/*
* $Log$
*/

#ifndef LABYTYPEDEFINITION_H
#define LABYTYPEDEFINITION_H

namespace mastermind
{

typedef bool		U1;
typedef unsigned char	U8;
typedef unsigned short	U16;
typedef unsigned long	U32;

typedef char	S8;
typedef short	S16;
typedef long	S32;

#define mMin(x,y) (((x)<(y))?(x):(y))
#define mMax(x,y) (((x)>(y))?(x):(y))
#define mAbsDiff(x,y) (((x)>(y))?((x)-(y)):((y)-(x)))

#define mLibAssert(boolVal) while(!boolVal);

//#define __INLINE__ __attribute__((always_inline)) inline
#define __INLINE__

typedef struct{
U32 blackPigs;
U32 whitePigs;
} tScore;

}

#endif
