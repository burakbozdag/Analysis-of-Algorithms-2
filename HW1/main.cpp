#include <chrono>
#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#define PIKACHU_HP 273 //200 273
#define BLASTOISE_HP 361 //200 361

using namespace std;

struct Pokemon {
  int hp;
  int pp;
};

struct Stats {
  char turn;
  double prob;
  int level;
  bool isLeaf;
};

class Node {
public:
  Pokemon pikachu;
  Pokemon blastoise;
  Stats stats;
  string attackName;
  string effectiveness; //effective / noneffective
  Node* parent;
  vector<Node*> children;

  Node() {
    pikachu.hp = PIKACHU_HP;
    pikachu.pp = 100;
    blastoise.hp = BLASTOISE_HP;
    blastoise.pp = 100;
    stats.turn = 'P';
    stats.prob = 1;
    stats.level = 0;
    stats.isLeaf = false;
    parent = nullptr;
  }

  Node(const Node &copy) {
    pikachu.hp = copy.pikachu.hp;
    pikachu.pp = copy.pikachu.pp;
    blastoise.hp = copy.blastoise.hp;
    blastoise.pp = copy.blastoise.pp;
    stats.turn = copy.stats.turn;
    stats.prob = copy.stats.prob;
    stats.level = copy.stats.level;
    stats.isLeaf = copy.stats.isLeaf;
    children = copy.children;
  }
};

class Graph {
public:
  vector<Node*> nodes;

  Graph() {
    nodes.push_back(new Node());
  }

  int findIndex(Node* node) {
    int index;
    for (index = 0; index < nodes.size(); ++index) {
      if (nodes[index] == node)
        break;
    }
    return index;
  }
};

struct Attack {
  string attack;
  int pp;
  int accuracy;
  int damage;
  int firstUsage;
};

void easyFinder(Graph* graph, Node* node);

