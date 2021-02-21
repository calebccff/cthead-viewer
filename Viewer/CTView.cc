#include <vector>

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>

#include "RenderMaps.hh"
#include "CTFile.hh"
#include "CTView.hh"
#include "Vars.h"

namespace ct
{
	CTView::CTView(ct::CTFile* file) : file(file), renderFunc(&renderSimple)
	{
	}

	void setRenderMap(const RenderMap map) {
		switch (map)
		{
			case CT_MAP_SIMPLE_RENDER:
				renderFunc = &renderSimple;
				break;
			case CT_MAP_TRANSFER_FUNCTION:
				renderFunc = &renderTransferFunction;
				break;
			default:
				renderFunc = &renderSimple;
				break;
		}
	}
} // namespace ct
