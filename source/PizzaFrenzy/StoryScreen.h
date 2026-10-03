// StoryScreen: the story screen (story scripts, DAILY REPORT and BONUS REPORT, game-won totals, topping upgrade).
#pragma once

#include <string>
#include <vector>

#include "engine/FadeTimer.h"
#include "engine/RefPtr.h"
#include "engine/Screen.h"
#include "AnimatedContainer.h"

namespace engine
{
	class Bitmap;
	class Image;
	class ImageButton;
	class SoundHandle;
	class TextItem;
	class TextTyper;
}

class GameProgress;
class Level;
class StorySequence;
class Topping;
class ToppingUpgrade;

// res/screenLayouts/storyScreen.xml, created by the game with new (0x5B0 bytes) and kept at game+0xDC. The members
// end at +0x5A8; the vtordisp (+0x5A8) and the Interface subobject (+0x5AC) follow them.
class StoryScreen : public engine::Screen
{
public:
	StoryScreen();
	virtual ~StoryScreen();

	virtual void setStory(const std::string& file);				// slot 95
	virtual void initLayout();									// slot 96
	virtual void setLevel(Level* level);						// slot 97
	virtual void resetStory();									// slot 98
	virtual void playStory();									// slot 99
	virtual void loadStory();									// slot 100
	virtual void updateStory(engine::UpdateContext& ctx);		// slot 101
	virtual void initSlideAnimations();							// slot 102
	virtual void updateSlideAnimations();						// slot 103

	virtual void activate();									// slot 34 (engine::Component), folded body 0x443BB0
	virtual void deactivate();									// slot 35 (engine::Component)
	virtual void onKeyDown(int keyCode);						// slot 86 (engine::ScreenLayout)
	virtual void prepareShow();									// slot 91 (engine::Screen)
	virtual void show();										// slot 92 (engine::Screen)
	virtual void hide();										// slot 93 (engine::Screen)
	virtual void onAction(std::string action);					// slot 0 of engine::ActionListener (+0x1AC)

	void showGameWonTexts();
	void setQuitButtonVisible(bool visible);
	engine::Bitmap* getStoryImage(const std::string& name);
	void stopStory();
	engine::TextTyper* showSpeechBubble(const std::string& speaker);
	void initReport(GameProgress* progress);
	void showFunFact();
	void showGameWon();
	void initBonusReport();
	void finishBonusReport();
	void showToppingUpgrade(Topping* topping, ToppingUpgrade* upgrade, int level);
	void showChefTitle();
	void updateOrderTexts();
	void handleStoryEvent(const std::string& action, const std::string& param);
	void finishDailyReport();
	void createComboIcons();
	void createRatingStars();
	void initDailyReport();

