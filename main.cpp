#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <stack>

using namespace std;

const int MAX_N = 23000;

// Represents the vertex objects, it stores the id and the adjacency map.
// It also has some methods to add and remove edges.
struct Vertex {
    int id;
	unordered_map<int, int> adjacencyMap;

    void addEdge(int id, int cost) {
    	adjacencyMap[id] = cost;
	}

	int removeEdge(int id) {
		int cost = adjacencyMap[id];
		adjacencyMap.erase(id);
		return cost;
	}

};

// I used the algorithm that is provided below to write this function. I did some changes according to my code.
// URL: https://konaeakira.github.io/posts/using-the-shortest-path-faster-algorithm-to-find-negative-cycles.html
// It reconstructs the negative cycle by doing modifications in the vector given as a parameter and gives the path.
void trace(int pre[], int v, int size, vector<int>* cycle) {
	stack<int> temp;
	bool on_stack[MAX_N];
	fill(on_stack, on_stack + size, false);
	while(!on_stack[v]) {
		temp.push(v);
		on_stack[v] = true;
		v = pre[v];
	}
	cycle->push_back(v);
	while(temp.top() != v) {
		cycle->push_back(temp.top());
		temp.pop();
	}
	cycle->push_back(v);
}


// I used the code that is provided in the link below to write this function. I did some changes according to my code.
// URL: https://konaeakira.github.io/assets/code-snippets/cycle-detection-with-spfa.cpp
// Detects the negative cycles in the graph if there is.
bool detect_cycle(unordered_map<int, Vertex>* graph, int pre[], int* v) {
	bool visited[MAX_N], on_stack[MAX_N];
	vector<int> vec;
    fill(on_stack, on_stack + graph->size(), false);
    fill(visited, visited + graph->size(), false);
    for (int i = 0; i < graph->size(); ++i)
        if (!visited[i])
        {
            for (int j = i; j != -1; j = pre[j])
                if (!visited[j])
                {
                    visited[j] = true;
                    vec.push_back(j);
                    on_stack[j] = true;
                }
                else
                {
                    if (on_stack[j]) {
                    	*v = j;
                        return true;
                    }
                    break;
                }
            for (int j : vec)
                on_stack[j] = false;
            vec.clear();
        }
    return false;
}

// I used the code that is provided in the link below to write this function. I did some changes according to my code.
// URL: https://konaeakira.github.io/assets/code-snippets/cycle-detection-with-spfa.cpp
bool spfa_early_terminate(unordered_map<int, Vertex>* graph) {
	long long dis[MAX_N];
	int pre[MAX_N], len[MAX_N];
	bool in_queue[MAX_N];

	fill(dis, dis + graph->size(), 0);
	fill(pre, pre + graph->size(), -1);
	fill(in_queue, in_queue + graph->size(), true);
	queue<int> queue;
	for (int i = 0; i < graph->size(); i++) {
		queue.push(i);
	}
    int iter = 0;
	while (!queue.empty())
	{
		int u = queue.front();
		queue.pop();
		in_queue[u] = false;
			for (auto map: graph->at(u).adjacencyMap) 
				if (dis[u] + map.second < dis[map.first])
				{
					pre[map.first] = u;
					dis[map.first] = dis[u] + map.second;
					if (iter++ == graph->size())
	                {
	                    iter = 0;
	                    int v = 0;
	                    // If detects a negative cycle, takes the path via trace method with the given parameters.
	                    if (detect_cycle(graph, pre, &v)) {
	                    	vector<int> temp;
	                    	trace(pre, v, graph->size(), &temp);
	                    	// Remove the edges in the path and add new edges in the reverse direction with the minus cost.
	                    	for(int i=0; i<temp.size()-1; i++) {
					    		int cost = graph->at(temp[i]).removeEdge(temp[i+1]);
					    		graph->at(temp[i+1]).addEdge(temp[i], -cost);
					    	}
					    	return true;
 						}
	                }
					if (!in_queue[map.first])
					{
						queue.push(map.first);
						in_queue[map.first] = true;
					}
				}
	}
	int k = 0;
	// The same algorithm with the above one.
	if (detect_cycle(graph, pre, &k)) {
		vector<int> temporary;
	    trace(pre, k, graph->size(), &temporary);
		for(int i=0; i<temporary.size()-1; i++) {
    		int cost = graph->at(temporary[i]).removeEdge(temporary[i+1]);
    		graph->at(temporary[i+1]).addEdge(temporary[i], -cost);
    	}
		return true;
    }
	return false;
}


int main(int argc, char* argv[]) {
	ifstream infile;
	infile.open(argv[1]);

	ofstream outfile;
	outfile.open(argv[2]);

	int numOfTestCases;
	infile >> numOfTestCases;

	// For loop to take all inputs in the input file test by test.
	for(int i=0; i<numOfTestCases; i++) {
		int numOfCables;
		infile >> numOfCables;

		// Map stores all vertices according to their IDs.
		unordered_map<int, Vertex> graph;

		// Creates all vertices at the beginning with the unique id and empty adjacency map.
		for(int k=0; k<2*numOfCables; k++) {
			graph[k] = Vertex{k, {}};
		}
		// I determined a maximum flow in each input. Every vertex in the left side is matched with the corresponding vertex in the right side.
		// It doesn't matter what flow is after all cables are connected. I connected the edges between the vertices that give the same remainder when divided by number of cables.		
		for(int k=0; k<numOfCables; k++) {
			for(int t=numOfCables; t<2*numOfCables; t++) {
				int cost;
				infile >> cost;
				// If the remainder is same, connects them with the voltage as a cost in the reverse direction.
				// If different, connects with the minus voltage value.
				if(t%numOfCables != k%numOfCables) {
					graph[k].addEdge(t, -cost);
				} else {
					graph[t].addEdge(k, cost);
				}
			}
		}

		// Loop continues until no negative cycles remain, it calls early terminate method.
		while(spfa_early_terminate(&graph)) {
			
		}
	
		// This loops are summing all the outdegree edges from the right side of the graph with the positive cost value.
		int total = 0;
		for(int i=numOfCables; i<2*numOfCables; i++) {
			for(auto map : graph[i].adjacencyMap) {
				if(map.second > 0) {
					total += map.second;
				}
			}
		}
		outfile << total << endl;
	}

	infile.close();
	outfile.close();
    return 0;
}
