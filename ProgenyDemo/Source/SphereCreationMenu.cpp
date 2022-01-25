#include "SphereCreationMenu.h"

#include <Interface/Frame.h>
#include <Interface/InterfaceFactory.h>
#include <Interface/Control/EditableText.h>
#include <Interface/Control/RadioButton.h>
#include <cstdlib>

int SphereCreationMenu::TEXTURE_CHECKERBOARD = 0;
int SphereCreationMenu::TEXTURE_EARTH = 1;
int SphereCreationMenu::TEXTURE_EARTHLIKE = 2;
int SphereCreationMenu::TEXTURE_ALIEN = 3;

SphereCreationMenu::SphereCreationMenu(Horizon::IFrame* parent, Horizon::CInterfaceFactory* factory): 
_parent(parent),
	_factory(factory)
{
	factory->SetStyle(INTERFACE_STYLE_WIREFRAME);
	_container = factory->CreateContainer(parent, 0.0f, 0.0f, 1000.0f, 533.0f);
	_container->SetVisible(true);
	textureType = TEXTURE_CHECKERBOARD;
	Horizon::SFrameDesc desc;
	desc.Label = "Radius";
	desc.Anchor = Horizon::ORIGIN_TOPLEFT;
	desc.HAlign = Horizon::EHA_LEFT;
	Horizon::CStaticText* text = factory->CreateStaticText(_container, -340.0f, 220.0f, 220.0f, 40.0f, &desc);
	text->SetTextScale(36.0f);

	desc.Label = "Detail";
	text = factory->CreateStaticText(_container, -340.0f, 160.0f, 220.0f, 40.0f, &desc);
	text->SetTextScale(36.0f);

	_radius = factory->CreateEditableText(_container, -140.0f, 220.0f, 400.0f, 40.0f, &desc);
	_radius->SetTextScale(36.0f);
	_radius->SetText(L"3.0");

	_detail = factory->CreateEditableText(_container, -140.0f, 160.0f, 400.0f, 40.0f, &desc);
	_detail->SetTextScale(36.0f);
	_detail->SetText(L"80");

	desc.Label = "Checkerboard";
	text = factory->CreateStaticText(_container, -300.0, 60.0f, 600.0f, 40.0f, &desc);
	text->SetTextScale(36.0f);
	text->SetCustomDataInt(SphereCreationMenu::TEXTURE_CHECKERBOARD);
	text->AddEventHandler(Horizon::EVNCLASS_STATICTEXT, Horizon::EVN_POINTER_CLICK, changeTexture, this);
	_checkboardButton = factory->CreateRadioButton(_container, -340.0f, 40.0f, 40.0f, 40.0f);

	desc.Label = "Earth";
	text = factory->CreateStaticText(_container, -300.0, 0.0f, 600.0f, 40.0f, &desc);
	text->SetTextScale(36.0f);
	text->SetCustomDataInt(SphereCreationMenu::TEXTURE_EARTH);
	_earthButton = factory->CreateRadioButton(_container, -340.0f, -20.0f, 40.0f, 40.0f);

	desc.Label = "Random Earthlike";
	text = factory->CreateStaticText(_container, -300.0, -60.0f, 600.0f, 40.0f, &desc);
	text->SetTextScale(36.0f);
	text->SetCustomDataInt(SphereCreationMenu::TEXTURE_EARTHLIKE);
	_earthlikeButton = factory->CreateRadioButton(_container, -340.0f, -80.0f, 40.0f, 40.0f);

	desc.Label = "Random Alien";
	text = factory->CreateStaticText(_container, -300.0, -120.0f, 600.0f, 40.0f, &desc);
	text->SetTextScale(36.0f);
	text->SetCustomDataInt(SphereCreationMenu::TEXTURE_ALIEN);
	_alienButton = factory->CreateRadioButton(_container, -340.0f, -140.0f, 40.0f, 40.0f);
}

bool SphereCreationMenu::changeTexture(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata) {
	SphereCreationMenu* menu = (SphereCreationMenu*)userdata;
	int textureType = frame->GetCustomDataInt();
	if (textureType == SphereCreationMenu::TEXTURE_CHECKERBOARD) {
		menu->_checkboardButton->SetEnabled(true);
	}
	else if (textureType == SphereCreationMenu::TEXTURE_EARTH) {
		menu->_earthButton->SetEnabled(true);
	}
	else if (textureType == SphereCreationMenu::TEXTURE_EARTHLIKE) {
		menu->_earthlikeButton->SetEnabled(true);
	}
	else if (textureType == SphereCreationMenu::TEXTURE_ALIEN) {
		menu->_alienButton->SetEnabled(true);
	}
	return true;
}

Horizon::CContainer* SphereCreationMenu::getContainer()
{
	return _container;
}

double SphereCreationMenu::getRadius()
{
	wchar_t const* text = _radius->GetText();
	return getDoubleFromWChar(text);
}

double SphereCreationMenu::getDetailLevel()
{
	wchar_t const* text = _detail->GetText();
	return getDoubleFromWChar(text);
}

int SphereCreationMenu::getTextureType() {
	if (_checkboardButton->GetValue() == Horizon::RADIO_CHECKED) {
		return TEXTURE_CHECKERBOARD;
	}
	if (_earthlikeButton->GetValue() == Horizon::RADIO_CHECKED) {
		return TEXTURE_EARTHLIKE;
	}
	if (_earthButton->GetValue() == Horizon::RADIO_CHECKED) {
		return TEXTURE_EARTH;
	}
	if (_alienButton->GetValue() == Horizon::RADIO_CHECKED) {
		return TEXTURE_ALIEN;
	}
}

double SphereCreationMenu::getDoubleFromWChar(wchar_t const* text)
{
	char* textString = new char[wcslen(text)+1];
#pragma warning ( suppress : 4996 )
	wcstombs( textString, text, wcslen(text) );
	double number = atof(textString);
	delete textString;
	return number;
}