// engine::SplatFactory: creates splats (pop-up texts and images) from the definitions of res\manifests\splats.xml.
#pragma once

#include <map>
#include <string>
#include <vector>

#include "Container.h"

namespace engine
{
	class Splat;
	struct SplatDef;
	struct UpdateContext;

	// Container +0x00, members from +0x128, then the vtordisp (+0x144) and the Interface subobject (+0x148): 0x14C
	// bytes. New splats become children at the next update; children whose life time ran out are finished and
	// removed.
	class SplatFactory : public Container
	{
	public:
		SplatFactory();
		virtual ~SplatFactory();

		virtual std::string getTypeName() const;	// slot 24 (engine::Component): "SplatFactory"
		virtual void update(UpdateContext& context);	// slot 37 (engine::Component)
		virtual void updateBounds();				// slot 38 (engine::Component): empty bounds (body folded: 0x473AA0)

		bool load(const std::string& fileName);
		void addSplat(Splat* splat);
		Splat* createSplat(const std::string& type, float x, float y, void (*onFinished)(void*), void* userData);
		void addSplatDef(const std::string& type, SplatDef* def);

		std::map<std::string, SplatDef*> m_splatDefs;	// +0x128 definitions by type name, deleted by the destructor
		std::vector<Splat*> m_newSplats;				// +0x134 splats created since the last update
	};
}
