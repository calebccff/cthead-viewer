#include <vector>

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>

#include "Vars.h"
namespace ct {

	// Is a view a top down view, front view or side view
	enum CTViewType {
		TOP = 0,
		FRONT,
		SIDE,
	};

	enum CTRenderType {
		SIMPLE = 0,
		VOLREND = 1,
	};

	struct CTView {
		enum CTViewType type;
		sf::Int32 width;
		sf::Int32 height;
		sf::Int32 depth;

		std::vector<sf::Uint8> pixBuf;
	};

	class CTFile {
	  private:
		//std::vector<sf::Int16> pixBuf;
		// The data can go negative, so we can't convert to unsigned yet!
		short pixMap[CT_IMAGE_SLICES][CT_IMAGE_HEIGHT][CT_IMAGE_WIDTH]; // z/x/y
		std::vector<struct CTView*> views;
		short minBrightness;
		short maxBrightness;

		sf::Color renderSimple(sf::Int16 val);
		sf::Color renderTF(sf::Int16 val);
		inline sf::Color mapToPixel(CTView *view, int z, int x, int y);

		// struct CTView* getViewTop(size_t slice);
		// struct CTView* getViewFront(size_t slice);
		// struct CTView* getViewSide(size_t slice);
		void getView_(CTView* view, size_t slice);

	  public:
		struct CTView* getView(const CTViewType type, size_t slice);
		enum CTRenderType renderType;

		CTFile(char* filename);
		~CTFile();
	};
}