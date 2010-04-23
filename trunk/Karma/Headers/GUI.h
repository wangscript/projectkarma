#ifndef GUI_H
#define GUI_H

#include <vector>
#include <Ogre.h>
#include <OISMouse.h>
#include "AppState.h"

// MOE = MouseOverElement

template <class T> class GUI
{
public:
	struct mouseOverElement
	{		
	Ogre::OverlayElement* overlay, *overlayMOE, *overlayLocked;
	Ogre::Real left,right,top,bottom;
	void (GUI::*onGUIPressFunction)();		
	void (T::*onStatePressFunction)();
	bool locked;
	std::vector<int>* lockRelationships, *unlockRelationships;
	};

	GUI(T* state, Ogre::String& s);
	~GUI();
	void	addLockedRelation(int currentMOE, std::vector<int> relationshipMOEs);
	void	addUnlockedRelation(int currentMOE, std::vector<int> relationshipMOEs);
	int		addMouseOver(Ogre::String &s,void (T::*onStatePressFunction)(), bool lockedAvailable = false, bool locked = false);
	bool	checkMouseOver(const int x, const int y);
	bool	mousePressed();
	void	resetMouseOver(){mtpCurrentMouseOver = NULL;};

protected:

	T*									mtpClass;
	int									mtWindowWidth;
	int									mtWindowHeight;
	mouseOverElement*					mtpCurrentMouseOver;
	bool								mtMouseOver;
	std::vector<mouseOverElement>		mtMouseOverElements;	

	bool	isMouseOver(mouseOverElement &,const int, const int);
	void	lockElements(mouseOverElement* mOE);
	void	showMouseOver(mouseOverElement* mOE, bool showMouseOver = true);
	void	unlockElements(mouseOverElement* mOE);
};

#endif

