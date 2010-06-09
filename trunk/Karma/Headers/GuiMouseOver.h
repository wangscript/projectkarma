/*---------------------------------------------------------------------------------*/
/* File: GuiMouseOver.h															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	GuiMouseOver is a class that takes cares of GUI elements that	   */ 
/* has mouseOver effects and functions. For clarification in this class,		   */
/* MOE = MouseOverElement														   */
/*---------------------------------------------------------------------------------*/

#ifndef GUIMOUSEOVER_H
#define GUIMOUSEOVER_H

#include <vector>
#include <Ogre.h>
#include <OISMouse.h>
#include "AppState.h"

template <class T> class GuiMouseOver
{
public:
	/* Information about every MouseOverElement(MOE).
	
	3 different overlay elements
	Normal = Normal state.
	MouseOver = MouseOver state.
	Locked = Locked/Hidden/Not available.
	onGuiMouseOverPressFunction = Pointer to a function inside this class.
	onStatePressFunction = Function outside the class bound with the element. Will be fired when the element is pressed
	lockReleationships = vector of IDs of elements that should be locked when the element is pressed.
	unlockRelationships = vector of IDs of elements taht should be unlocked when the element is pressed.
	*/
	struct mouseOverElement
	{		
	Ogre::OverlayElement	*overlay,*overlayMOE,*overlayLocked;
	Ogre::Real				 left,right,top,bottom;
	void			  (GuiMouseOver::*onGuiMouseOverPressFunction)();		
	void				(T::*onStatePressFunction)();
	bool					 locked;
	std::vector<int>        *lockRelationships, *unlockRelationships;
	};

	/* state = use this-pointer from the state you're calling GuiMouseOver from.
	s = name of the GUI overlay.*/
	GuiMouseOver(T* state, Ogre::String& s);
	~GuiMouseOver();

	/* Adds a vector containing the lockRelationship for a MOE. See struct mouseOverElement */
	void	addLockedRelation(int currentMOE, std::vector<int> relationshipMOEs);

	/* Adds a Mouse Over element. 
	s = Name of the normal GUI component.
	onStatePressFunction = Function that will be fired when the element is pressed.
	lockedAvailable = Can the MOE be locked/hidden?
	locked = Is it by default locked?*/
	int		addMouseOver(Ogre::String &s,void (T::*onStatePressFunction)(), bool lockedAvailable = false, bool locked = false);	

	/* Adds a vector containing the unlockRelationship for a MOE. See struct mouseOverElement */
	void	addUnlockedRelation(int currentMOE, std::vector<int> relationshipMOEs);

	/* Checks if the mouse has intersected with any MOE. x and y = screen coordinates. */
	bool	checkMouseOver(const int x, const int y);

	/* Fires the functions that is bound with the active MOE. Also changes locked/unlocked relationships. 
	Returns false if the element doesn't lock any MOEs. If the functions returns true, then we have to
	manually reset the mvpCurrentMouseOver in the AppState.	This is needed since we might quit the whole 
	application as a T::function, and then mvpCurrentMouseOver won't exist.*/
	bool	mousePressed();

	/* Resets the current MOE. */
	void	resetMouseOver(){mvpCurrentMouseOver = NULL;};

private:
	T*									mvpClass;
	int									mvWindowWidth,mvWindowHeight;
	mouseOverElement*					mvpCurrentMouseOver;
	bool								mvMouseOver;
	std::vector<mouseOverElement>		mvMouseOverElements;	

	/* Returns true if the MOE is mouseOvered at the coordinates x and y */
	bool	isMouseOver(mouseOverElement &,const int x, const int y);

	/* Lock the elements in the lockedRelationship vector of the MOE. */
	void	lockElements(mouseOverElement* mOE);

	/* Either shows or hides the MOE. Default = Show. */
	void	showMouseOver(mouseOverElement* mOE, bool showMouseOver = true);

	/* Unlock the elements in the unlockedRelationship vector of the MOE. */
	void	unlockElements(mouseOverElement* mOE);
};

#endif

