// The game's tuning constants: a block of named constants in .rdata (0x4FA1FC-0x4FA2B8) that code all over the
// game loads from memory, so they are extern constants defined in one object (Constants.cpp). Names marked
// "guess" in Constants.cpp come from how the code uses them; the unused ones are named after their address.
#pragma once

extern const float g_typerSpeed;			// 0x4FA1FC
extern const float g_typerDelay;			// 0x4FA200
extern const float g_typerWidth;			// 0x4FA204
extern const float g_dailyReportDelay;		// 0x4FA208
extern const float g_unused4FA20C;			// 0x4FA20C
extern const float g_unused4FA210;			// 0x4FA210
extern const float g_levelIntroFadeTime;	// 0x4FA214
extern const float g_tipThreeStarTime;		// 0x4FA218
extern const float g_tipTwoStarTime;		// 0x4FA21C
extern const float g_tipJarRollupTime;		// 0x4FA220
extern const int g_tipValueScale;			// 0x4FA224
extern const int g_unused4FA228;			// 0x4FA228
extern const int g_satisfactionPenalty;		// 0x4FA22C
extern const int g_startingSatisfaction;	// 0x4FA230
extern const int g_lowSatisfaction;			// 0x4FA234
extern const int g_pizzaEditorUnlock;		// 0x4FA238
extern const int g_unused4FA23C;			// 0x4FA23C
extern const int g_unused4FA240;			// 0x4FA240
extern const int g_unused4FA244;			// 0x4FA244
extern const int g_toppingPrice;			// 0x4FA248
extern const int g_premiumToppingPrice;		// 0x4FA24C
extern const int g_happinessBonus;			// 0x4FA250
extern const int g_frenzyComboSize;			// 0x4FA254
extern const int g_frenzyComboBonus;		// 0x4FA258
extern const int g_defaultTipValue;			// 0x4FA25C
extern const int g_thiefBillCount;			// 0x4FA260
extern const float g_billInterval;			// 0x4FA264
extern const int g_stolenBillCash;			// 0x4FA268
extern const float g_unused4FA26C;			// 0x4FA26C
extern const float g_orderShuffleTime;		// 0x4FA270
extern const float g_unused4FA274;			// 0x4FA274
extern const float g_orderPrepareDelay;		// 0x4FA278
extern const float g_copterLiftTime;		// 0x4FA27C
extern const int g_copterLiftHeight;		// 0x4FA280
extern const float g_unused4FA284;			// 0x4FA284
extern const float g_waveTimeScale;			// 0x4FA288
extern const float g_moneyCollectTime;		// 0x4FA28C
extern const float g_popupShowTime;			// 0x4FA290
extern const float g_popupCloseTime;		// 0x4FA294
extern const float g_unused4FA298;			// 0x4FA298
extern const float g_unused4FA29C;			// 0x4FA29C
extern const float g_unused4FA2A0;			// 0x4FA2A0
extern const float g_comboShowTime;			// 0x4FA2A4
extern const float g_comboFadeTime;			// 0x4FA2A8
extern const float g_comboDelay;			// 0x4FA2AC
extern const float g_orderPrepareTime;		// 0x4FA2B0
extern const float g_memoryFlipTime;		// 0x4FA2B4
extern const int g_maxWaveOrders;			// 0x4FA2B8
