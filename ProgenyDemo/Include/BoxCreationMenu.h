#ifndef _BOX_CREATION_MENU_H_
#define _BOX_CREATION_MENU_H_

namespace Horizon
{
	class CInterfaceFactory;
	class IFrame;
	class CContainer;
	class CEditableText;
}

class BoxCreationMenu
{
public:
	BoxCreationMenu(Horizon::IFrame* parent, Horizon::CInterfaceFactory* factory);

	double getLength();
	double getHeight();
	double getWidth();
	Horizon::CContainer* getContainer();

protected:

private:
	double getDoubleFromWChar(wchar_t const* text);

	Horizon::IFrame* _parent;
	Horizon::CInterfaceFactory* _factory ;

	Horizon::CContainer* _container;
	Horizon::CEditableText* _length;
	Horizon::CEditableText* _width;
	Horizon::CEditableText* _height;
};


#endif //_BOX_CREATION_MENU_H_