/*---------------------------------------------------------------------------------*/
/*									PUBLIC										   */
/*---------------------------------------------------------------------------------*/
template <class T> GuiMouseOver<T>::GuiMouseOver(T* state, Ogre::String& s)
{
	//Associative pointer to the class that initiated the GuiMouseOver klass.
	mvpClass = state;
	
	//Start values for member variables.
	mvpCurrentMouseOver=NULL;
	mvMouseOver= 0;

	//These are being used when initiating new MOEs. 
	mvWindowWidth = GameFramework::getSingletonPtr()->mpRenderWnd->getWidth();
	mvWindowHeight = GameFramework::getSingletonPtr()->mpRenderWnd->getHeight();

	//Load the overlay that hold our MOEs
	Ogre::Overlay* GuiMouseOverOverlay = Ogre::OverlayManager::getSingleton().getByName(s);
	GuiMouseOverOverlay->show();

	//Loops through all elements in the Overlay and hides them (default = show);
	Ogre::Overlay::Overlay2DElementsIterator Iter = GuiMouseOverOverlay->get2DElementsIterator();
	while (Iter.hasMoreElements())
	{
		Ogre::OverlayContainer* pContainer = Iter.getNext();

		Ogre::OverlayContainer::ChildIterator IterCont = pContainer->getChildIterator();
		while (IterCont.hasMoreElements())
		{
			Ogre::OverlayElement* pElement = IterCont.getNext();
			pElement->hide();
		}
	}
}
/*---------------------------------------------------------------------------------*/
template <class T> GuiMouseOver<T>::~GuiMouseOver()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Deleting GuiMouseOver stuff...");
	//Deallocate earlier allocated vectors.
	for (unsigned int i= 0; i < mvMouseOverElements.size();i++)
	{
		delete mvMouseOverElements[i].lockRelationships;
		delete mvMouseOverElements[i].unlockRelationships;
	}
}
/*---------------------------------------------------------------------------------*/
template <class T> void GuiMouseOver<T>::addLockedRelation(int currentMOE, std::vector<int> relationshipMOEs)
{
	//Adds a vector that has information about what elements that's going to be locked when currentMOE is pressed.
	mvMouseOverElements[currentMOE].lockRelationships = new std::vector<int>(relationshipMOEs);
}
/*---------------------------------------------------------------------------------*/
template <class T> void GuiMouseOver<T>::addUnlockedRelation(int currentMOE, std::vector<int> relationshipMOEs)
{
	//Adds a vector that has information about what elements that's going to be unlocked when currentMOE is pressed.
	mvMouseOverElements[currentMOE].unlockRelationships = new std::vector<int>(relationshipMOEs);
}
/*---------------------------------------------------------------------------------*/
template <class T> int GuiMouseOver<T>::addMouseOver(Ogre::String &s,void (T::*onStatePressFunction)(),bool lockedAvailable, bool locked)
{
	mouseOverElement mOE;
	mOE.locked = locked;

	//NULL since we might not add any relationships. Easy to check to later on
	mOE.lockRelationships = NULL;
	mOE.unlockRelationships = NULL;

	//Show normal overlay, hide the mouseOver. 
	mOE.overlay =Ogre::OverlayManager::getSingleton().getOverlayElement(s); 
	mOE.overlay->show();
	mOE.overlayMOE =Ogre::OverlayManager::getSingleton().getOverlayElement(s + "MouseOver");
	mOE.overlayMOE->hide();
				
	//Loads the locked overlay element(if available)
	if (lockedAvailable)
	{
		mOE.overlayLocked = Ogre::OverlayManager::getSingleton().getOverlayElement(s + "Locked");
		//If locked = true then start as locked
		if (locked)
		{
			mOE.overlayLocked->show();
			mOE.overlay->hide();
		}
		else
		{
			mOE.overlayLocked->hide();
		}
	}

	//Information about the size. 
	mOE.top = mOE.overlay->_getDerivedTop() * mvWindowHeight;
	mOE.bottom = (mOE.overlay->_getDerivedTop() +  mOE.overlay->_getHeight()) * mvWindowHeight;
	mOE.left = mOE.overlay->_getDerivedLeft()* mvWindowWidth;
	mOE.right = (mOE.overlay->_getDerivedLeft()  +  mOE.overlay->_getWidth() ) * mvWindowWidth;

	//Pointer to the function that will be executed when element is pressed;
	mOE.onStatePressFunction = onStatePressFunction;
	
	//Add the mouseOverElement to the vector. Returns the index of the added vector element
	mvMouseOverElements.push_back(mOE);
	return mvMouseOverElements.size() - 1;
}
/*---------------------------------------------------------------------------------*/
template <class T> bool GuiMouseOver<T>::checkMouseOver(const int x, const int y)
{
	/*mvpCurrentMouseOver is a pointer to the current mouseOverElement.
	So if the cursor is over an element, we will start next frame by checking if it still is.
	If that's the case, return, no need to continue looking.
	If not, unload the pointer.
	*/
	
	//If mvpCurrentMouseOver== NULL then nothing was mouseOvered previous frame
	if (mvpCurrentMouseOver)
	{
		//If element is not locked and isMouseOver return false, then change back to normal overlay
		if(  !mvpCurrentMouseOver->locked && !isMouseOver(*mvpCurrentMouseOver,x,y))
		{
			mvMouseOver = false;
			showMouseOver(mvpCurrentMouseOver,false);
			mvpCurrentMouseOver = NULL;
		}
		//Cursor is still at the same MOE. Do nothing.
		else
			return true;
	}

	//Loops through all elements to check possible mouseovers.
	for (unsigned int i = 0; i < mvMouseOverElements.size(); i++)
	{
		if ( !mvMouseOverElements[i].locked && isMouseOver(mvMouseOverElements[i],x,y))
		{
			mvpCurrentMouseOver = &mvMouseOverElements[i];
			//Change to mouseOver element
			showMouseOver(mvpCurrentMouseOver);
			mvMouseOver = true;
			break;
		}
	}
	//If any object is mouseOvered, this will return true.
	return mvMouseOver;
}
/*---------------------------------------------------------------------------------*/
template <class T> bool GuiMouseOver<T>::mousePressed()
{
	//By default, a press will not lock the button
	bool locked = false;
	//This will be false if no element is mouseOvered and we will return the locked ==false
	if (mvpCurrentMouseOver)
	{
		//If we have added lock relationships
		if(mvpCurrentMouseOver->lockRelationships != NULL)
		{
			lockElements(mvpCurrentMouseOver);
			locked = true;
		}
		//If we have added unlock relationships
		if(mvpCurrentMouseOver->unlockRelationships != NULL)
		{
			unlockElements(mvpCurrentMouseOver);
		}
		
		//Fire the function has been linked with the MOE.
		void (T::*onStatePressFunction)() = mvpCurrentMouseOver->onStatePressFunction;
		(mvpClass->*onStatePressFunction)();
	}
	//If locked == true, then we have to manually reset the mvpCurrentMouseOver in the AppState
	//This is needed since we might quit the whole application as a T::function, and then mvpCurrentMouseOver won't exist.
	return locked;
}
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*									PRIVATE										   */
/*---------------------------------------------------------------------------------*/
template <class T> bool GuiMouseOver<T>::isMouseOver(mouseOverElement &mOE,const int x, const int y)
{
	//If the cursor in inside the MOE border, return true
	if (x > mOE.left && x < mOE.right && y > mOE.top && y < mOE.bottom)
	{
		return true;
	}
	return false;
}
/*---------------------------------------------------------------------------------*/
template <class T> void GuiMouseOver<T>::lockElements(mouseOverElement* mOE)
{
	//Lock the elements that is binded with the current MOE
	for(unsigned int i = 0; i < mOE->lockRelationships->size(); i++)
	{
		mvMouseOverElements[mOE->lockRelationships->at(i)].locked = true;
		mvMouseOverElements[mOE->lockRelationships->at(i)].overlay->hide();
		mvMouseOverElements[mOE->lockRelationships->at(i)].overlayMOE->hide();
		mvMouseOverElements[mOE->lockRelationships->at(i)].overlayLocked->show();
	}
}
/*---------------------------------------------------------------------------------*/
template <class T> void GuiMouseOver<T>::showMouseOver(mouseOverElement* mOE, bool showMouseOver)
{
	//Default - showMouseOver == true
	if (showMouseOver)
	{
		mOE->overlay->hide();
		mOE->overlayMOE->show();
	}
	else
	{
		mOE->overlay->show();
		mOE->overlayMOE->hide();
	}
}
/*---------------------------------------------------------------------------------*/
template <class T> void GuiMouseOver<T>::unlockElements(mouseOverElement* mOE)
{
	//Unlock the elements that is binded with the current MOE
	for(unsigned int i = 0; i < mOE->unlockRelationships->size(); i++)
	{
		mvMouseOverElements[mOE->unlockRelationships->at(i)].locked = false;
		mvMouseOverElements[mOE->unlockRelationships->at(i)].overlay->show();
		mvMouseOverElements[mOE->unlockRelationships->at(i)].overlayMOE->hide();
		mvMouseOverElements[mOE->unlockRelationships->at(i)].overlayLocked->hide();
	}
}