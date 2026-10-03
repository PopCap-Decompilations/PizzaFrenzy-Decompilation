// UserProgress: the current user's saved progress (kept in the user's properties), with a ModeProgress per game mode.
#pragma once

#include <map>
#include <string>
#include <vector>

#include "engine/Object.h"
#include "engine/RefPtr.h"
#include "engine/sigslot.h"

class Topping;

// The saved record of one game mode (UserProgress::m_modes), "mode%d..." in the user's properties. Implicit
// constructor (0x427AA0) and destructor (0x427AC0).
class ModeProgress
{
public:
	int m_mode;									// +0x00 0..2, set by UserProgress::reset
	int m_lastCity;								// +0x04 "mode%dLastCity"
	int m_lastLevel;							// +0x08 "mode%dLastLevel"
	int m_lastPizza;							// +0x0C "mode%dLastPizza"
	int m_lastDay;								// +0x10 "mode%dLastDay"
	int m_lastScore;							// +0x14 "mode%dLastScore"
	float m_elapsedTime;						// +0x18 "mode%dElapsedTime"
	std::string m_lastTopping;					// +0x1C "mode%dLastTopping"
	int m_highestDay;							// +0x38 "mode%dHighestDay"
};

// The current user's progress (the game's instance is game+0x128): reloaded from the user's properties when the
// UserManager reports a change of user. Implicit destructor (0x428F90).
class UserProgress : public engine::Object, public sigslot::has_slots<>
{
public:
	UserProgress();

	int getLastLevel(int mode);
	void setLastLevel(int mode, int level);
	int getLastCity(int mode);
	void setLastCity(int mode, int city);
	int getLastDay(int mode);
	void setLastDay(int mode, int day);
	int getLastScore(int mode);
	void setLastScore(int mode, int score);
	int getHighestDay(int mode);
	void updateHighestDay(int mode, int day);
	float getElapsedTime(int mode);
	void setElapsedTime(int mode, float time);
	bool hasGameInProgress(int mode);
	std::vector<engine::RefPtr<Topping> >& getNewToppings();
	std::vector<engine::RefPtr<Topping> >& getPickedToppings();
	bool getUnlockTopping();
	void setUnlockTopping(bool unlock);
	int getMaxCity();
	void updateMaxCity(int city);
	void clearToppingLevels();
	std::string getToppingLevelsString();
	std::string getNewToppingsString();
	std::string getPickedToppingsString();
	void reset();
	bool save();
	bool clearGameInProgress(int mode);
	void removeNewTopping(Topping* topping);
	int getToppingLevel(Topping* topping);
	void setToppingLevel(Topping* topping, int level);
	void parseToppingLevels(const std::string& text);
	void parseNewToppings(const std::string& text);
	void parsePickedToppings(const std::string& text);
	void addNewTopping(Topping* topping);
	bool load();
	void onUserChanged();
	bool init();

	// +0x0C: sigslot::has_slots<>
	std::string m_userName;										// +0x1C copied from the user by load()
	int m_maxCity;												// +0x38 "maxCity"; only grows
	std::map<Topping*, int> m_toppingLevels;					// +0x3C "ToppingLevels" ("name=level,")
	std::vector<engine::RefPtr<Topping> > m_newToppings;		// +0x48 "NewToppings" ("name,")
	std::vector<engine::RefPtr<Topping> > m_pickedToppings;		// +0x58 "PickToppings" ("name,")
	bool m_unlockTopping;										// +0x68 "unlockTopping"
	ModeProgress m_modes[3];									// +0x6C one record per game mode
};
