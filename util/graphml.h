//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(graphml_H)
#define graphml_H

#include <iostream>
#include <string>

class GraphmlWriter
{
    private:
        std::ostream * out_;
        unsigned edgeId_;
        bool isTree_;
        
        std::string getStringOfColor(unsigned colorId);
        
    public:
        GraphmlWriter(std::ostream * out = &std::cout);
        virtual ~GraphmlWriter();
        
	void printHeader();

        void printNode(unsigned id, unsigned colorId = 0, const char * label = 0);

	void printEdge(unsigned sourceId, unsigned targetId, bool cutted, const char * label = 0);

	void printFooter();
	
	unsigned getDefaultColorId();
	
	void isTree(bool tree);
};

#endif // graphml_H
