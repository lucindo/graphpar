//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(partition_H)
#define partition_H

#include "tree.h"
#include "graph.h"
#include <set>
#include <algorithm>

// Classes and funcions needed for implementation
// of max-min q-partiton algorithm of Perl & Schach

// Each partiton componet has informantion about
// it's weight and vertices
struct PartitionComponent
{
    unsigned weight;
    int rootVertex;
    std::set<unsigned> vertices;
    
    PartitionComponent() : weight(0), rootVertex(-1)
    {
    }
    
    void addVertex(unsigned vertex)
    {
        vertices.insert(vertex);
    }
    
    void removeVertex(unsigned vertex)
    {
        vertices.erase(vertex);
    }
};

bool comparePartitionComponents(PartitionComponent p1, PartitionComponent p2);

class Partition
{
    private:
        std::vector<PartitionComponent> components_;
        unsigned lightestWeight_, lightestid_;
        unsigned heaviestWeight_, heaviestid_;
        
    public:
        Partition()
        {
            lightestWeight_ = heaviestWeight_ = 0;
        }
        
        Partition(const std::vector<PartitionComponent>& components) : components_(components)
        {
	    lightestWeight_ = std::numeric_limits<unsigned>::max();
	    heaviestWeight_ = std::numeric_limits<unsigned>::min();
	    for (unsigned c = 0; c < components.size(); ++c)
	    {
		if (lightestWeight_ > components[c].weight) 
		{
                    lightestWeight_ = components[c].weight;
                    lightestid_     = c;
		}
		if (heaviestWeight_ < components[c].weight) 
		{
                    heaviestWeight_ = components[c].weight;
                    heaviestid_     = c;
		}	
	    }
            if (heaviestid_ == lightestid_ and components.size() != 1)
            {
                for (unsigned c = 0; c < components.size(); ++c)
                {
                    if (components[c].weight == heaviestWeight_ and c != heaviestid_)
                    {
                        heaviestid_ = c;
                        break;
                    }
                }
            }
        }
        
        unsigned lightestWeight() const
        {
            return lightestWeight_;
        }
        
        unsigned heaviestWeight() const
        {
            return heaviestWeight_;
	}
	
	unsigned nComponents() const
	{
	   return components_.size();
	}
	
	unsigned lightestID() const
	{
	   return lightestid_;
	}

	unsigned heaviestID() const
	{
	   return heaviestid_;
	}
	
	PartitionComponent& component(unsigned id)
	{
	   return components_[id];
	}
	
	PartitionComponent& lightest()
	{
	   return component(lightestid_);
	}
	
	PartitionComponent& heaviest()
	{
	   return component(heaviestid_);
	}
	
	std::vector<PartitionComponent> getAllComponents() const
	{
	   return components_;
	}
	
	void sort()
	{
	   // sort by weight
	   std::sort(components_.begin(), components_.end(), comparePartitionComponents);	   
	}
};

void getTreeFromRandomTree(const Graph& graph, Tree * tree, bool shouldAlloc);

void partitionByPerlSchach(Tree& tree, unsigned q, std::vector<PartitionComponent>& components, unsigned * iterations = 0);

//
// Auxiliary functions and Classes
//

// Scoped guard to temporary removal of cuts
class CutRemoveGuard
{
    private:
        const TreeEdge * edge_;
        
    public:
        CutRemoveGuard(const TreeEdge * edge) : edge_(edge)
        {
            this->edge_->removeCut();
        }
        
        ~CutRemoveGuard()
        {
            this->edge_->addCut();
        }
};

// Scoped guard to temporary addition of cuts
class CutAddGuard
{
    private:
        const TreeEdge * edge_;
        
    public:
        CutAddGuard(const TreeEdge * edge) : edge_(edge)
        {
            this->edge_->addCut();
        }
        
        ~CutAddGuard()
        {
            this->edge_->removeCut();
        }
};

// Initialization funcions
void initializeCuts(Tree& tree, unsigned q, std::vector<const TreeEdge *>& edgeCuts);
void initializeComponentsWeight(Tree& tree, std::vector<PartitionComponent>& components);

// General funcions
unsigned weightOfLightestComponent(std::vector<PartitionComponent>& components);
unsigned weightOfComponent(Tree& tree, unsigned startingVertex, std::set<unsigned>& vertices);
void recalculateWeightOfComponents(Tree& tree, std::vector<PartitionComponent>& components, unsigned startingVertex);

#endif // partition_H
