//Burak Bozdag
//150170110

#include <iostream>
#include <fstream>
#include <climits>
#include <deque>

using namespace std;

struct Path{
  int* time;
  int* parent;

  Path(int N) {
    time = new int[N];
    parent = new int[N];
  }
};

int minTime(int time[], bool sptSet[], int n) {
  int min = INT_MAX, min_index;
  for (int v = 0; v < n; ++v) {
    if (!sptSet[v] && time[v] <= min) {
      min = time[v];
      min_index = v;
    }
  }
  return min_index;
}

void printPath(int time[], int parent[], int j) {
  if (parent[j] == - 1) {
    return;
  }
  printPath(time, parent, parent[j]);
  cout << "Node: " << j << " Time: " << time[j] << endl;
}

void printSolution(int time[], int parent[], int src, int dest) {
  cout << "Node: " << src << " Time: " << time[src] << endl;
  printPath(time, parent, dest);
}

Path dijkstra(int** graph, int H, int D, int N) {
  int time[N];
  bool sptSet[N];
  int parent[N];
  for (int i = 0; i < N; ++i) {
    parent[H] = -1;
    time[i] = INT_MAX;
    sptSet[i] = false;
  }
  time[H] = 0;
  for (int count = 0; count < N; ++count) {
    int u = minTime(time, sptSet, N);
    sptSet[u] = true;
    for (int v = 0; v < N; ++v) {
      if (!sptSet[v] && graph[u][v] && time[u] + graph[u][v] < time[v]) {
        parent[v] = u;
        time[v] = time[u] + graph[u][v];
      }
    }
  }

  Path path(N);
  for (int i = 0; i < N; ++i) {
    path.time[i] = time[i];
    path.parent[i] = parent[i];
  }
  return path;
}

