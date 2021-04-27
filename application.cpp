// application.cpp <Starter Code>
// Michal Fitrzyk & Remik Wisniewski
//
// University of Illinois at Chicago
//
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <set>
#include <queue>
#include <algorithm>
#include <cmath>

#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "graph.h"

using namespace std;
using namespace tinyxml2;

const double INF = numeric_limits<double>::max();
const long long INF2 = numeric_limits<long long>::max();

class prioritize {
  public:
    bool operator()(const pair<long long, double> &p1,
                    const pair<long long, double> &p2) const {
      if (p1.second == p2.second) {
        return p1.first > p2.first;
      }
      return p1.second > p2.second;
    }
};


vector<long long> DijkstraAlgo(graph<long long, double>& G, long long startV, 
  map<long long, double>& distances, map<long long, long long>& predeccesors) {
  vector<long long> visited;
  vector<long long> vertices;

  vertices = G.getVertices();

  priority_queue<pair<long long, double>,
    vector<pair<long long, double>>, prioritize> unvisitedQueue;

  for (auto &e : vertices) {
    distances[e] = INF;
    unvisitedQueue.push(make_pair(e, INF));
    predeccesors[e] = INF2;
  }

  distances[startV] = 0;
  unvisitedQueue.push(make_pair(startV, 0));
  while (!unvisitedQueue.empty()) {
    pair<long long, double> currentV = unvisitedQueue.top();
    unvisitedQueue.pop();
    if (distances[currentV.first] == INF) {
      break;
    } else if (std::find(visited.begin(), visited.end(),
               currentV.first) != visited.end()) {
      continue;
    } else {
      visited.push_back(currentV.first);
    }
    set<long long> neighbors;
    neighbors = G.neighbors(currentV.first);
    for (auto &e : neighbors) {
      double edgeWeight, altPathDist;
      G.getWeight(currentV.first, e, edgeWeight);
      altPathDist = distances[currentV.first] + edgeWeight;
      if (altPathDist < distances[e]) {
        distances[e] = altPathDist;
        unvisitedQueue.push(make_pair(e, altPathDist));
        predeccesors[e] = currentV.first;
      }
    }
  }

  return visited;
}

