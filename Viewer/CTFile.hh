#include <vector>

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>

#define CT_IMAGE_WIDTH 256
#define CT_IMAGE_HEIGHT 256
#define CT_IMAGE_SLICES 113 // this is the Z axis

namespace ct {

	// Is a view a top down view, front view or side view
	enum CTViewType {
		TOP = 0,
		FRONT,
		SIDE,
	};

	struct CTView {
		sf::Int32 width;
		sf::Int32 height;

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

		inline sf::Color mapToPixel(sf::Int16 val);

		struct CTView* getViewTop(size_t slice);
		struct CTView* getViewFront(size_t slice);
		struct CTView* getViewSide(size_t slice);

	  public:
		struct CTView* getView(const CTViewType view, size_t slice);

		CTFile(char* filename);
		~CTFile();
	};
}