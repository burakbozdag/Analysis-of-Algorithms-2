#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <climits>

using namespace std;

class Graph{
    vector <vector<int>> capacity;
    vector <vector<int>> flow;
    int nodeCount;
    vector <int> excess;
    vector <int> height; 
    public:
    void addCapacity(int, int, int);
    void push(int, int);
    void relabel(int);
    void initializePreflow(int);
    int overflowingVertex();
    int findMaxFlow();
    Graph(int);
    ~Graph();
};

Graph::Graph(int n){
    nodeCount = n;
    capacity.assign(nodeCount, vector<int>(nodeCount, 0));
}

void Graph::addCapacity(int from, int to, int cap){
    capacity[from][to] = cap;
}

int Graph::overflowingVertex(){
		for(int i = 1; i < nodeCount - 1; ++i){ // Except source and destination
    		if(excess[i] > 0){
    				return i;
    		}
    }
    return -1;
}

void Graph::initializePreflow(int source){
		height.assign(nodeCount, 0);
		excess.assign(nodeCount, 0);
		
		flow.assign(nodeCount, vector<int>(nodeCount, 0));
		
		height[source] = nodeCount;
		
		for(int v = 0; v < nodeCount; ++v){
				if(capacity[source][v] != 0){
						flow[source][v] = capacity[source][v];
						flow[v][source] = -1 * capacity[source][v];
						excess[v] = capacity[source][v];
						excess[source] -= capacity[source][v];
				}
		}
}

void Graph::push(int u, int v){
		int delta;
		if(excess[u] > capacity[u][v] - flow[u][v]){
				delta = capacity[u][v] - flow[u][v];
		} else{
				delta = excess[u];
		}
		flow[u][v] += delta;
		flow[v][u] -= delta;
		excess[u] -= delta;
		excess[v] += delta;
}

void Graph::relabel(int u){
		int minHeight = INT_MAX;
		for(int v = 0; v < nodeCount; ++v){
				if(capacity[u][v] > flow[u][v]){
						if(minHeight > height[v]){
								minHeight = height[v];
						}
				}
		}
		height[u] = minHeight + 1;
}

int Graph::findMaxFlow(){
		int u = overflowingVertex();
		while(u != -1){
				bool pushed = false;
				for(int v = 0; v < nodeCount; ++v){
						if(height[v] < height[u] && flow[u][v] < capacity[u][v]){
								push(u, v);
								pushed = true;
								break;
						}
				}
				if(!pushed){
						relabel(u);
				}
				u = overflowingVertex();
		}
		return excess[nodeCount - 1];
}

Graph::~Graph(){

}

int main(int argumentNumber, char *arguments[]){
    int nodeCount;
    string filename = arguments[1];
    string tempString;
    int temp, from, to, flow;
    
    ifstream file;
    file.open(filename);
    file >> nodeCount;
    Graph graph(nodeCount);
    file >> tempString >> tempString >> tempString;
    while(file.good()){
        file >> from >> to >> flow;
        graph.addCapacity(from, to, flow);
    }
    graph.initializePreflow(0); // 0th node is source
    cout << "Maximum flow: " << graph.findMaxFlow() << endl; 
    return 0;
}
