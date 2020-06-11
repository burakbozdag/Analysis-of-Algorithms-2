//Burak Bozdag
//150170110

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int levenshtein(int* arr1, int* arr2, int len) { //Dynamic programmed edit distance calculator
  int array[2][len + 1]; //Previous result storing array
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j <= len; ++j) {
      array[i][j] = 0;
    }
  }
  for (int i = 0; i <= len; i++)
    array[0][i] = i;
  for (int i = 1; i <= len; i++) {
    for (int j = 0; j <= len; j++) {
      if (j == 0) {
        array[i % 2][j] = i;
      } else if (arr1[j - 1] == arr2[i - 1]) {
        array[i % 2][j] = array[(i - 1) % 2][j - 1];
      } else {
        array[i % 2][j] = 1 +
                          min(
                              array[(i - 1) % 2][j],           //Insert
                              min(array[i % 2][j - 1],         //Remove
                                  array[(i - 1) % 2][j - 1])); //Replace
      }
    }
  }
  return array[len % 2][len];
}

struct Test {
  string id;
  int bugs;
  double time;
  int** testCaseProfiles;
  int profileCount;
  int profileLength;
  bool selected;
  int* profileOrder;
};

int main(int argc, char** argv) {
  ifstream read(argv[1]);
  int maxTime;
  string temp;
  getline(read, temp, ':');
  read >> maxTime;
  getline(read, temp);
  getline(read, temp);
  getline(read, temp);

  ifstream suiteCounter(argv[1]);
  int count = -3;
  while (getline(suiteCounter, temp))
    ++count;
  suiteCounter.close();

  Test testSuites[count]; //A variable that will hold the data

  string line;
  int tsCount = 0;
  while (getline(read, line)) { //Reading lines
    istringstream readLine(line);
    string id;
    int bugs, time;
    readLine >> id >> bugs >> time;

    istringstream freqCounter(line); //Frequency array counter code block
    int profileCount = -3;
    while (freqCounter >> temp)
      ++profileCount;

    int profileLength = 1;
    for (int i = 0; i < temp.size(); ++i) { //Frequency length finder
      if (temp[i] == '-')
        ++profileLength;
    }

    testSuites[tsCount].testCaseProfiles = new int*[profileCount];
    for (int k = 0; k < profileCount; ++k) { //Reading profiles
      string profile;
      readLine >> profile;
      for (int i = 0; i < profile.size(); ++i) {
        if (profile[i] == '-')
          profile[i] = ' ';
      }
      istringstream readProfile(profile);
      int* freqProfile = new int[profileLength];
      testSuites[tsCount].testCaseProfiles[k] = freqProfile;
      for (int j = 0; j < profileLength; ++j) {
        readProfile >> freqProfile[j];
      }
    }
    testSuites[tsCount].id = id;
    testSuites[tsCount].bugs = bugs;
    testSuites[tsCount].time = time;
    testSuites[tsCount].profileCount = profileCount;
    testSuites[tsCount].profileLength = profileLength;
    testSuites[tsCount].selected = false;
    ++tsCount;
  }

  /*Tester code piece
  for (int i = 0; i < tsCount; ++i) {
    cout << testSuites[i].id << " " << testSuites[i].bugs << " "
         << testSuites[i].time << " ";
    for (int j = 0; j < testSuites[i].profileCount; ++j) {
      for (int k = 0; k < testSuites[i].profileLength; ++k) {
        cout << testSuites[i].testCaseProfiles[j][k] << ",";
      }
      cout << " ";
    }
    cout << endl;
  }*/

  double currentTime = 0; //To be matched to maxTime
  int currentBugs = 0; //To be maximized
  while (currentTime < maxTime) { //Greedy algorithm for suite selection
    int max = -1;
    Test* suite;
    for (int i = 0; i < tsCount; ++i) {
      if (!(testSuites[i].selected) && testSuites[i].bugs > max
          && currentTime + testSuites[i].time <= maxTime) {
        max = testSuites[i].bugs;
        suite = &testSuites[i];
      }
    }
    if (max == -1)
      break;

    currentTime += suite->time;
    currentBugs += suite->bugs;
    suite->selected = true;
  }

  for (int i = 0; i < tsCount; ++i) {
    if (testSuites[i].selected) { //Profile info modification
      for (int j = 0; j < testSuites[i].profileCount; ++j) {
        int* modifiedProfile = new int[testSuites[i].profileLength];
        int addCounter = 0;
        int searchedNumber = 0;
        while (addCounter < testSuites[i].profileLength) {
          for (int k = 0; k < testSuites[i].profileLength; ++k) {
            if (testSuites[i].testCaseProfiles[j][k] == searchedNumber) {
              modifiedProfile[addCounter++] = k;
            }
          }
          ++searchedNumber;
        }
        testSuites[i].testCaseProfiles[j] = modifiedProfile;
      }
    }
  }

  for (int i = 0; i < tsCount; ++i) { //Edit distance - ordering cases
    if (testSuites[i].selected) {
      int covered[testSuites[i].profileCount] = {0}; //Covered lines for each case
      for (int j = 0; j < testSuites[i].profileCount; ++j) {
        int coveredLines = 0;
        for (int k = 0; k < testSuites[i].profileLength; ++k) {
          if (testSuites[i].testCaseProfiles[j][k] != 0) {
            ++coveredLines;
          }
        }
        covered[j] = coveredLines;
      }

      int maxCoverIndex = -1;
      for (int j = 0; j < testSuites[i].profileCount; ++j) { //Selecting max-covering test case
        if (covered[j] > maxCoverIndex) {
          maxCoverIndex = j;
        }
      }

      int distance[testSuites[i].profileCount] = {0};
      distance[maxCoverIndex] = -1;
      for (int j = 0; j < testSuites[i].profileCount; ++j) { //Finding distances
        if (distance[j] == -1)
          continue;
        distance[j] = levenshtein(testSuites[i].testCaseProfiles[j],
                                  testSuites[i].testCaseProfiles[maxCoverIndex],
                                  testSuites[i].profileLength);
      }

      int* result = new int[testSuites[i].profileCount];
      testSuites[i].profileOrder = result;
      result[0] = maxCoverIndex;
      for (int j = 1; j < testSuites[i].profileCount; ++j) { //Ordering cases
        int max = -1;
        int selectedCase = 0;
        for (int k = 0; k < testSuites[i].profileCount; ++k) {
          if (distance[k] > max) {
            max = distance[k];
            selectedCase = k;
          }
        }
        result[j] = selectedCase;
        distance[selectedCase] = -1;
      }
    }
  }

  cout << "Total running time: " << currentTime << endl;
  cout << "Total bugs detected: " << currentBugs << endl;

  cout << endl;
  for (int i = 0; i < tsCount; ++i) { //Output
    if (testSuites[i].selected) {
      cout << testSuites[i].id << " ";
      for (int j = 0; j < testSuites[i].profileCount; ++j) {
        cout << testSuites[i].profileOrder[j] + 1 << " ";
      }
      cout << endl;
    }
  }

  read.close();
  return 0;
}