Graph* graphImplementation(int maxLevel, bool output, bool easy, char easyMode) {
  vector<Attack> pikachu;
  ifstream readP("pikachu.txt");
  string line;
  getline(readP, line);
  while (getline(readP, line)) { //Saving attacks for Pikachu
    line += ',';
    istringstream str(line);
    Attack attack;
    string temp;
    getline(str, temp, ','); //Get attack
    attack.attack = temp;
    //cout << attack.attack << ",";
    getline(str, temp, ','); //Get PP
    attack.pp = stoi(temp);
    //cout << attack.pp << ",";
    getline(str, temp, ','); //Get accuracy
    attack.accuracy = stoi(temp);
    //cout << attack.accuracy << ",";
    getline(str, temp, ','); //Get damage
    attack.damage = stoi(temp);
    //cout << attack.damage << ",";
    getline(str, temp, ','); //Get first usage
    attack.firstUsage = stoi(temp);
    //cout << attack.firstUsage << endl;
    pikachu.push_back(attack);
  }
  vector<Attack> blastoise;
  ifstream readB("blastoise.txt");
  getline(readB, line);
  while (getline(readB, line)) { //Saving attacks for Blastoise
    line += ',';
    istringstream str(line);
    Attack attack;
    string temp;
    getline(str, temp, ','); //Get attack
    attack.attack = temp;
    //cout << attack.attack << ",";
    getline(str, temp, ','); //Get PP
    attack.pp = stoi(temp);
    //cout << attack.pp << ",";
    getline(str, temp, ','); //Get accuracy
    attack.accuracy = stoi(temp);
    //cout << attack.accuracy << ",";
    getline(str, temp, ','); //Get damage
    attack.damage = stoi(temp);
    //cout << attack.damage << ",";
    getline(str, temp, ','); //Get first usage
    attack.firstUsage = stoi(temp);
    //cout << attack.firstUsage << endl;
    blastoise.push_back(attack);
  }
  /*
  cout << "Pikachu:" << endl;
  for (int i = 0; i < pikachu.size(); ++i) {
    cout << pikachu[i].attack << pikachu[i].pp << pikachu[i].accuracy
    << pikachu[i].damage << pikachu[i].firstUsage << endl;
  }
  cout << "Blastoise:" << endl;
  for (int i = 0; i < pikachu.size(); ++i) {
    cout << blastoise[i].attack << blastoise[i].pp << blastoise[i].accuracy
    << blastoise[i].damage << blastoise[i].firstUsage << endl;
  }
  */

  Graph* graph = new Graph;
  int i = 0;
  for (Node* node = graph->nodes[0];
  (node->stats.level < maxLevel);
  node = graph->nodes[++i]) { //For each node in the scope of maxLevel
    if (node->stats.turn == 'P') { //Pikachu's turn
      vector<Attack> pikachuAttacks;
      for (int j = 0; j < pikachu.size(); ++j) { //Possible attacks for current level
        if (pikachu[j].firstUsage <= node->stats.level
        && node->pikachu.pp >= (pikachu[j].pp * (-1))
        && node->pikachu.hp > 0 && node->blastoise.hp > 0
        && !(node->stats.isLeaf)) {
          if (pikachuAttacks.size() > 0 && pikachu[j].pp > 0)
            continue;
          else
            pikachuAttacks.push_back(pikachu[j]);
        }
      }
      //cout << "Pikachu attacks: " << pikachuAttacks.size() << endl;
      int attackCount = pikachuAttacks.size(); //Count of possible attacks
      for (int attackNum = 0; attackNum < attackCount; ++attackNum) { //For each attack
        Attack attack = pikachuAttacks[attackNum];
        if (attack.accuracy == 100) { //100% accuracy
          Node* child = new Node();
          child->pikachu.hp = node->pikachu.hp;
          child->pikachu.pp = node->pikachu.pp + attack.pp;
          child->blastoise.hp = node->blastoise.hp - attack.damage;
          child->blastoise.pp = node->blastoise.pp;
          child->stats.turn = 'B';
          child->stats.prob = node->stats.prob / attackCount;
          child->stats.level = node->stats.level + 1;
          child->stats.isLeaf = (child->stats.level < maxLevel) ? false : true;
          if (child->pikachu.hp <= 0 || child->blastoise.hp <= 0) {
            child->stats.isLeaf = true;
          }
          child->attackName = attack.attack;
          child->effectiveness = "effective";
          child->parent = node;
          node->children.push_back(child);
          graph->nodes.push_back(child);
          if (child->blastoise.hp <= 0) {
            if (easy && (easyMode == 'p')) {
              easyFinder(graph, child);
              return graph;
            }
          }
          //cout << child->stats.prob << endl;
        } else if (attack.accuracy == 0) { //0% accuracy
          Node* child = new Node();
          child->pikachu.hp = node->pikachu.hp;
          child->pikachu.pp = node->pikachu.pp + attack.pp;
          child->blastoise.hp = node->blastoise.hp;
          child->blastoise.pp = node->blastoise.pp;
          child->stats.turn = 'B';
          child->stats.prob = node->stats.prob / attackCount;
          child->stats.level = node->stats.level + 1;
          child->stats.isLeaf = (child->stats.level < maxLevel) ? false : true;
          child->attackName = attack.attack;
          child->effectiveness = "noneffective";
          child->parent = node;
          node->children.push_back(child);
          graph->nodes.push_back(child);
          //cout << child->stats.prob << endl;
        } else { //Between 0% and 100% accuracy
          Node* child = new Node(); //For accurate skill
          child->pikachu.hp = node->pikachu.hp;
          child->pikachu.pp = node->pikachu.pp + attack.pp;
          child->blastoise.hp = node->blastoise.hp - attack.damage;
          child->blastoise.pp = node->blastoise.pp;
          child->stats.turn = 'B';
          child->stats.prob = node->stats.prob / attackCount;
          child->stats.prob *= attack.accuracy / 100.0;
          child->stats.level = node->stats.level + 1;
          child->stats.isLeaf = (child->stats.level < maxLevel) ? false : true;
          if (child->pikachu.hp <= 0 || child->blastoise.hp <= 0) {
            child->stats.isLeaf = true;
          }
          child->attackName = attack.attack;
          child->effectiveness = "effective";
          child->parent = node;
          node->children.push_back(child);
          graph->nodes.push_back(child);
          if (child->blastoise.hp <= 0) {
            if (easy && (easyMode == 'p')) {
              easyFinder(graph, child);
              return graph;
            }
          }
          //cout << child->stats.prob << endl;

          child = new Node(); //For inaccurate skill
          child->pikachu.hp = node->pikachu.hp;
          child->pikachu.pp = node->pikachu.pp + attack.pp;
          child->blastoise.hp = node->blastoise.hp;
          child->blastoise.pp = node->blastoise.pp;
          child->stats.turn = 'B';
          child->stats.prob = node->stats.prob / attackCount;
          child->stats.prob *= (100.0 - attack.accuracy) / 100.0;
          child->stats.level = node->stats.level + 1;
          child->stats.isLeaf = (child->stats.level < maxLevel) ? false : true;
          child->attackName = attack.attack;
          child->effectiveness = "noneffective";
          child->parent = node;
          node->children.push_back(child);
          graph->nodes.push_back(child);
          //cout << child->stats.prob << endl;
        }
      }
    } else { //Blastoise's turn
      vector<Attack> blastoiseAttacks;
      for (int j = 0; j < blastoise.size(); ++j) { //Possible attacks for current level
        if (blastoise[j].firstUsage <= node->stats.level
        && node->blastoise.pp >= (blastoise[j].pp * (-1))
        && node->pikachu.hp > 0 && node->blastoise.hp > 0
        && !(node->stats.isLeaf)) {
          if (blastoiseAttacks.size() > 0 && blastoise[j].pp > 0)
            continue;
          else
            blastoiseAttacks.push_back(blastoise[j]);
        }
      }
      //cout << "Blastoise attacks: " << blastoiseAttacks.size() << endl;
      int attackCount = blastoiseAttacks.size(); //Count of possible attacks
      for (int attackNum = 0; attackNum < attackCount; ++attackNum) { //For each attack
        Attack attack = blastoiseAttacks[attackNum];
        if (attack.accuracy == 100) { //100% accuracy
          Node* child = new Node();
          child->pikachu.hp = node->pikachu.hp - attack.damage;
          child->pikachu.pp = node->pikachu.pp;
          child->blastoise.hp = node->blastoise.hp;
          child->blastoise.pp = node->blastoise.pp + attack.pp;
          child->stats.turn = 'P';
          child->stats.prob = node->stats.prob / attackCount;
          child->stats.level = node->stats.level + 1;
          child->stats.isLeaf = (child->stats.level < maxLevel) ? false : true;
          if (child->pikachu.hp <= 0 || child->blastoise.hp <= 0) {
            child->stats.isLeaf = true;
          }
          child->attackName = attack.attack;
          child->effectiveness = "effective";
          child->parent = node;
          node->children.push_back(child);
          graph->nodes.push_back(child);
          if (child->pikachu.hp <= 0) {
            if (easy && (easyMode == 'b')) {
              easyFinder(graph, child);
              return graph;
            }
          }
          //cout << child->stats.prob << endl;
        } else if (attack.accuracy == 0) { //0% accuracy
          Node* child = new Node();
          child->pikachu.hp = node->pikachu.hp;
          child->pikachu.pp = node->pikachu.pp;
          child->blastoise.hp = node->blastoise.hp;
          child->blastoise.pp = node->blastoise.pp + attack.pp;
          child->stats.turn = 'P';
          child->stats.prob = node->stats.prob / attackCount;
          child->stats.level = node->stats.level + 1;
          child->stats.isLeaf = (child->stats.level < maxLevel) ? false : true;
          child->attackName = attack.attack;
          child->effectiveness = "noneffective";
          child->parent = node;
          node->children.push_back(child);
          graph->nodes.push_back(child);
          //cout << child->stats.prob << endl;
        } else { //Between 0% and 100% accuracy
          Node* child = new Node(); //For accurate skill
          child->pikachu.hp = node->pikachu.hp - attack.damage;
          child->pikachu.pp = node->pikachu.pp;
          child->blastoise.hp = node->blastoise.hp;
          child->blastoise.pp = node->blastoise.pp + attack.pp;
          child->stats.turn = 'P';
          child->stats.prob = node->stats.prob / attackCount;
          child->stats.prob *= attack.accuracy / 100.0;
          child->stats.level = node->stats.level + 1;
          child->stats.isLeaf = (child->stats.level < maxLevel) ? false : true;
          if (child->pikachu.hp <= 0 || child->blastoise.hp <= 0) {
            child->stats.isLeaf = true;
          }
          child->attackName = attack.attack;
          child->effectiveness = "effective";
          child->parent = node;
          node->children.push_back(child);
          graph->nodes.push_back(child);
          if (child->pikachu.hp <= 0) {
            if (easy && (easyMode == 'b')) {
              easyFinder(graph, child);
              return graph;
            }
          }
          //cout << child->stats.prob << endl;

          child = new Node(); //For inaccurate skill
          child->pikachu.hp = node->pikachu.hp;
          child->pikachu.pp = node->pikachu.pp;
          child->blastoise.hp = node->blastoise.hp;
          child->blastoise.pp = node->blastoise.pp + attack.pp;
          child->stats.turn = 'P';
          child->stats.prob = node->stats.prob / attackCount;
          child->stats.prob *= (100.0 - attack.accuracy) / 100.0;
          child->stats.level = node->stats.level + 1;
          child->stats.isLeaf = (child->stats.level < maxLevel) ? false : true;
          child->attackName = attack.attack;
          child->effectiveness = "noneffective";
          child->parent = node;
          node->children.push_back(child);
          graph->nodes.push_back(child);
          //cout << child->stats.prob << endl;
        }
      }
    }
  }

  //Output the last layer's node information

  //double probProve = 0;
  if (output) {
      i = 0;
      cout << endl;
      for (Node* node = graph->nodes[0]; i < graph->nodes.size();
      node = graph->nodes[++i]) {
          if (node->stats.isLeaf) {
              cout << "P_HP:" << node->pikachu.hp
              << " P_PP:" << node->pikachu.pp
              << " B_HP:" << node->blastoise.hp
              << " B_PP:" << node->blastoise.pp
              << " PROB:" << node->stats.prob << endl;
              //probProve += node->stats.prob;
          }
      }
  }
  //cout << endl << "Total prob = " << probProve << endl;

  return graph;
}

