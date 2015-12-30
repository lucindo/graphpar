//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "graphml.h"

#include <cstring>

GraphmlWriter::GraphmlWriter(std::ostream * out) : out_(out), edgeId_(0), isTree_(true)
{
    this->printHeader();
}

GraphmlWriter::~GraphmlWriter()
{
    this->printFooter();
}

void GraphmlWriter::isTree(bool tree)
{
    isTree_ = tree;
}

void GraphmlWriter::printHeader()
{
    *out_ << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    *out_ << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns/graphml\"" << std::endl;
    *out_ << "         xmlns:y=\"http://yworks.com/xml/graphml\"" << std::endl;
    *out_ << "         xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" << std::endl;
    *out_ << "         xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns/graphml" << std::endl;
    *out_ << "         http://www.yworks.com/xml/schema/graphml/1.0/ygraphml.xsd\">" << std::endl;
    *out_ << "  <key id=\"node\" for=\"node\" yfiles.type=\"nodegraphics\"/>" << std::endl;
    *out_ << "  <key id=\"edge\" for=\"edge\" yfiles.type=\"edgegraphics\"/>" << std::endl;
    *out_ << "  <graph id=\"graph\" edgedefault=\"directed\">" << std::endl;
}

void GraphmlWriter::printNode(unsigned id, unsigned colorId, const char * label)
{
    char label_[1024] = { 0, };
    if (label == 0)
    {
        sprintf(label_, "%d", id);
    }
    else
    {
        strncpy(label_, label, 1024);
    }
    
    *out_ << "    <node id=\"n" << id << "\">" << std::endl;
    *out_ << "      <data key=\"node\">" << std::endl;
    *out_ << "        <y:ShapeNode>" << std::endl;
    *out_ << "          <y:NodeLabel>" << label_ << "</y:NodeLabel>" << std::endl;
    *out_ << "          <y:Shape type=\"ellipse\"/>" << std::endl;
    *out_ << "          <y:Fill color=\"" << this->getStringOfColor(colorId) << "\"  transparent=\"false\"/>" << std::endl;
    *out_ << "        </y:ShapeNode>" << std::endl;
    *out_ << "      </data>" << std::endl;
    *out_ << "    </node>" << std::endl;
}

void GraphmlWriter::printEdge(unsigned sourceId, unsigned targetId, bool cutted, const char * label)
{
    *out_ << "    <edge id=\"e" << edgeId_++ << "\" source=\"n" << sourceId << "\" target=\"n" << targetId << "\">" << std::endl;
    *out_ << "      <data key=\"edge\">" << std::endl;
    *out_ << "        <y:PolyLineEdge>" << std::endl;
    if (this->isTree_)
    {
        *out_ << "          <y:Arrows source=\"none\" target=\"standard\"/>" << std::endl;
    }
    else
    {
        *out_ << "          <y:Arrows source=\"none\" target=\"none\"/>" << std::endl;
    }
    if (label != 0)
    {
        *out_ << "          <y:EdgeLabel>" << label << "</y:EdgeLabel>" << std::endl;
    }
    if (cutted)
    {
        *out_ << "<y:LineStyle type=\"dotted\" width=\"3.0\" color=\"#8f8f8f\" />" << std::endl;
    }
    *out_ << "        </y:PolyLineEdge>" << std::endl;
    *out_ << "      </data>" << std::endl;
    *out_ << "    </edge>" << std::endl;
}

void GraphmlWriter::printFooter()
{
    *out_ << "  </graph>" << std::endl;
    *out_ << "</graphml>" << std::endl;
}

unsigned GraphmlWriter::getDefaultColorId()
{
    return 0;
}

// see colors at http://www.pitt.edu/~nisg/cis/web/cgi/rgb.html
std::string GraphmlWriter::getStringOfColor(unsigned colorId)
{
    const char * color;
    
    switch (colorId)
    {
        case  0: color = "#ffe4b5"; break;
        case  1: color = "#8470ff"; break;
        case  2: color = "#add8e6"; break;
        case  3: color = "#8fbc8f"; break;
        case  4: color = "#bebebe"; break;
        case  5: color = "#bdb76b"; break;
        case  6: color = "#cd853f"; break;
        case  7: color = "#f08080"; break;
        case  8: color = "#ba55d3"; break;
        case  9: color = "#d8bfd8"; break;
        case 10: color = "#ffdead"; break;
        case 11: color = "#4876ff"; break;
        case 12: color = "#9ac0cd"; break;
        case 13: color = "#00cd66"; break;
        case 14: color = "#ffb90f"; break;
        case 15: color = "#cd3278"; break;
        default: color = "#ffe4b5"; break;
    };
    
    return std::string(color);
}
