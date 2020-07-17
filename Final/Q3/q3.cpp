#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <climits>

using namespace std;

struct Edge{
    int sourceNode;
    int destinationNode;
    int weight;
};

class Graph{
    int numberOfNodes;
    int numberOfEdges;
    vector<Edge> edges;
    
public:
    void setNumberOfNodes(int n){
        numberOfNodes = n;
    }
    int getNumberOfNodes(){
        return numberOfNodes;
    }
    void setNumberOfEdges(int n){
        numberOfEdges = n;
    }
    int getNumberOfEdges(){
        return numberOfEdges;
    }
    
    void AddEdge(int sourceNode, int destinationNode, int weight){
        Edge e = Edge();
        e.sourceNode = sourceNode;
        e.destinationNode = destinationNode;
        e.weight = weight;
        
        edges.push_back(e);
    }
    vector<Edge> getAllEdges(){
        return edges;
    }
    
    vector<Edge> getOutgoingEdges(int src){
        vector<Edge> outgoingEdges;
        
        for (Edge e : edges){
            if(e.sourceNode == src){
                outgoingEdges.push_back(e);
            }
        }
        
        return outgoingEdges;
    }
    
    int getWeightOfEdge(int src, int dst){
       int weight = 0;
       for (Edge e : edges){
           if(e.sourceNode == src && e.destinationNode == dst){
               //cout << "weigth: " << e.weight << endl;
               weight = e.weight;
           }
       }
       
       return weight;
    }
    
    void printGraph(){
        cout << "************* Graph data *************" << endl;
        cout << "Number of total edges: " << numberOfEdges << endl;
        cout << "Number of total nodes: " << numberOfNodes << endl << endl;
        
        cout << "Source node --> Destination node : Edge weigth" << endl;
        for (std::vector<Edge>::const_iterator i = edges.begin(); i != edges.end(); ++i){
            std::cout << i->sourceNode << " --> " ;
            std::cout << i->destinationNode << " : " ;
            std::cout << setw(2) << i->weight << endl ;
        }
        cout << "**************************************" << endl << endl;
    }
};

class BellmanFord{
    Graph G;
    int sourceNode;
    int destinationNode;
    int** M;
    bool graphContainsNegativeWeightCycle;
        
public:
    void setGraph(Graph g){
        G = g;
    }
    Graph getGraph(){
        return G;
    }
    void setSourceNode(int s){
        sourceNode = s;
    }
    int getSourceNode(){
        return sourceNode;
    }
    void setDestinationNode(int t){
        destinationNode = t;
    }
    int getDestinationNode(){
        return destinationNode;
    }
    bool buildDPTable(){
        graphContainsNegativeWeightCycle = false;
				
				int n = G.getNumberOfNodes();
        M = new int*[n];
        for(int i = 0; i < n; ++i){
        		M[i] = new int[n];
        }
        
        for(int i = 0; i < n; ++i){
        		M[0][i] = INT_MAX;
        }
				M[0][destinationNode] = 0;
        
        vector<Edge> edges = G.getAllEdges();
        
        for(int i = 1; i < n; ++i){
        		for(int v = 0; v < n; ++v){
        				M[i][v] = M[i - 1][v];
        				int opt = INT_MAX;
        				for(int e = 0; e < G.getNumberOfEdges(); ++e){
        						if(edges[e].sourceNode == v){
        								int w = edges[e].destinationNode;
        								int temp = M[i - 1][w] + edges[e].weight;
        								if(M[i - 1][w] == INT_MAX){
        										continue;
        								}
        								if(temp < opt){
        										opt = temp;
        								}
        						}
        				}
        				if(opt < M[i - 1][v]){
        						M[i][v] = opt;
        				}
        		}
        }
        
        for(int v = 0; v < n; ++v){
        		int opt = INT_MAX;
        		for(int e = 0; e < G.getNumberOfEdges(); ++e){
        				if(edges[e].sourceNode == v){
        						int w = edges[e].destinationNode;
        						int temp = M[n - 1][w] + edges[e].weight;
        						if(M[n - 1][w] == INT_MAX){
        								continue;
        						}
        						if(temp < opt){
        								opt = temp;
        						}
       					}
      			}
      			if(opt < M[n - 1][v]){
								graphContainsNegativeWeightCycle = true;
        				cout << "Graph contains negative weight cycle!" << endl;
        				return false;
      			}
      	}
        return true;
    }
    