void dfs(Graph* graph, Node* node, unsigned int* nodeCount,
    unsigned int* leafCount) { //Recursive DFS helper
  ++(*nodeCount);
  /*cout << "P_HP:" << node->pikachu.hp << " P_PP:" << node->pikachu.pp
        << " B_HP:" << node->blastoise.hp << " B_PP:" << node->blastoise.pp
        << " PROB:" << node->stats.prob << endl;*/
  if (node->stats.isLeaf) {
    ++(*leafCount);
    return;
  }
  int i = 0;
  for (Node* child = node->children[0]; i < node->children.size();
  child = node->children[++i]) {
    dfs(graph, child, nodeCount, leafCount);
  }
}

void bfsDfs(int maxLevel, char mode) {
  cout << "Implementing the graph..." << endl;
  Graph* graph = graphImplementation(maxLevel, false, false, '\0');
  cout << "Graph is implemented successfully." << endl;
  cout << "----------------------------------" << endl;

  auto start = chrono::high_resolution_clock::now(); //Start of clock
  unsigned int nodeCount = 0;
  unsigned int leafCount = 0;
  int i = 0;

  if (mode == 'b') { //BFS
    i = 0;
    for (Node* node = graph->nodes[0]; i < graph->nodes.size();
    node = graph->nodes[++i]) { //Graph is optimized for BFS
        ++nodeCount;
        if (node->stats.isLeaf)
          ++leafCount;
        /*cout << "P_HP:" << node->pikachu.hp << " P_PP:" << node->pikachu.pp
        << " B_HP:" << node->blastoise.hp << " B_PP:" << node->blastoise.pp
        << " PROB:" << node->stats.prob << endl;*/
    }
  } else { //DFS
    dfs(graph, graph->nodes[0], &nodeCount, &leafCount); //Recursive approach
  }

  auto stop = chrono::high_resolution_clock::now(); //End of clock
  auto duration = chrono::duration<double, milli>(stop - start);
  cout << "Node count: " << nodeCount << endl;
  cout << "Leaf count: " << leafCount << endl;
  cout << "Running time: " << duration.count() << " ms" << endl;
}

