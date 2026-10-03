// Blimp: the prize blimp that crosses the city after a perfect day and drops BalloonTips.
#pragma once

#include <string>

#include "engine/Point.h"
#include "engine/Range.h"
#include "engine/RefPtr.h"
#include "engine/sigslot.h"
#include "Vehicle.h"

namespace engine
{
	class Image;
	class SoundHandle;
}

class BuildingTile;
class CityMap;

// Made as "PrizeBlimp" by PrizeBlimpAction: flies across the screen with a looping engine sound and a scrolling
// dot-matrix banner (localized string 105 with the user's name), dropping 3 BalloonTips every 1-1.75 s. Bases:
// Vehicle +0x00, sigslot::has_slots<> +0x184 (unused by the blimp's code). The members end at +0x1B0; the vtordisp
// (+0x1B0) and the Interface subobject (+0x1B4) follow them (0x1B8 bytes).
class Blimp : public Vehicle, public sigslot::has_slots<>
{
public:
	Blimp(const std::string& name);
	virtual ~Blimp();

	virtual void start(CityMap* map, const engine::Point& pos, BuildingTile* owner);	// slot 90: pos in pixels
	virtual bool isFinished();									// slot 91
	virtual void setDirection(const engine::Vector2& dir);		// slot 92
	virtual void dropTips();									// slot 93

	virtual bool init();										// slot 80 (Vehicle)
	virtual void updateOnMap(engine::UpdateContext& ctx);		// slot 81 (Vehicle)
	virtual void faceTowards(const engine::Point& tile);		// slot 89 (Vehicle)

	void fly(engine::UpdateContext& ctx);
	void updateBanner(engine::UpdateContext& ctx);

	static engine::Range s_tipDropInterval;						// 0x53290C (1.0f, 1.75f): re-arms m_dropTimer
	static int s_pendingTips;									// 0x532958 zeroed by start, read by isFinished only

	engine::Vector2 m_velocity;									// +0x194 pixels per second
	float m_dropTimer;											// +0x19C seconds until the next dropTips
	engine::SoundHandle* m_engineSound;							// +0x1A0 looping "bonus_blimpEngine"; never released
	bool m_finished;											// +0x1A4 has left the screen
	engine::RefPtr<engine::Image> m_banner;						// +0x1A8 dot-matrix text image
	float m_bannerTime;											// +0x1AC banner scroll timer (8 s period)
};
