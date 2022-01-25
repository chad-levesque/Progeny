#ifndef _SPHERE_CREATION_MENU_H_
#define _SPHERE_CREATION_MENU_H_

#include <Toolbox/Event.h>

namespace Horizon
{
	class CInterfaceFactory;
	class IFrame;
	class CContainer;
	class CEditableText;
	class CRadioButton;
}

class SphereCreationMenu
{
public:
	static int TEXTURE_CHECKERBOARD;
	static int TEXTURE_EARTH;
	static int TEXTURE_EARTHLIKE;
	static int TEXTURE_ALIEN;


	SphereCreationMenu(Horizon::IFrame* parent, Horizon::CInterfaceFactory* factory);

	double getRadius();
	double getDetailLevel();
	Horizon::CContainer* getContainer();
	int getTextureType();

private:
	static bool changeTexture(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);
	double getDoubleFromWChar(wchar_t const* text);

	Horizon::IFrame* _parent;
	Horizon::CInterfaceFactory* _factory ;

	Horizon::CContainer* _container;
	int textureType;
	Horizon::CEditableText* _radius;
	Horizon::CEditableText* _detail;

	Horizon::CRadioButton* _checkboardButton;
	Horizon::CRadioButton* _earthButton;
	Horizon::CRadioButton* _earthlikeButton;
	Horizon::CRadioButton* _alienButton;
};


#endif // _SPHERE_CREATION_MENU_H_