	engine::RefPtr<engine::Container> m_pulldownScreenGroup;	// +0x1D4 "pulldownScreenGroup"
	engine::RefPtr<engine::Container> m_blackboardGroup;		// +0x1D8 "blackboardGroup"
	engine::RefPtr<engine::Container> m_pulldownScreen;			// +0x1DC "pulldownScreen": the story <image> pictures
	engine::RefPtr<engine::Container> m_blackboard;				// +0x1E0 "blackboard"
	engine::RefPtr<engine::Container> m_paula;					// +0x1E4 "Paula"
	engine::RefPtr<engine::Image> m_paulaBubble;				// +0x1E8 "PaulaBubble"
	engine::RefPtr<engine::TextTyper> m_paulaText;				// +0x1EC replaces "paulaText", child of m_paula
	engine::RefPtr<engine::Container> m_lisa;					// +0x1F0 "Lisa"
	engine::RefPtr<engine::Image> m_lisaBubble;					// +0x1F4 "LisaBubble"
	engine::RefPtr<engine::TextTyper> m_lisaText;				// +0x1F8 replaces "lisaText", child of m_lisa
	engine::RefPtr<engine::Container> m_lorenzo;				// +0x1FC "Lorenzo"
	engine::RefPtr<engine::Image> m_lorenzoBubble;				// +0x200 "LorenzoBubble"
	engine::RefPtr<engine::TextTyper> m_lorenzoText;			// +0x204 replaces "lorenzoText", child of m_lorenzo
	engine::RefPtr<engine::Container> m_niccolo;				// +0x208 "Niccolo"
	engine::RefPtr<engine::Image> m_niccoloBubble;				// +0x20C "NiccoloBubble"
	engine::RefPtr<engine::TextTyper> m_niccoloText;			// +0x210 replaces "niccoloText", child of m_niccolo
	engine::RefPtr<engine::TextItem> m_skipString;				// +0x214 "skipString" (Esc skips only while visible)
	engine::RefPtr<StorySequence> m_script;						// +0x218 the running story script
	std::string m_storyFile;									// +0x21C story XML set by setStory
	engine::RefPtr<engine::Container> m_stats;					// +0x238 "stats" (DAILY REPORT)
	engine::RefPtr<engine::TextItem> m_actualRevenue;			// +0x23C "actualRevenue"
	engine::RefPtr<engine::TextItem> m_salesTarget;				// +0x240 "salesTarget"
	engine::RefPtr<engine::TextItem> m_bonusSales;				// +0x244 "bonusSales"
	engine::RefPtr<engine::TextItem> m_totalScore;				// +0x248 "totalScore"
	engine::RefPtr<engine::TextItem> m_ordersDelivered;			// +0x24C "ordersDelivered"
	engine::RefPtr<engine::TextItem> m_foodBank;				// +0x250 "foodBank"
	engine::RefPtr<engine::Container> m_serviceRating;			// +0x254 "serviceRating": parent of the stars
	std::vector<engine::RefPtr<engine::Image> > m_stars;		// +0x258 rating stars (createRatingStars)
	engine::RefPtr<engine::Container> m_comboGroup;				// +0x268 "comboGroup": parent of the combo icons
	engine::RefPtr<engine::TextItem> m_longestCombo;			// +0x26C "longestCombo"
	std::vector<engine::RefPtr<engine::Image> > m_comboIcons;	// +0x270 combo icons (createComboIcons)
	engine::SoundHandle* m_rollupSound;							// +0x280 "menu_rollup"; raw, never released
	engine::RefPtr<engine::Container> m_bonusStats;				// +0x284 "bonusStats" (BONUS REPORT)
	engine::RefPtr<engine::TextItem> m_perfectToppings;			// +0x288 "perfectToppings"
	std::string m_toppingLineFormat;							// +0x28C text of perfectToppings ("%d x $%d = $%d")
	engine::RefPtr<engine::TextItem> m_goodToppings;			// +0x2A8 "goodToppings"
	engine::RefPtr<engine::TextItem> m_okayToppings;			// +0x2AC "okayToppings"
	engine::RefPtr<engine::TextItem> m_missedToppings;			// +0x2B0 "missedToppings"
	engine::RefPtr<engine::TextItem> m_toppingBonus;			// +0x2B4 "toppingBonus"
	engine::RefPtr<engine::TextItem> m_timeLeft;				// +0x2B8 "timeLeft"
	engine::RefPtr<engine::TextItem> m_timeLeftBonus;			// +0x2BC "timeLeftBonus"
	engine::RefPtr<engine::TextItem> m_bonusTotal;				// +0x2C0 "bonusTotal"
	engine::RefPtr<engine::TextItem> m_bonusTotalScore;			// +0x2C4 "bonusTotalScore"
	engine::RefPtr<engine::Container> m_gameWon;				// +0x2C8 "gameWon"
	engine::RefPtr<engine::TextItem> m_totalSales;				// +0x2CC "totalSales"
	engine::RefPtr<engine::TextItem> m_elapsedTime;				// +0x2D0 "elapsedTime"
	engine::RefPtr<engine::TextItem> m_finalFoodBank;			// +0x2D4 "finalFoodBank"
	GameProgress* m_progress;									// +0x2D8 the game's progress (initReport); raw
	float m_stepTimer;											// +0x2DC delay until the next report step
	float m_revealDelay;										// +0x2E0 g_dailyReportDelay (initDailyReport), never read
	int m_starIndex;											// +0x2E4 next star to reveal
	int m_bonusToCount;											// +0x2E8 bonus still to count into the totals
	int m_timeBonus;											// +0x2EC 100 * seconds left (bonus report)
	int m_bonus;												// +0x2F0 bonus earned
	int m_shownTotal;											// +0x2F4 total currently displayed
	int m_deliveredToCount;										// +0x2F8 orders still to count into the food bank
	int m_deliveredCount;										// +0x2FC orders delivered this day
	int m_comboCount;											// +0x300 longest combo of m_comboTopping
	engine::RefPtr<Topping> m_comboTopping;						// +0x304 topping with the longest combo
	int m_unused308;											// +0x308 never initialised or used
	int m_comboIndex;											// +0x30C next combo icon to reveal
	int m_shownFoodBank;										// +0x310 orders served to date, displayed
	engine::RefPtr<engine::FadeContainer> m_levelUpgradeGroup;	// +0x314 "levelUpgradeGroup" (TOPPING UPGRADE; fade slot 75)
	engine::RefPtr<engine::TextTyper> m_toppingDescription;		// +0x318 replaces "toppingDescription"
	engine::RefPtr<engine::TextItem> m_toppingName;				// +0x31C "toppingName"
	engine::RefPtr<engine::Image> m_toppingImage;				// +0x320 "toppingImage"
	engine::RefPtr<engine::TextItem> m_levelText;				// +0x324 "levelText"
	engine::RefPtr<engine::TextItem> m_toppingClass;			// +0x328 "toppingClass"
	engine::RefPtr<engine::TextItem> m_nextLevelText;			// +0x32C "nextLevelText"
	std::string m_nextLevelFormat;								// +0x330 text of nextLevelText ("(next level at %s)")
	std::string m_levelFormat;									// +0x34C text of levelText ("LEVEL %d")
	engine::RefPtr<engine::Container> m_quitButtonGroup;		// +0x368 "quitButtonGroup"
	engine::RefPtr<engine::ImageButton> m_quitButton;			// +0x36C "quitButton"
	engine::RefPtr<engine::Container> m_continueButtonGroup;	// +0x370 "continueButtonGroup" (only bound)
	engine::RefPtr<engine::ImageButton> m_continueButton;		// +0x374 "continueButton" (only bound)
	Level* m_level;												// +0x378 set by setLevel, never read
	int m_step;													// +0x37C 0 idle, 1 in, 2 story, 3 out, 4-12 daily, 13-21 bonus
	engine::FadeTimer m_screenTween;							// +0x380 drives m_screenSlide
	engine::FadeTimer m_blackboardTween;						// +0x3A4 drives m_blackboardSlide
	engine::PositionTrack m_screenSlide;						// +0x3C8 slide of m_pulldownScreenGroup
	engine::PositionTrack m_blackboardSlide;					// +0x418 slide of m_blackboardGroup
	engine::ScaleTrack m_scaleTrack1;							// +0x468 constructed and destroyed, never used
	engine::ScaleTrack m_scaleTrack2;							// +0x4B8 unused
	engine::ScaleTrack m_scaleTrack3;							// +0x508 unused
	engine::ScaleTrack m_scaleTrack4;							// +0x558 unused
};
