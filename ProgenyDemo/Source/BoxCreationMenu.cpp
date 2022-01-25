#include "BoxCreationMenu.h"

#include <Interface/Frame.h>
#include <Interface/InterfaceFactory.h>
#include <Interface/Control/EditableText.h>
#include <cstdlib>


BoxCreationMenu::BoxCreationMenu(Horizon::IFrame* parent, Horizon::CInterfaceFactory* factory): 
	_parent(parent),
	_factory(factory)
{
	factory->SetStyle(INTERFACE_STYLE_WIREFRAME);
	_container = factory->CreateContainer(parent, -222.0f, 260.0f, 719.0f, 533.0f);
	_container->SetAnchor(Horizon::ORIGIN_TOPLEFT);
	_container->SetVisible(false);

	Horizon::SFrameDesc desc;
	desc.Label = "Length";
	desc.Anchor = Horizon::ORIGIN_TOPLEFT;
	desc.HAlign = Horizon::E_HALIGN::EHA_LEFT;
	Horizon::CStaticText* text = factory->CreateStaticText(_container, -340.0f, 220.0f, 220.0f, 40.0f, &desc);
	text->SetTextScale(36.0f);

	desc.Label = "Width";
	text = factory->CreateStaticText(_container, -340.0f, 160.0f, 220.0f, 40.0f, &desc);
	text->SetTextScale(36.0f);

	desc.Label = "Height";
	text = factory->CreateStaticText(_container, -340.0f, 100.0f, 220.0f, 40.0f, &desc);
	text->SetTextScale(36.0f);

	_length = factory->CreateEditableText(_container, -140.0f, 220.0f, 400.0f, 40.0f, &desc);
	_length->SetTextScale(36.0f);
	_length->SetText(L"1.0");

	_width = factory->CreateEditableText(_container, -140.0f, 160.0f, 400.0f, 40.0f, &desc);
	_width->SetTextScale(36.0f);
	_width->SetText(L"1.0");

	_height = factory->CreateEditableText(_container, -140.0f, 100.0f, 400.0f, 40.0f, &desc);
	_height->SetTextScale(36.0f);
	_height->SetText(L"1.0");
}

Horizon::CContainer* BoxCreationMenu::getContainer()
{
	return _container;
}

double BoxCreationMenu::getLength()
{
	wchar_t const* text = _length->GetText();
	return getDoubleFromWChar(text);
}

double BoxCreationMenu::getWidth()
{
	wchar_t const* text = _width->GetText();
	return getDoubleFromWChar(text);
}

double BoxCreationMenu::getHeight()
{
	wchar_t const* text = _height->GetText();
	return getDoubleFromWChar(text);
}

double BoxCreationMenu::getDoubleFromWChar(wchar_t const* text)
{
	char* textString = new char[wcslen(text)+1];
	#pragma warning ( suppress : 4996 )
	wcstombs( textString, text, wcslen(text) );
	double number = atof(textString);
	delete textString;
	return number;
}