int main(int argc, char** argv) {
  ifstream readFile(argv[1]);
  int N;
  readFile >> N;
  int** matrix = new int*[N];
  for (int i = 0; i < N; ++i) { //Creating matrix
    matrix[i] = new int[N];
  }
  for (int i = 0; i < N; ++i) { //Initializing matrix
    for (int j = 0; j < N; ++j) {
      matrix[i][j] = 0;
    }
  }
  int JH, JD, LH, LD;
  readFile >> JH >> JD >> LH >> LD;
  if (JH == LH) { //Error checking
    cout << "No solution!" << endl;
    return 0;
  }
  int from, to, weight;
  while (readFile >> from >> to >> weight) { //Adding edges
    matrix[from][to] = weight;
  }

  Path josephH = dijkstra(matrix, JH, JD, N);
  Path josephD = dijkstra(matrix, JD, JH, N);
  Path lucyH = dijkstra(matrix, LH, LD, N);
  Path lucyD = dijkstra(matrix, LD, LH, N);

  //Extracting pairs
  deque<int> jhRoute;
  int currentNode = JD;
  jhRoute.push_front(currentNode);
  while (josephH.parent[currentNode] != -1) {
    currentNode = josephH.parent[currentNode];
    jhRoute.push_front(currentNode);
  }
  deque<int> lhRoute;
  currentNode = LD;
  lhRoute.push_front(currentNode);
  while (lucyH.parent[currentNode] != -1) {
    currentNode = lucyH.parent[currentNode];
    lhRoute.push_front(currentNode);
  }
  deque<int> jdRoute;
  currentNode = JH;
  jdRoute.push_front(currentNode);
  while (josephD.parent[currentNode] != -1) {
    currentNode = josephD.parent[currentNode];
    jdRoute.push_front(currentNode);
  }
  deque<int> ldRoute;
  currentNode = LH;
  ldRoute.push_front(currentNode);
  while (lucyD.parent[currentNode] != -1) {
    currentNode = lucyD.parent[currentNode];
    ldRoute.push_front(currentNode);
  }

  //Checking and fixing intersections
  //Between jhRoute and lhRoute (joseph.timeH & lucy.timeH)
  int intersection = -1;
  for (int j = 1; j < jhRoute.size(); ++j) {
    for (int l = 0; l < lhRoute.size(); ++l) {
      if (jhRoute[j] == lhRoute[l]
          && josephH.time[jhRoute[j]] == lucyH.time[lhRoute[l]]) {
        intersection = jhRoute[j];
        break;
      }
    }
    if (intersection != -1) {
      break;
    }
  }
  if (intersection == JD || intersection == LD
      || intersection == JH || intersection == LH) {
    cout << "No solution!" << endl;
    return 0;
  }
  if (intersection != -1) {
    int** newMatrix = new int*[N];
    for (int i = 0; i < N; ++i) {
      newMatrix[i] = new int[N];
    }

    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        newMatrix[i][j] = matrix[i][j];
      }
    }
    for (int i = 0; i < N; ++i) { //Removing intersection
      newMatrix[i][intersection] = 0;
      newMatrix[intersection][i] = 0;
    }

    Path newLucyH = dijkstra(newMatrix, LH, LD, N); //Checking new route
    deque<int> newLhRoute;
    currentNode = LD;
    newLhRoute.push_front(currentNode);
    /*for (int i = 0; i < N; ++i) { //Error check
      if (newLucyH.parent[i] >= N || newLucyH.parent[i] <= -2) {
        newLucyH.parent[i] = -1;
      }
    }*/
    bool visited[N];
    for (int v = 0; v < N; ++v) {
      visited[v] = false;
    }
    while (newLucyH.parent[currentNode] != -1
           && newLucyH.parent[currentNode] < N
           && newLucyH.parent[currentNode] > -2) {
      currentNode = newLucyH.parent[currentNode];
      if (visited[currentNode]) {
        cout << "No solution!" << endl;
        return 0;
      }
      visited[currentNode] = true;
      newLhRoute.push_front(currentNode);
    }
    intersection = -1;
    for (int j = 1; j < jhRoute.size(); ++j) {
      for (int l = 0; l < newLhRoute.size(); ++l) {
        if (jhRoute[j] == newLhRoute[l]
            && josephH.time[jhRoute[j]] == newLucyH.time[newLhRoute[l]]) {
          intersection = jhRoute[j];
          cout << "No solution!" << endl;
          return 0;
        }
      }
    }
    lucyH = newLucyH;
    lhRoute = newLhRoute;
  }

  for (int i = 0; i < N; ++i) { //From destination to hotel
    josephD.time[i] += josephH.time[JD] + 30;
    lucyD.time[i] += lucyH.time[LD] + 30;
  }

  //Between jdRoute and ldRoute (joseph.timeD & lucy.timeD)
  intersection = -1;
  for (int j = 1; j < jdRoute.size(); ++j) {
    for (int l = 0; l < ldRoute.size(); ++l) {
      if (jdRoute[j] == ldRoute[l]
          && josephD.time[jdRoute[j]] == lucyD.time[ldRoute[l]]) {
        intersection = jdRoute[j];
        break;
      }
    }
    if (intersection != -1) {
      break;
    }
  }
  if (intersection == JD || intersection == LD
      || intersection == JH || intersection == LH) {
    cout << "No solution!" << endl;
    return 0;
  }
  if (intersection != -1) {
    int** newMatrix = new int*[N];
    for (int i = 0; i < N; ++i) {
      newMatrix[i] = new int[N];
    }

    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        newMatrix[i][j] = matrix[i][j];
      }
    }
    for (int i = 0; i < N; ++i) { //Removing intersection
      newMatrix[i][intersection] = 0;
      newMatrix[intersection][i] = 0;
    }

    Path newLucyD = dijkstra(newMatrix, LD, LH, N);
    deque<int> newLdRoute;
    currentNode = LH;
    newLdRoute.push_front(currentNode);
    /*for (int i = 0; i < N; ++i) { //Error check
      if (newLucyD.parent[i] >= N || newLucyD.parent[i] <= -2) {
        newLucyD.parent[i] = -1;
      }
    }*/
    bool visited[N];
    for (int v = 0; v < N; ++v) {
      visited[v] = false;
    }
    while (newLucyD.parent[currentNode] != -1
           && newLucyD.parent[currentNode] < N
           && newLucyD.parent[currentNode] > -2) {
      currentNode = newLucyD.parent[currentNode];
      if (visited[currentNode]) {
        cout << "No solution!" << endl;
        return 0;
      }
      visited[currentNode] = true;
      newLdRoute.push_front(currentNode);
    }
    for (int i = 0; i < N; ++i) { //From destination to hotel
      newLucyD.time[i] += lucyH.time[LD] + 30;
    }
    intersection = -1;
    for (int j = 1; j < jdRoute.size(); ++j) {
      for (int l = 0; l < newLdRoute.size(); ++l) {
        if (jdRoute[j] == newLdRoute[l]
            && josephD.time[jdRoute[j]] == newLucyD.time[newLdRoute[l]]) {
          intersection = jdRoute[j];
          cout << "No solution!" << endl;
          return 0;
        }
      }
    }
    lucyD = newLucyD;
    ldRoute = newLdRoute;
  }

  if (jhRoute.size() <= 1 || jdRoute.size() <= 1
      || lhRoute.size() <= 1 || ldRoute.size() <= 1) {
    cout << "No solution!" << endl;
    return 0;
  }

  cout << "Joseph's Path, duration: " << josephD.time[JH] << endl;
  printSolution(josephH.time, josephH.parent, JH, JD);
  cout << "-- return --" << endl;
  printSolution(josephD.time, josephD.parent, JD, JH);
  cout << endl;

  cout << "Lucy's Path, duration: " << lucyD.time[LH] << endl;
  printSolution(lucyH.time, lucyH.parent, LH, LD);
  cout << "-- return --" << endl;
  printSolution(lucyD.time, lucyD.parent, LD, LH);

  return 0;
}