int main() {
  // maps a Node ID to it's coordinates (lat, lon)
  map<long long, Coordinates>  Nodes;
  // info about each footway, in no particular order
  vector<FootwayInfo>          Footways;
  // info about each building, in no particular order
  vector<BuildingInfo>         Buildings;
  XMLDocument                  xmldoc;

  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "") {
    filename = def_filename;
  }

  //
  // Load XML-based map file
  //
  if (!LoadOpenStreetMap(filename, xmldoc)) {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }

  //
  // Read the nodes, which are the various known positions on the map:
  //
  int nodeCount = ReadMapNodes(xmldoc, Nodes);

  //
  // Read the footways, which are the walking paths:
  //
  int footwayCount = ReadFootways(xmldoc, Footways);

  //
  // Read the university buildings:
  //
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  //
  // Stats
  //
  assert(nodeCount == (int)Nodes.size());
  assert(footwayCount == (int)Footways.size());
  assert(buildingCount == (int)Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;


  //
  // TO DO: build the graph, output stats:
  //

  graph<long long, double> G;

  for (auto const &e : Nodes) {
      G.addVertex(e.first);
  }

  for (auto const &e : Footways) {
    for (int i = 0; i < (int)e.Nodes.size() - 1; i++) {
        G.addEdge(e.Nodes[i], e.Nodes[i + 1],
            distBetween2Points(
                Nodes.at(e.Nodes[i]).Lat, Nodes.at(e.Nodes[i]).Lon,
                Nodes.at(e.Nodes[i + 1]).Lat, Nodes.at(e.Nodes[i + 1]).Lon));
        G.addEdge(e.Nodes[i + 1], e.Nodes[i],
            distBetween2Points(
                Nodes.at(e.Nodes[i]).Lat, Nodes.at(e.Nodes[i]).Lon,
                Nodes.at(e.Nodes[i + 1]).Lat, Nodes.at(e.Nodes[i + 1]).Lon));
    }
  }

  cout << "# of vertices: " << G.NumVertices() << endl;
  cout << "# of edges: " << G.NumEdges() << endl;
  cout << endl;

  //
  // Navigation from building to building
  //
  string startBuilding, destBuilding;

  cout << "Enter start (partial name or abbreviation), or #> ";
  getline(cin, startBuilding);

  while (startBuilding != "#") {
    cout << "Enter destination (partial name or abbreviation)> ";
    getline(cin, destBuilding);

    bool validStart = false, validEnd = false;
    string startName, endBuilding;

    Coordinates start, end;

    //
    // TO DO: lookup buildings, find nearest start and dest nodes,
    // run Dijkstra's alg, output distance and path to destination:
    //
    for (auto const &e : Buildings) {
        if (e.Abbrev == startBuilding) {
            validStart = true;
            startName = e.Fullname;
            start = e.Coords;
            break;
        }
    }

    if (!validStart) {
       for (auto const &e : Buildings) {
           if (e.Fullname.find(startBuilding) != string::npos
               || e.Fullname == startBuilding) {
                   startName = e.Fullname;
                   validStart = true;
                   start = e.Coords;
                   break;
           }
        }
    }
    
     for (auto const &e : Buildings) {
        if (e.Abbrev == destBuilding) {
            validEnd = true;
            endBuilding = e.Fullname;
            end = e.Coords;
            break;
        }
    }

    if (!validEnd) {
       for (auto const &e : Buildings) {
           if (e.Fullname.find(destBuilding) != string::npos
               || e.Fullname == destBuilding) {
                   endBuilding = e.Fullname;
                   validEnd = true;
                   end = e.Coords;
                   break;
           }
        } 
    }

    if (!validStart) {
        cout << "Start building not found" << endl;
    } else if (!validEnd) {
        cout << "Destination building not found" << endl;
    } else {
        cout << "Starting point:" << endl;
        cout << " " << startName << endl;
        cout << " " << "(" << start.Lat << ", " << start.Lon << ")" << endl;
        cout << "Destination point:" << endl;
        cout << " " << endBuilding << endl;
        cout << " " << "(" << end.Lat << ", " << end.Lon << ")" << endl;

        long long minStart, minDest;
        double startMinDist, endMinDist;
        minStart = Footways[0].Nodes[0];
        minDest = Footways[0].Nodes[0];
        startMinDist = distBetween2Points(Nodes[Footways[0].Nodes[0]].Lat,
                            Nodes[Footways[0].Nodes[0]].Lon,
                            start.Lat, start.Lon);
        endMinDist = distBetween2Points(Nodes[Footways[0].Nodes[0]].Lat,
                            Nodes[Footways[0].Nodes[0]].Lon,
                            end.Lat, end.Lon);                   
        for (auto const &e : Footways) {
            for (auto const &x : e.Nodes) {
                if (distBetween2Points(Nodes[x].Lat, Nodes[x].Lon, start.Lat,
                        start.Lon) < startMinDist) {
                    minStart = x;
                    startMinDist = distBetween2Points(Nodes[x].Lat,
                        Nodes[x].Lon, start.Lat, start.Lon);
                }
                if (distBetween2Points(Nodes[x].Lat, Nodes[x].Lon, end.Lat,
                        end.Lon) < endMinDist) {
                    minDest = x;
                    endMinDist = distBetween2Points(Nodes[x].Lat,
                        Nodes[x].Lon, end.Lat, end .Lon);
                }
            }
        }
        cout << endl;
        cout << "Nearest start node:" << endl;
        cout << " " << minStart << endl;
        cout << " (" << Nodes[minStart].Lat << ", " << Nodes[minStart].Lon
             << ")" << endl;
        cout << "Nearest destination node:" << endl;
        cout << " " << minDest << endl;
        cout << " (" << Nodes[minDest].Lat << ", " << Nodes[minDest].Lon
             << ")" << endl;
        cout << endl;

        map<long long, double> distances;
        map<long long, long long> predeccesors;

        cout << "Navigating with Dijkstra..." << endl;
        vector<long long> visited = 
            DijkstraAlgo(G, minStart, distances, predeccesors);
        if (distances[minDest] == INF) {
            cout << "Sorry, destination unreachable" << endl;
        } else {
            cout << "Distance to dest: " << distances[minDest] 
                 << " miles" << endl;
            vector<long long> path;
            long long currDouble = predeccesors[minDest];
            path.push_back(minDest);
            if (currDouble != INF2) {
                while (true) {
                    path.push_back(currDouble);
                    if (currDouble == minStart) {
                        break;
                    }
                    currDouble = predeccesors[currDouble];
                }
            }
            cout << "Path: ";
            for (int i = (int)path.size() - 1; i > -1; i--) {
                if (i == 0) {
                    cout << path[i];
                } else {
                    cout << path[i] << "->";
                }
            }
            cout << endl;   
        }
    }

    cout << endl;
    cout << "Enter start (partial name or abbreviation), or #> ";
    getline(cin, startBuilding);
    if (startBuilding == "#") {
        break;
    }
  }

  //
  // done:
  //
  cout << "** Done **" << endl;
  return 0;
}
