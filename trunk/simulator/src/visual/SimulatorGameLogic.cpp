/*
 * SimulatorGameLogic.cpp
 *
 *  Created on: 23-10-2011
 *      Author: wojtas
 */

#include "SimulatorGameLogic.h"
#include <Application.h>
#include <OGRE/OgreRoot.h>

#include "DotSceneHandler.h"
#include "MainMenu.h"

#include "LogManager.h"
#include "OgreWidget.h"
#include "StyleSettingsWidget.h"

#include <OgreEntity.h>
#include <OgreRenderWindow.h>
#include <OgreResourceGroupManager.h>
#include <OgreRoot.h>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QSettings>

#include "Avatar.h"
#include "model/ModelHelpers.h"

SimulatorGameLogic::~SimulatorGameLogic() {
	// TODO Auto-generated destructor stub
}
SimulatorGameLogic::SimulatorGameLogic(void) :
	GameLogic() {
	mSceneManager = NULL;
}

void SimulatorGameLogic::initialise(void) {
	//qApp->setStyleSheet(qApp->settings()->value("UI/StyleFile").toString());

	mDemoLog = mApplication->createLog("Demo");

	//mApplication->showLogManager();

	mDemoLog->logMessage("A demonstration debug message", LL_DEBUG);
	mDemoLog->logMessage("A demonstration info message", LL_INFO);
	mDemoLog->logMessage("A demonstration warning message", LL_WARNING);
	mDemoLog->logMessage("A demonstration error message", LL_ERROR);

	//Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	// Create the generic scene manager
	mSceneManager = Ogre::Root::getSingleton().createSceneManager(
			Ogre::ST_GENERIC, "GenericSceneManager");

	//Set up scene
	loadScene("media/scenes/test.scene");

	//mApplication->ogreRenderWindow()->addViewport(mCamera)->setBackgroundColour(Ogre::ColourValue::Black);

	mSceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));

	//Create the MainMenu
	mMainMenu = new MainMenu(qApp, qApp->mainWidget());

	mTime = new QTime;
	mTime->start();

	mIsFirstFrame = true;

	mCameraSpeed = 10.0;

	for (Ogre::SceneManager::MovableObjectIterator moi =
			mSceneManager->getMovableObjectIterator("Entity"); moi.hasMoreElements(); moi.moveNext()) {
		Ogre::Entity *entity = static_cast<Ogre::Entity*> (moi.peekNextValue());

		Ogre::AnimationStateSet* animationStateSet =
				entity->getAllAnimationStates();
		if (animationStateSet && animationStateSet->hasAnimationState("Walk")) {
			Ogre::AnimationState* walkAnimationState =
					animationStateSet->getAnimationState("Walk");
			walkAnimationState->setLoop(true);
			walkAnimationState->setEnabled(true);
		}
	}

	// mApplication->showFPSCounter();

	mStyleSettingsWidget = new StyleSettingsWidget;
	mApplication->addSettingsWidget("Style", mStyleSettingsWidget);
}

void SimulatorGameLogic::update(void) {

	mLastFrameTime = mCurrentTime;
	mCurrentTime = mTime->elapsed();

	float timeElapsedInSeconds = (mCurrentTime - mLastFrameTime) / 1000.0f;

	/*	for (Ogre::SceneManager::MovableObjectIterator moi = mSceneManager->getMovableObjectIterator("Entity"); moi.hasMoreElements(); moi.moveNext())
	 {
	 Ogre::Entity *entity = static_cast<Ogre::Entity*>(moi.peekNextValue());

	 Ogre::AnimationStateSet* animationStateSet = entity->getAllAnimationStates();
	 if(animationStateSet && animationStateSet->hasAnimationState("Walk"))
	 {
	 Ogre::AnimationState* walkAnimationState = animationStateSet->getAnimationState("Walk");
	 walkAnimationState->addTime(timeElapsedInSeconds);
	 }
	 }*/

	float distance = mCameraSpeed * timeElapsedInSeconds;

	if (mKeyStates[Qt::Key_W] == KS_PRESSED) {
		mCamera->setPosition(mCamera->getPosition() + mCamera->getDirection()
				* distance);
	}
	if (mKeyStates[Qt::Key_S] == KS_PRESSED) {
		mCamera->setPosition(mCamera->getPosition() - mCamera->getDirection()
				* distance);
	}
	if (mKeyStates[Qt::Key_A] == KS_PRESSED) {
		mCamera->setPosition(mCamera->getPosition() - mCamera->getRight()
				* distance);
	}
	if (mKeyStates[Qt::Key_D] == KS_PRESSED) {
		mCamera->setPosition(mCamera->getPosition() + mCamera->getRight()
				* distance);
	}

	if (!mIsFirstFrame) {
		QPoint mouseDelta = mCurrentMousePos - mLastFrameMousePos;
		mCamera->yaw(Ogre::Radian(-mouseDelta.x() * timeElapsedInSeconds));
		mCamera->pitch(Ogre::Radian(-mouseDelta.y() * timeElapsedInSeconds));

		int wheelDelta = mCurrentWheelPos - mLastFrameWheelPos;
		Ogre::Radian fov = mCamera->getFOVy();
		fov += Ogre::Radian(-wheelDelta * 0.001);
		fov = (std::min)(fov, Ogre::Radian(2.0f));
		fov = (std::max)(fov, Ogre::Radian(0.5f));
		mCamera->setFOVy(fov);
	}
	mLastFrameMousePos = mCurrentMousePos;
	mLastFrameWheelPos = mCurrentWheelPos;

	mIsFirstFrame = false;
}