void easyFinder(Graph* graph, Node* node) {
  vector<Node*> resultStack;
  Node* leaf = node;
  while (node->parent != nullptr) {
    resultStack.push_back(node);
    node = node->parent;
  }
  cout << endl;
  int size = resultStack.size();
  for (int i = 0; i < size; ++i) {
    Node* node1 = resultStack.back();
    resultStack.pop_back();
    if (node1->stats.turn == 'B') {
      cout << "Pikachu used " << node1->attackName << ". It's " << node1->effectiveness << "." << endl;
    } else {
      cout << "Blastoise used " << node1->attackName << ". It's " << node1->effectiveness << "." << endl;
    }
  }
  cout << endl << "Level count : " << leaf->stats.level << endl;
  cout << "Probability : " << leaf->stats.prob << endl;
  //cout << "P_HP: " << leaf->pikachu.hp << " / B_HP: " << leaf->blastoise.hp << endl;
  //cout << "P_PP: " << leaf->pikachu.pp << " / B_PP: " << leaf->blastoise.pp << endl;
}

void easiestPath(char mode) {
  Graph* graph = graphImplementation(numeric_limits<int>::max(), false, true, mode);
}

int main(int argc, char const *argv[]) {
  if (argc != 3 && argc != 4) {
    cerr << "Invalid arguments." << endl;
    return 1;
  }
  int part = argv[1][4] - '0';
  int maxLevel;
  char mode;
  switch (part) {
  case 1: //Part 1
    if (argc != 3) {
      cerr << "Invalid arguments." << endl;
      return 1;
    }
    maxLevel = stoi(argv[2]);
    mode = '\0';
    graphImplementation(maxLevel, true, false, '\0');
    break;
  case 2: //Part 2
    if (argc != 4) {
      cerr << "Invalid arguments." << endl;
      return 1;
    }
    maxLevel = stoi(argv[2]);
    mode = argv[3][0];
    bfsDfs(maxLevel, mode);
    break;
  case 3: //Part 3
    if (argc != 3) {
      cerr << "Invalid arguments." << endl;
      return 1;
    }
    mode = argv[2][0];
    easiestPath(mode);
    break;
  default:
    cerr << "Invalid arguments." << endl;
    return 1;
  }

  return 0;
}
