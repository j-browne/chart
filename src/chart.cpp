#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "cmdline.h"

using namespace std;

class color {
public:
	int c[3];
};

class nucleus {
public:
	string name;
	int z;
	int n;
	string color;
};

void get_colors(gengetopt_args_info& args_info, map<string,color>& colors) {
	ifstream colfile(args_info.colors_arg);
	string str;
	while(getline(colfile, str), colfile.good()) {
		stringstream ss(str);
		string n;
		color c;

		ss >> n >> c.c[0] >> c.c[1] >> c.c[2];

		colors.insert(pair<string,color>(n,c));
	}
	colfile.close();
}

void get_nuclei(gengetopt_args_info& args_info, vector<nucleus>& nuclei) {
	ifstream nucfile(args_info.nuclei_arg);
	string str;
	while(getline(nucfile, str), nucfile.good()) {
		stringstream ss(str);
		nucleus n;

		ss >> n.name >> n.z >> n.n >> n.color;

		nuclei.push_back(n);
	}
	nucfile.close();
}

void get_elements(gengetopt_args_info& args_info, map<int,string>& elements) {
	ifstream elemfile(args_info.elements_arg);
	string str;
	while(getline(elemfile, str), elemfile.good()) {
		stringstream ss(str);
		int z;
		string n;

		ss >> z >> n;

		elements.insert(pair<int,string>(z,n));
	}
	elemfile.close();
}

void get_magic(gengetopt_args_info& args_info, vector<int>& magic) {
	ifstream magfile(args_info.magic_arg);
	string str;
	while(getline(magfile, str), magfile.good()) {
		stringstream ss(str);
		int n;

		ss >> n;

		magic.push_back(n);
	}
	magfile.close();
}

