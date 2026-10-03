// PoliceCar: the police car the PolicePopup sends to arrest a special customer.
#pragma once

#include "engine/RefPtr.h"
#include "Vehicle.h"

namespace engine
{
	class AnimImage;
	class Image;
}

class CustomerTile;
class VehicleType;

// The "PoliceCar" VehicleType with a flashing red/blue light bar and the "special_police" siren; arrests the
// customer if it is the special one, else a false arrest. The members end at +0x190; the vtordisp (+0x190) and the
// Interface subobject (+0x194) follow them (0x198 bytes).
class PoliceCar : public Vehicle
{
public:
	PoliceCar();
	virtual ~PoliceCar();

	virtual void completeDelivery();							// slot 83 (Vehicle)
	virtual void dispatchTo(CustomerTile* customer);			// slot 85 (Vehicle)
	virtual void setDefinition(VehicleType* def);				// slot 88 (Vehicle)
	virtual void faceTowards(const engine::Point& tile);		// slot 89 (Vehicle)

	engine::RefPtr<engine::AnimImage> m_lights;					// +0x184 light bar, frames m_redLight and m_blueLight
	engine::RefPtr<engine::Image> m_redLight;					// +0x188 res\fx\policeLightRed.jpg
	engine::RefPtr<engine::Image> m_blueLight;					// +0x18C res\fx\policeLightBlue.jpg
};
