/*
 * SimulatorGameLogic.h
 *
 *  Created on: 23-10-2011
 *      Author: wojtas
 */

#ifndef SIMULATORGAMELOGIC_H_
#define SIMULATORGAMELOGIC_H_

#include <GameLogic.h>
#include <Log.h>
#include <OgrePrerequisites.h>
#include "MainMenu.h"
#include "StyleSettingsWidget.h"

using namespace QtOgre;
using namespace Ogre;
class Avatar;
class SimulatorGameLogic: public GameLogic {
public:
	enum KeyStates {
		KS_RELEASED, KS_PRESSED
	};

	SimulatorGameLogic();
	virtual ~SimulatorGameLogic();

	void initialise(void);
	void update(void);
	void shutdown(void);

	void onKeyPress(QKeyEvent* event);
	void onKeyRelease(QKeyEvent* event);

	void onMouseMove(QMouseEvent* event);
	void onMousePress(QMouseEvent* event);

	void onWheel(QWheelEvent* event);

	QtOgre::Log* demoLog(void);

	void loadScene(QString filename);


	Avatar * createCreatureAvatar(QString creatureId);
	void assureAvatarOnField(Avatar *avatar, unsigned x, unsigned y);
	Avatar * findCreatureAvatar(QString creatureId);
	void animateAvatarMovementToField(Avatar *avatar, unsigned x, unsigned y);
	void animateAvatarEating(Avatar *avatar, unsigned articleId);
	void animateAvatarBorn (Avatar *avatar);
private:
	QHash<int, KeyStates> mKeyStates;
	QPoint mLastFrameMousePos;
	QPoint mCurrentMousePos;

	int mLastFrameWheelPos;
	int mCurrentWheelPos;
	QTime* mTime;

	int mLastFrameTime;
	int mCurrentTime;

	bool mIsFirstFrame;

	float mCameraSpeed;

	/*ChooseMeshWidget* mChooseMeshWidget; */

	 MainMenu* mMainMenu;

	 StyleSettingsWidget* mStyleSettingsWidget;

	Ogre::Camera* mCamera;

	QtOgre::Log* mDemoLog;
	Ogre::SceneManager* mSceneManager;
};

#endif /* SIMULATORGAMELOGIC_H_ */