void output_svg(gengetopt_args_info& args_info, vector<nucleus>& nuclei, map<string,color>& colors, map<int,string>& elements, vector<int>& magic) {
	map<int,pair<int,int>> zLimits;
	map<int,pair<int,int>> nLimits;
	int maxZ=0;
	int maxN=0;
	const int scale = 10;

	/*
	 * Determine the limits of the chart
	 */
	for (vector<nucleus>::iterator it=nuclei.begin(); it!=nuclei.end(); ++it) {
		if (it->n > maxN) {
			maxN=it->n;
		}
		if (it->z > maxZ) {
			maxZ=it->z;
		}
	}

	/*
	 * Determine the lowest and highest Z for each N
	 * and lowest and highest N for each Z
	 */
	for (vector<nucleus>::iterator it=nuclei.begin(); it!=nuclei.end(); ++it) {
		if (zLimits.count(it->z)==0) {
			zLimits.insert(pair<int,pair<int,int>>(it->z,pair<int,int>(it->n,it->n)));
		} else if (it->n < zLimits[it->z].first) {
			zLimits[it->z].first = it->n;
		} else if (it->n > zLimits[it->z].second) {
			zLimits[it->z].second = it->n;
		}
		if (nLimits.count(it->n)==0) {
			nLimits.insert(pair<int,pair<int,int>>(it->n,pair<int,int>(it->z,it->z)));
		} else if (it->z < nLimits[it->n].first) {
			nLimits[it->n].first = it->z;
		} else if (it->z > nLimits[it->n].second) {
			nLimits[it->n].second = it->z;
		}
	}

	/*
	 * Output the SVG
	 */
	ofstream svgfile(args_info.output_arg);
	// Header
	svgfile << "<svg xmlns=\"http://www.w3.org/2000/svg\""
	        << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
	        << " width=\"" << (maxN+4)*scale << "\""
	        << " height=\"" << (maxZ+3)*scale << "\""
	        << ">" << endl;

	// Styling
	svgfile << "<style>" << endl;
	svgfile << ".nucBox{stroke:black;stroke-width:.1;}" << endl;
	svgfile << ".elName{text-anchor:end;}" << endl;
	svgfile << ".magBox{fill:none;stroke:black;stroke-width:.25;}" << endl;
	for (map<string,color>::iterator it=colors.begin(); it!=colors.end(); ++it) {
		svgfile << "." << it->first << " {"
		        << "fill:rgb(" << it->second.c[0] << ","
		                       << it->second.c[1] << ","
		                       << it->second.c[2] << ");"
		        << "}" << endl;
	}
	svgfile << "</style>" << endl;

	// Create Transform Group
	svgfile << "<g transform=\""
	        << "scale(" << scale << ")"
	        << " translate(2,1)\""
	        << ">" << endl;

	// Nuclide Boxes
	for (vector<nucleus>::iterator it=nuclei.begin(); it!=nuclei.end(); ++it) {
		svgfile << "<rect"
		        << " x=\"" << it->n << "\""
		        << " y=\"" << maxZ-(it->z) << "\""
		        << " width=\"1\""
		        << " height=\"1\""
		        << " class=\"" << "nucBox" << " " <<  it->color << "\""
		        << " />" << endl;
	}

	// Element Symbols
	for (map<int,string>::iterator it=elements.begin(); it!=elements.end(); ++it) {
		// Determine x position
		// Only include element symbol if one of its isotopes is included
		if (zLimits.count(it->first)) {
			int x=zLimits[it->first].first;
			if (zLimits.count((it->first)+1) && zLimits[(it->first)+1].first < x) {
				x=zLimits[(it->first)+1].first;
			}
			svgfile << "<text"
			        << " x=\"" << x-.25 << "\""
			        << " y=\"" << maxZ-(it->first)+.75 << "\""
			        << " font-size=\".9\""
			        << " class=\"" << "elName" << "\""
			        << ">"
			        << it->second
			        << "</text>" << endl;
		}
	}

	// Magic Number Outlines
	for (vector<int>::iterator it=magic.begin(); it!=magic.end(); ++it) {
		// Only include magic number outline if one of those isotones is include
		if (nLimits.count(*it)) {
			svgfile << "<rect"
			        << " x=\"" << *it << "\""
			        << " y=\"" << maxZ-nLimits[*it].second << "\""
			        << " width=\"1\""
			        << " height=\"" << nLimits[*it].second-nLimits[*it].first+1 << "\""
			        << " class=\"" << "magBox" << "\""
			        << " />" << endl;
		}
		// Only include magic number outline if one of those isotopes is include
		if (zLimits.count(*it)) {
			svgfile << "<rect"
			        << " x=\"" << zLimits[*it].first << "\""
			        << " y=\"" << maxZ-*it << "\""
			        << " width=\"" << zLimits[*it].second-zLimits[*it].first+1 << "\""
			        << " height=\"1\""
			        << " class=\"" << "magBox" << "\""
			        << " />" << endl;
		}
	}

	svgfile << "</g>" << endl;
	svgfile << "</svg>" << endl;
	svgfile.close();
}

int main(int argc, char *argv[]) {
	gengetopt_args_info args_info;
	map<string,color> colors;
	vector<nucleus> nuclei;
	vector<int> magic;
	map<int,string> elements;

	/*
	 * Parse Command Line Arguments
	 */
	if (cmdline_parser (argc, argv, &args_info) != 0) {
		exit(1);
	}

	/*
	 * Load in the colors
	 * The file is formatted as:
	 * name	r	g	b
	 */
	get_colors(args_info, colors);

	/*
	 * Load in the nuclei
	 * The file is formatted as:
	 * name	z	n	color
	 *
	 * The color is associated with a color name from the color file
	 */
	get_nuclei(args_info, nuclei);

	/*
	 * Load in the element names
	 * The file is formatted as:
	 * z	name
	 */
	get_elements(args_info, elements);

	/*
	 * Load in the magic numbers
	 * The file is formatted as:
	 * n
	 */
	get_magic(args_info, magic);

	/*
	 * Output the SVG
	 */
	output_svg(args_info, nuclei, colors, elements, magic);
}
