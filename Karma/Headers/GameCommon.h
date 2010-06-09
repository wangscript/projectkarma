/*---------------------------------------------------------------------------------*/
/* File: GameCommon.h											   				   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	All common #includes that almost every class needs.				   */
/*---------------------------------------------------------------------------------*/

#ifndef GAMECOMMON_H
#define GAMECOMMON_H

#include <Ogre.h>
#include <OISMouse.h>
#include <NxOgreOGRE3D.h>
#include "AppState.h"
#include <NxActor.h>
#include <NxScene.h>
#include <NxUserContactReport.h>
#include <NxOgreCommon.h>
#include <NxOgreRigidBodyDynamicFunctions.h>
#include "SoundManager.h"
#include "Effects.h"
#include "Debug.h"
#include "GameEnums.h"

/* GridData is a class used for chunks.*/
class GridData
{
public:
	GridData(){};
	GridData(int x1,int y2) {x = x1;y=y2;};

	int x,y;
};

#endif