    void printDPTable(){
        if(!graphContainsNegativeWeightCycle){
            cout << endl << "Dynamic programming table: (shortest paths from each node to destination node): " << endl << endl;
            
            int numberOfNodes = G.getNumberOfNodes();
            
            for(int i = 0; i <= numberOfNodes; i++){
                for(int j = 0; j <= numberOfNodes; j++){
                    if(i == 0 && j == 0)
                        cout << "\t";
                    else if(i != 0 && j == 0)
                        cout << setw(10) << "Iteration-" << i - 1;
                    else if(i == 0 && j != 0)
                        cout << "\t" << "Node-" << j - 1 << " ";
                    else{
                        if(M[i - 1][j - 1] == INT_MAX)
                            cout << "\t " << "\u221E";
                        else
                            cout << "\t" << setw(2) << M[i - 1][j - 1];
                    }
                }
                cout << endl;
            }
            cout << endl;
        }
    }
    
    void printShortestPathBetweenSourceAndDestination(){
    		int n = G.getNumberOfNodes();
				cout << endl << "Shortest path value from " << sourceNode << " to "
				<< destinationNode << ": " << M[n - 1][sourceNode] << endl;
				
				cout << "Shortest path from " << sourceNode << " to "
				<< destinationNode << ": " << sourceNode;
				vector<int> dist;
				for(int v = 0; v < n; ++v){
						dist.push_back(M[n - 1][v]);
				}
				vector<int> path;
				for(int i = 0; i < n; ++i){
						path.push_back(i);
				}
				
				bool swap;
				for(int i = 0; i < n - 1; ++i){
						swap = false;
						for(int j = 0; j < n - i - 1; ++j){
								if(dist[j] > dist[j + 1]){
										int temp = dist[j];
										dist[j] = dist[j + 1];
										dist[j + 1] = temp;
										temp = path[j];
										path[j] = path[j + 1];
										path[j + 1] = temp;
										swap = true;
								}
						}
						if(!swap){
								break;
						}
				}
				
				for(int i = 0; i < n; ++i){
						if(path[i] != sourceNode && path[i] != destinationNode){
								cout << " -> " << path[i];
						}
				}
				cout << " -> " << destinationNode << endl;
    }
};

int main(int argc, char* argv[])
{
    if(argc > 1){
        Graph graph;
        
        //read graph data from file
        string inputFileName = argv[1];
        string line;
        ifstream infile(inputFileName);
        
        getline(infile, line);
        graph.setNumberOfNodes(stoi(line));
        getline(infile, line);
        graph.setNumberOfEdges(stoi(line));

        int src, dst, w;
        while (infile >> src >> dst >> w) {
           graph.AddEdge(src, dst, w);
        }
        
        graph.printGraph();
        
        BellmanFord bf;
        bf.setGraph(graph);
        
        int dstNode;
        int srcNode;
        
        cout << "Enter the destination node: ";
        cin >> dstNode;
        
        bf.setDestinationNode(dstNode);
    
        if(bf.buildDPTable()){
            bf.printDPTable();
            
            cout << "Enter a source node: ";
            cin >> srcNode;
            bf.setSourceNode(srcNode);
            
            bf.printShortestPathBetweenSourceAndDestination();
        }
    }
    else{
        cout << "Error: please provide a test file!" << endl;
    }
    
    return 0;
}
