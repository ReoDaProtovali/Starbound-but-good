#pragma once
#include "Framework/Graphics/Text.hpp"
class dffonts {
	TextContext ftctx;
public:
	Font videotype = { "res/fonts/videotype.ttf", ftctx };
};

extern dffonts DefaultFonts;