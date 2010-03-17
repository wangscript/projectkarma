
#include "input.h"
#include "simulation.h"

#include "Ogre.h"
#include "OgreWindowEventUtilities.h"

#if defined(WIN32)
#include "windows.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#else
int main (int argc, char *argv[]) {
#endif

	Ogre::Root *ogre;
	Ogre::RenderWindow *window;
	Ogre::SceneManager *sceneMgr;
	Ogre::Camera *camera;

	// fire up an Ogre rendering window. Clearing the first two (of three) params will let us 
	// specify plugins and resources in code instead of via text file
	ogre = new Ogre::Root("", "");


	// This is a VERY minimal rendersystem loading example; we are hardcoding the OpenGL 
	// renderer, instead of loading GL and D3D9. We will add renderer selection support in a 
	// future article.

	// I separate the debug and release versions of my plugins using the same "_d" suffix that
	// the Ogre main libraries use; you may need to remove the "_d" in your code, depending on the
	// naming convention you use 
	// EIHORT NOTE: All Ogre DLLs use this suffix convention now -- #ifdef on the basis of the _DEBUG 
	// define
#if defined(_DEBUG)
	ogre->loadPlugin("RenderSystem_GL_d");
#else
	ogre->loadPlugin("RenderSystem_GL");
#endif

	Ogre::RenderSystemList *renderSystems = NULL;
	Ogre::RenderSystemList::iterator r_it;

	// we do this step just to get an iterator that we can use with setRenderSystem. In a future article
	// we actually will iterate the list to display which renderers are available. 
	renderSystems = ogre->getAvailableRenderers();
	r_it = renderSystems->begin();
	ogre->setRenderSystem(*r_it);
	ogre->initialise(false);

	// load common plugins
#if defined(_DEBUG)
	ogre->loadPlugin("Plugin_CgProgramManager_d");		
	ogre->loadPlugin("Plugin_OctreeSceneManager_d");
#else
	ogre->loadPlugin("Plugin_CgProgramManager");		
	ogre->loadPlugin("Plugin_OctreeSceneManager");
#endif
	// load the basic resource location(s)
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		"media", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		"media/gui.zip", "Zip", "GUI");
#if defined(WIN32)
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		"c:\\windows\\fonts", "FileSystem", "GUI");
#endif

	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("GUI");

	// setup main window; hardcode some defaults for the sake of presentation
	Ogre::NameValuePairList opts;
	opts["resolution"] = "1024x768";
	opts["fullscreen"] = "false";
	opts["vsync"] = "false";

	// create a rendering window with the title "CDK"
	window = ogre->createRenderWindow("CDK", 1024, 768, false, &opts);

	// since this is basically a CEGUI app, we can use the ST_GENERIC scene manager for now; in a later article 
	// we'll see how to change this
	sceneMgr = ogre->createSceneManager(Ogre::ST_GENERIC);
	camera = sceneMgr->createCamera("camera");
	camera->setNearClipDistance(5);
	Ogre::Viewport* vp = window->addViewport(camera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// most examples get the viewport size to calculate this; for now, we'll just 
	// set it to 4:3 the easy way
	camera->setAspectRatio((Ogre::Real)1.333333);

	// this next bit is for the sake of the input handler
	unsigned long hWnd;
	window->getCustomAttribute("WINDOW", &hWnd);

	// set up the input handlers
	Simulation *sim = new Simulation();
	InputHandler *handler = new InputHandler(sim, hWnd);
	sim->requestStateChange(SIMULATION);

	while (sim->getCurrentState() != SHUTDOWN) {

		handler->capture();

		// run the message pump (Eihort)
		Ogre::WindowEventUtilities::messagePump();

		ogre->renderOneFrame();
	}

	// clean up after ourselves
	delete handler;
	delete sim;
	delete ogre;

	return 0;
}
