#include <iostream>
#include <fstream>
#include <vector>

#define PIKACHU_HP 273
#define BLASTOISE_HP 361

using namespace std;

class Pokemon{
public:
  int hp;
  int pp;
};

class Stats{
public:
  char turn;
  double prob;
  int level;
  bool isLeaf;
};

class Node{
public:
  Pokemon pikachu;
  Pokemon blastoise;
  Stats stats;

  Node(){
    pikachu.hp = PIKACHU_HP;
    pikachu.pp = 100;
    blastoise.hp = BLASTOISE_HP;
    blastoise.pp = 100;
    stats.turn = 'P';
    stats.prob = 1;
    stats.level = 0;
    stats.isLeaf = false;
  }
};

class Graph{

};

int main(int argc, char const *argv[]) {

  return 0;
}
