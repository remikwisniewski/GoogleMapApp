// graph.h <Starter Code>
// Michal Fitrzyk & Remik Wisniewski
//
// Basic graph class using adjacency matrix representation.  Currently
// limited to a graph with at most 100 vertices.
//
// University of Illinois at Chicago
// CS 251: Fall 2020
// Project #7 - Openstreet Maps
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include <set>

using namespace std;

template<typename VertexT, typename WeightT>
class graph {
 private:

  map<VertexT, map<VertexT, WeightT>> AdjList;
  vector<VertexT> Vertices;

 public:
  //
  // constructor:
  //

  graph() {
  }

  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const {
    return static_cast<int>(this->AdjList.size());
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  int NumEdges() const {
    int count = 0;

    for (auto const &e : this->AdjList) {
        count += e.second.size();
    }

    return count;
  }

  //
  // addVertex
  //
  // Adds the vertex v to the graph if there's room, and if so
  // returns true.  If the vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v) {
    //
    // is the vertex already in the graph?  If so, we do not
    // insert again otherwise Vertices may fill with duplicates:
    //
    if (this->AdjList.count(v) > 0) {
      return false;
    }

    //
    // if we get here, vertex does not exist so insert.
    //
    Vertices.push_back(v);
    map<VertexT, WeightT> emptyMap;
    this->AdjList[v] = emptyMap;

    return true;
  }

  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true. If the vertices do not exist, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from, VertexT to, WeightT weight) {
    if (this->AdjList.count(from) <= 0) {  // not found:
      return false;
    }

    if (this->AdjList.count(to) <= 0) {  // not found:
      return false;
    }

    map<VertexT, WeightT> tempMap = AdjList[from];
    tempMap[to] = weight;

    AdjList[from] = tempMap;

    return true;
  }

  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const {
    if (this->AdjList.count(from) <= 0) {  // not found:
      return false;
    }

    if (this->AdjList.count(to) <= 0) {  // not found:
      return false;
    }

    //
    // the vertices exist, but does the edge exist?
    //
    if (AdjList.at(from).count(to) <= 0) {  // no:
      return false;
    }

    //
    // Okay, the edge exists, return the weight via the
    // reference parameter:
    //
    weight = AdjList.at(from).at(to);

    return true;
  }

  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v) const {
    set<VertexT>  S;

    // Checks if the vertex exists in the graph
    if (this->AdjList.count(v) <= 0) {  // not found:
      return S;
    }

    for (auto const &e : AdjList.at(v)) {
        S.insert(e.first);
    }

    return S;
  }

  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const {
    return this->Vertices;  // returns a copy:
  }

  //
  // dump
  //
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G(26);
  //    ...
  //    G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const {
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;

    output << "**Num vertices: " << this->NumVertices() << endl;
    output << "**Num edges: " << this->NumEdges() << endl;

    output << endl;
    output << "**Vertices:" << endl;
    for (int i = 0; i < this->NumVertices(); ++i) {
      output << " " << i << ". " << this->Vertices[i] << endl;
    }

    output << endl;
    output << "**Edges:" << endl;

    for (auto const &e : this->AdjList) {
        output << e.first << ": ";
        for (auto const &x : e.second) {
            output << "(" << e.first << "," << x.first << ","
                   << x.second << ") ";
        }
        output << endl;
    }

    output << "**************************************************" << endl;
  }
};
