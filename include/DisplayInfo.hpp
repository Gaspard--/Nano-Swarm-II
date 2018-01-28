#pragma once

#include <unordered_map>
//#include "Hud.hpp"
enum ID_BLOCK
{
	BLOC_TIMER,
	BLOC_SCORE,
	BLOC_UNIT_LIST,
	BLOC_UNIT,
	nb_bloc
};

struct HudBlock
{
	HudBlock() = default;
	HudBlock(const Rect &nBackground, const Rect &nRectMessage, const std::string &nMessage, unsigned int nFontSize, const claws::Vect<3u, float> &nColorMessage) :
		background(nBackground),
		rectMessage(nRectMessage),
		message(nMessage),
		fontSize(nFontSize),
		colorMessage(nColorMessage) {}
	Rect				background;
	Rect				rectMessage;
	std::string			message;
	unsigned int		fontSize;
	claws::Vect<3u, float>	colorMessage;
};

struct DisplayInfo
{
  std::unordered_map<GLuint, std::vector<Renderable>> renderables;
  std::unordered_map<GLuint, std::vector<EntityRenderable>> entityRenderables;
  std::vector<std::pair<claws::Vect<2u, float>, claws::Vect<2u, float>>> lines;
  std::vector<HudBlock> hud;
};
