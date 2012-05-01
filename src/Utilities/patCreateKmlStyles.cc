/*
 * patCreateKmlStyles.cc
 *
 *  Created on: Nov 23, 2011
 *      Author: jchen
 */

#include "patCreateKmlStyles.h"
#include "patTransportMode.h"
#include "kml/dom.h"
#include<iostream>
#include<sstream>
#include<fstream>
#include "patDisplay.h";
#include "kml/base/color32.h"

using kmldom::DocumentPtr;
using kmldom::IconStylePtr;
using kmldom::IconStyleIconPtr;
using kmldom::KmlFactory;
using kmldom::StylePtr;
using kmldom::LineStylePtr;
using kmldom::ColorStylePtr;

patCreateKmlStyles::patCreateKmlStyles() {

}

patCreateKmlStyles::~patCreateKmlStyles() {
}

 DocumentPtr patCreateKmlStyles::createStylesForKml() {

	KmlFactory* kml_factory = KmlFactory::GetFactory();
	DocumentPtr document=kml_factory->CreateDocument();

	StylePtr gps = kml_factory->CreateStyle();
	gps->set_id("gps");
	IconStylePtr iconstyle = kml_factory->CreateIconStyle();
	iconstyle->set_scale(1.1);
	iconstyle->set_color(kmlbase::Color32("ff0000ff"));
	IconStyleIconPtr icon = kml_factory->CreateIconStyleIcon();
	icon->set_href("http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png");
	iconstyle->set_icon(icon);

	gps->set_iconstyle(iconstyle);
	document->add_styleselector(gps);

	StylePtr stop = kml_factory->CreateStyle();
	stop->set_id("stop");
	iconstyle = kml_factory->CreateIconStyle();
	iconstyle->set_scale(0.5);
	 icon = kml_factory->CreateIconStyleIcon();
	icon->set_href("http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png");
	iconstyle->set_icon(icon);
	stop->set_iconstyle(iconstyle);
	document->add_styleselector(stop);

	vector<string> colors;
	colors.push_back("ffffff00");
	colors.push_back("ffff00ff");
	colors.push_back("ff00ffff");
	colors.push_back("ff00ff00");
	colors.push_back("ff0000ff");
	colors.push_back("ffff0000");
	for(int i=0; i<TransportMode(NONE); i++)
	{
		TransportMode current_mode = (TransportMode) i;
		std::stringstream ss;
		ss<<current_mode;
		string mode_id = ss.str();

		StylePtr mode_style = kml_factory->CreateStyle();
		mode_style->set_id(getTransportMode(current_mode));

		LineStylePtr linestyle = kml_factory->CreateLineStyle();
		linestyle->set_width(i+5);
		linestyle->set_color(kmlbase::Color32(colors[i]));
		mode_style->set_linestyle(linestyle);
		document->add_styleselector(mode_style);
	}


	StylePtr ddr_style = kml_factory->CreateStyle();
	ddr_style->set_id("ddr");

	LineStylePtr linestyle = kml_factory->CreateLineStyle();
	linestyle->set_width(3);
	linestyle->set_color(kmlbase::Color32("ffffffff"));
	ddr_style->set_linestyle(linestyle);
	document->add_styleselector(ddr_style);

	return document;

}
