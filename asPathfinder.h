// ============================================================================
// A* pathfinder
// ============================================================================
#include "asLinkedList.h"

#ifndef ASPATHFINDER_H
#define ASPATHFINDER_H

unsigned int asID = 0;

asList asNodeList;	
asList asOpendList;
asList asClosedList;
asList asChildList;
asList asPath;			

asZeroList(asNodeList);
asZeroList(asOpendList);
asZeroList(asClosedList);
asZeroList(asChildList);
asZeroList(asPath);

// Find waypoint nodes, that current node can see
function asFindChildren(asNode* node)
{
	asClearList(asChildList);
	
	asNodeList.itr = asNodeList.head;
	while(asNodeList.itr != NULL)
	{
		if(asNodeList.itr.ID != node.ID)
		{
			if((c_trace(node.pos, asNodeList.itr.pos, IGNORE_PASSABLE)) == 0)
			{
				asPrepend(asChildList, asNodeList.itr);
			}
		}
		asForth(asNodeList);
	}
}

// g: the actual shortest distance traveled from initial node to current node
// h: the estimated (or "heuristic") distance from current node to goal
// f: the sum of g and h
function asFindasPath(VECTOR* start, VECTOR* dest)
{
	var Path = -1;
	asNode* temp = NULL;
	
	asClearList(asOpendList);
	asClearList(asClosedList);
	asClearList(asPath);
	
	// Add the starting location to the open list
	asNode newnode;
	asZeroNode(newnode);
	newnode.h = vec_dist(start, dest);
	newnode.f = newnode.h;
	vec_set(newnode.pos, start);
	asPrepend(asOpendList, newnode);
	
	// Check to see if we can trace ray to the destination node without path finding
	if(c_trace(start, dest, IGNORE_PASSABLE) == 0)
	{
		// Add destination node
		asNode destnode;
		asZeroNode(destnode);
		vec_set(destnode.pos, dest);
		asPrepend(asPath, destnode);
		
		// Add start node
		asPrepend(asPath, newnode);
		
		// Path is found
		Path = 1;	
	}
	
	// Repeat until path is found, or it doesn't exist
	while(Path == -1)
	{
		// If open list isn't empty
		if(asOpendList.head != NULL)
		{
			// Find lowest f value in OpendList
			asOpendList.itr = asOpendList.head;
			var f = asOpendList.itr.f;
			while(asOpendList.itr != NULL)
			{
				if(asOpendList.itr.f < f)
					f = asOpendList.itr.f;
				asForth(asOpendList);	
			}
	
			// Find node with lowest f value, and drop it from open list to closed list
			asOpendList.itr = asOpendList.head;
			while(asOpendList.itr != NULL)
			{
				if(asOpendList.itr.f == f)
				{
					asPrepend(asClosedList, asOpendList.itr);
					// Find children for node with lowest f value
					asFindChildren(asOpendList.itr);
					asRemoveNode(asOpendList);
					
					break;
				}
				asForth(asOpendList);
			}
			
			// Check all child nodes
			asChildList.itr = asChildList.head;
			while(asChildList.itr != NULL)
			{
				int skip = 0;
				// If this node in the asClosedList skip this loop cycle
				asClosedList.itr = asClosedList.head;
				while(asClosedList.itr != NULL)
				{
					if(asChildList.itr.ID == asClosedList.itr.ID)
					{
						skip = 1;	
						break;
					}
					asForth(asClosedList);
				}
				
				if(skip == 1)
				{
					asForth(asChildList);
					continue;
				}
				
				// If this node is already on the asOpendList, check to see if this path to that node is better
				// (if the G score for that node is lower if we use the current node to get there)
				skip = 0;
				asOpendList.itr = asOpendList.head;
				while(asOpendList.itr != NULL)
				{
					if(asChildList.itr.ID == asOpendList.itr.ID)
					{
						skip = 1;
						var dist = vec_dist(asClosedList.head.pos, asChildList.itr.pos);
						if((asClosedList.head.g + dist) < asChildList.itr.g)
						{
							// If new G score better, recalculate F and G scores for this node
							// and change the "creator" of this node
							asRemoveNode(asOpendList);
							asChildList.itr.creator = asClosedList.head;							
							asChildList.itr.g = asClosedList.head.g + dist;
							asChildList.itr.h = vec_dist(asChildList.itr.pos, dest);
							asChildList.itr.f = asChildList.itr.g + asChildList.itr.h;
							asPrepend(asOpendList, asChildList.itr);
						}
					}
					asForth(asOpendList);
				}	
				
				// If current node isn't in asOpendList, add it to the asOpendList
				if(skip == 0)
				{
					asChildList.itr.creator = asClosedList.head;
					asChildList.itr.g = asClosedList.head.g + vec_dist(asClosedList.head.pos, asChildList.itr.pos);
					asChildList.itr.h = vec_dist(asChildList.itr.pos, dest);
					asChildList.itr.f = asChildList.itr.g + asChildList.itr.h;
					asPrepend(asOpendList, asChildList.itr);	
				}						
				
				// If current node can see target
				if((c_trace(asChildList.itr.pos, dest, IGNORE_PASSABLE)) == 0) 
				{
					// Remember this node 
					if(temp == NULL)
						temp = asChildList.itr;
					else
					{
						// If another child node has lower f value  
						if(asChildList.itr.f < temp.f)
						{
							temp = asChildList.itr;
						}
					}	
				}
				
				asForth(asChildList);
			}
			
			// If any of child nodes was able to see target, path is found
			if(temp != NULL)
			{
				// Add new node with destination coordinates
				asNode destnode;
				asZeroNode(destnode);
				vec_set(destnode.pos, dest);
				destnode.creator = temp;
				asPrepend(asPath, destnode);
				
				// Start from the destination node, go from each node to its 
				// creator node until we reach the starting node
				asNode* next = temp;
				while(next != NULL)
				{
					asPrepend(asPath, next);
					next = next.creator;
				}

				Path = 1;
				break;	
			}
		} 
		else // Path doesn't exist
		{
			Path = 0;
			break;	
		}
	}
	return Path;
}

/*action AddWaypoint()
{
	set(me, PASSABLE);
	asNode newnode;
	asZeroNode(newnode);
	vec_set(newnode.pos, my.x);
	asID += 1;
	newnode.ID = asID;
	asPrepend(asNodeList, newnode);
}*/

// Free all lists
function asFree()
{
	asClearList(asOpendList);
	asClearList(asClosedList);
	asClearList(asNodeList);
	asClearList(asPath);
}

#endif