template <class T> GUI<T>::GUI(T* state, Ogre::String& s)
{
	//Associative pointer to the class that initiated the GUI klass.
	mtpClass = state;
	
	//Start values for member variables.
	mtpCurrentMouseOver=NULL;
	mtMouseOver= 0;

	//These are being used when initiating new MOEs. 
	mtWindowWidth = GameFramework::getSingletonPtr()->mpRenderWnd->getWidth();
	mtWindowHeight = GameFramework::getSingletonPtr()->mpRenderWnd->getHeight();

	//Load the overlay that hold our MOEs
	Ogre::Overlay* guiOverlay = Ogre::OverlayManager::getSingleton().getByName(s);
	guiOverlay->show();

	//Loops through all elements in the Overlay and hides them (default = show);
	Overlay::Overlay2DElementsIterator Iter = guiOverlay->get2DElementsIterator();
	while (Iter.hasMoreElements())
	{
		Ogre::OverlayContainer* pContainer = Iter.getNext();

		OverlayContainer::ChildIterator IterCont = pContainer->getChildIterator();
		while (IterCont.hasMoreElements())
		{
			Ogre::OverlayElement* pElement = IterCont.getNext();
			pElement->hide();
		}
	}
}
/*---------------------------------------------------------------------------------*/
template <class T> GUI<T>::~GUI()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Deleting GUI stuff...");
	//Deallocate earlier allocated vectors.
	for (unsigned int i= 0; i < mtMouseOverElements.size();i++)
	{
		delete mtMouseOverElements[i].lockRelationships;
		delete mtMouseOverElements[i].unlockRelationships;
	}
	Ogre::OverlayManager::getSingleton().getByName("GuiKarma/Menu")->hide();
}
/*---------------------------------------------------------------------------------*/
template <class T> void GUI<T>::addLockedRelation(int currentMOE, std::vector<int> relationshipMOEs)
{
	//Adds a vector that has information about what elements that's going to be locked when currentMOE is pressed.
	mtMouseOverElements[currentMOE].lockRelationships = new std::vector<int>(relationshipMOEs);
}
/*---------------------------------------------------------------------------------*/
template <class T> void GUI<T>::addUnlockedRelation(int currentMOE, std::vector<int> relationshipMOEs)
{
	//Adds a vector that has information about what elements that's going to be unlocked when currentMOE is pressed.
	mtMouseOverElements[currentMOE].unlockRelationships = new std::vector<int>(relationshipMOEs);
}
/*---------------------------------------------------------------------------------*/
template <class T> int GUI<T>::addMouseOver(Ogre::String &s,void (T::*onStatePressFunction)(),bool lockedAvailable, bool locked)
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
	mOE.top = mOE.overlay->_getDerivedTop() * mtWindowHeight;
	mOE.bottom = (mOE.overlay->_getDerivedTop() +  mOE.overlay->_getHeight()) * mtWindowHeight;
	mOE.left = mOE.overlay->_getDerivedLeft() * mtWindowWidth;
	mOE.right = (mOE.overlay->_getDerivedLeft()  +  mOE.overlay->_getWidth() ) * mtWindowWidth;

	//Pointer to the function that will be executed when element is pressed;
	mOE.onStatePressFunction = onStatePressFunction;
	
	//Add the mouseOverElement to the vector. Returns the index of the added vector element
	mtMouseOverElements.push_back(mOE);
	return mtMouseOverElements.size() - 1;
}
/*---------------------------------------------------------------------------------*/
template <class T> bool GUI<T>::checkMouseOver(const int x, const int y)
{
	/*mtpCurrentMouseOver is a pointer to the current mouseOverElement.
	So if the cursor is over an element, we will start next frame by checking if it still is.
	If that's the case, return, no need to continue looking.
	If not, unload the pointer.
	*/
	
	//If mtpCurrentMouseOver== NULL then nothing was mouseOvered previous frame
	if (mtpCurrentMouseOver)
	{
		//If element is not locked and isMouseOver return false, then change back to normal overlay
		if(  !mtpCurrentMouseOver->locked && !isMouseOver(*mtpCurrentMouseOver,x,y))
		{
			mtMouseOver = false;
			showMouseOver(mtpCurrentMouseOver,false);
			mtpCurrentMouseOver = NULL;
		}
		//Cursor is still at the same MOE. Do nothing.
		else
			return true;
	}

	//Loops through all elements to check possible mouseovers.
	for (unsigned int i = 0; i < mtMouseOverElements.size(); i++)
	{
		if ( !mtMouseOverElements[i].locked && isMouseOver(mtMouseOverElements[i],x,y))
		{
			mtpCurrentMouseOver = &mtMouseOverElements[i];
			//Change to mouseOver element
			showMouseOver(mtpCurrentMouseOver);
			mtMouseOver = true;
			break;
		}
	}
	//If any object is mouseOvered, this will return true.
	return mtMouseOver;
}
/*---------------------------------------------------------------------------------*/
template <class T> bool GUI<T>::mousePressed()
{
	//By default, a press will not lock the button
	bool locked = false;
	//This will be false if no element is mouseOvered and we will return the locked ==false
	if (mtpCurrentMouseOver)
	{
		//If we have added lock relationships
		if(mtpCurrentMouseOver->lockRelationships != NULL)
		{
			lockElements(mtpCurrentMouseOver);
			locked = true;
		}
		//If we have added unlock relationships
		if(mtpCurrentMouseOver->unlockRelationships != NULL)
		{
			unlockElements(mtpCurrentMouseOver);
		}
		
		//Fire the function has been linked with the MOE.
		void (T::*onStatePressFunction)() = mtpCurrentMouseOver->onStatePressFunction;
		(mtpClass->*onStatePressFunction)();
	}
	//If locked == true, then we have to manually reset the mtpCurrentMouseOver in the AppState
	//This is needed since we might quit the whole application as a T::function, and then mtpCurrentMouseOver won't exist.
	return locked;
}
/*---------------------------------------------------------------------------------*/
template <class T> bool GUI<T>::isMouseOver(mouseOverElement &mOE,const int x, const int y)
{
	//If the cursor in inside the MOE border, return true
	if (x > mOE.left && x < mOE.right && y > mOE.top && y < mOE.bottom)
	{
		return true;
	}
	return false;
}
/*---------------------------------------------------------------------------------*/
template <class T> void GUI<T>::lockElements(mouseOverElement* mOE)
{
	//Lock the elements that is binded with the current MOE
	for(unsigned int i = 0; i < mOE->lockRelationships->size(); i++)
	{
		mtMouseOverElements[mOE->lockRelationships->at(i)].locked = true;
		mtMouseOverElements[mOE->lockRelationships->at(i)].overlay->hide();
		mtMouseOverElements[mOE->lockRelationships->at(i)].overlayMOE->hide();
		mtMouseOverElements[mOE->lockRelationships->at(i)].overlayLocked->show();
	}
}
/*---------------------------------------------------------------------------------*/
template <class T> void GUI<T>::showMouseOver(mouseOverElement* mOE, bool showMouseOver)
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
template <class T> void GUI<T>::unlockElements(mouseOverElement* mOE)
{
	//Unlock the elements that is binded with the current MOE
	for(unsigned int i = 0; i < mOE->unlockRelationships->size(); i++)
	{
		mtMouseOverElements[mOE->unlockRelationships->at(i)].locked = false;
		mtMouseOverElements[mOE->unlockRelationships->at(i)].overlay->show();
		mtMouseOverElements[mOE->unlockRelationships->at(i)].overlayMOE->hide();
		mtMouseOverElements[mOE->unlockRelationships->at(i)].overlayLocked->hide();
	}
}