void SimulatorGameLogic::shutdown(void) {
	mSceneManager->clearScene();
	Ogre::Root::getSingleton().destroySceneManager(mSceneManager);
}

void SimulatorGameLogic::onKeyPress(QKeyEvent* event) {
	mKeyStates[event->key()] = KS_PRESSED;

	if (event->key() == Qt::Key_Escape) {
		//qApp->centerWidget(mMainMenu, qApp->mMainWindow);
		mMainMenu->exec();
	}
}

void SimulatorGameLogic::onKeyRelease(QKeyEvent* event) {
	mKeyStates[event->key()] = KS_RELEASED;
}

void SimulatorGameLogic::onMousePress(QMouseEvent* event) {
	mCurrentMousePos = event->pos();
	mLastFrameMousePos = mCurrentMousePos;
}

void SimulatorGameLogic::onMouseMove(QMouseEvent* event) {
	mCurrentMousePos = event->pos();
}

void SimulatorGameLogic::onWheel(QWheelEvent* event) {
	mCurrentWheelPos += event->delta();
}

QtOgre::Log* SimulatorGameLogic::demoLog(void) {
	return mDemoLog;
}

void SimulatorGameLogic::loadScene(QString filename) {
	//The QtOgre DotScene loading code will clear the existing scene except for cameras, as these
	//could be used by existing viewports. Therefore we clear and viewports and cameras before
	//calling the loading code.
	mApplication->ogreRenderWindow()->removeAllViewports();
	mSceneManager->destroyAllCameras();

	//Now load the scene.
	DotSceneHandler handler(mSceneManager);
	QXmlSimpleReader reader;
	reader.setContentHandler(&handler);
	reader.setErrorHandler(&handler);

	QFile file(filename);
	file.open(QFile::ReadOnly | QFile::Text);
	QXmlInputSource xmlInputSource(&file);
	reader.parse(xmlInputSource);

	//Now create a viewport, using the first camera in the scene.
	mCamera = mSceneManager->getCameraIterator().peekNextValue();

	//mCamera->setPosition(0, 0, 20);
	//mCamera->lookAt(0, 0, 0);
	mCamera->setNearClipDistance(1.0);
	mCamera->setFarClipDistance(1000.0);
	//mCamera->setFOVy(Ogre::Radian(1.0f));

	Ogre::Viewport* viewport = mApplication->ogreRenderWindow()->addViewport(
			mCamera);
	viewport->setBackgroundColour(Ogre::ColourValue::Black);
}

Avatar * SimulatorGameLogic::createCreatureAvatar(QString creatureId) {
	if (mSceneManager) {
		Ogre::SceneNode *avatarNode =
				mSceneManager->getRootSceneNode()->createChildSceneNode(
						creatureId.toStdString().c_str(), Ogre::Vector3::ZERO,
						Ogre::Quaternion::IDENTITY);
		avatarNode->setVisible(false);
		Ogre::Entity* avatarEntity = mSceneManager->createEntity((creatureId
				+ "Entity").toStdString(), "robot.mesh");
		avatarNode->attachObject(avatarEntity);
	}
	Avatar *avatar = new Avatar();
	return avatar;
}

void SimulatorGameLogic::assureAvatarOnField(Avatar *avatar, unsigned x,
		unsigned y) {
	// Sprawdź czy aktualnie avatar znajduje się na polu x,y. Jeżeli nie to przenieś go tam i pokaż

}

Avatar * SimulatorGameLogic::findCreatureAvatar(QString creatureId) {

}

void SimulatorGameLogic::animateAvatarMovementToField(Avatar *avatar,
		unsigned x, unsigned y) {

	if (mSceneManager) {
		mSceneManager->getSceneNode("CubeNode2")->setScale(
				randBetweenAndStepped(0.1, 1.0, 0.1), randBetweenAndStepped(
						0.1, 1.0, 0.1), randBetweenAndStepped(0.1, 1.0, 0.1));
	}
}

void SimulatorGameLogic::animateAvatarEating(Avatar *avatar, unsigned articleId) {

}

void SimulatorGameLogic::animateAvatarBorn(Avatar *avatar) {

}
