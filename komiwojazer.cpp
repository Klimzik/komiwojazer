#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>
#include <limits>

const int nodeN = 40;
std::vector<std::pair<int, int>> nodes(nodeN);
std::vector<int> saPath;
std::vector<double> saDataDist;
double saBestDist;
std::vector<int> saBestPath;
double saTempStart = 10000;
double saTempAlpha = 0.976;
double saTemp;

int loopN = 400;

// Function declarations
std::vector<int> RandPath();
double PathDist(const std::vector<int>& path);
void PathMove(std::vector<int>& path, const std::pair<int, int>& move);
std::pair<int, int> RandAB();
void SaStep(std::vector<int>& path, double temp);
void Run();
void NodeRand();

int main() {
    NodeRand();
    Run();
    std::cout << "Best path distance: " << saBestDist << std::endl;
    return 0;
}

void Run() {
    auto startPath = RandPath();
    double startDist = PathDist(startPath);
    saPath = startPath;
    saDataDist.clear();
    saDataDist.push_back(startDist);
    saBestDist = startDist;
    saBestPath = startPath;

    saTemp = saTempStart;

    for (int i = 0; i < loopN; ++i) {
        SaStep(saPath, saTemp);
        saTemp = saTemp * saTempAlpha;
    }
    saPath = saBestPath;
    saDataDist.push_back(saBestDist);
}

void SaStep(std::vector<int>& path, double temp) {
    double c0 = PathDist(path);
    double praw = 0, akce = 0;
    for (int i = 0; i < 100; i++) {
        auto move = RandAB();
        PathMove(path, move);
        double c1 = PathDist(path);
        if (c1 > c0) {
            praw = std::exp((c0 - c1) / temp);
            akce = (static_cast<double>(rand()) / RAND_MAX < praw);
        }
        if (c1 <= c0 || akce) {
            c0 = c1;
        }
        else {
            PathMove(path, move);
        }
        if (saBestDist > c0) {
            saBestDist = c0;
            saBestPath = path;
        }
    }
    saDataDist.push_back(c0);
}

std::pair<int, int> RandAB() {
    int a = rand() % nodeN;
    int b = rand() % (nodeN - 1);
    if (b >= a) b++;
    if (a > b) std::swap(a, b);
    return { a, b };
}

void PathMove(std::vector<int>& path, const std::pair<int, int>& move) {
    int a = move.first, b = move.second;
    while (a < b) {
        std::swap(path[a], path[b]);
        a++;
        b--;
    }
}

double PathDist(const std::vector<int>& path) {
    double dist = 0;
    for (size_t i = 0; i < path.size(); i++) {
        auto& n0 = nodes[path[i]];
        auto& n1 = nodes[path[(i + 1) % path.size()]];
        dist += std::sqrt((n0.first - n1.first) * (n0.first - n1.first) + (n0.second - n1.second) * (n0.second - n1.second));
    }
    return dist;
}

void NodeRand() {
    nodes.clear();
    saPath.clear();
    saBestPath.clear();
    saDataDist.clear();
    for (int i = 0; i < nodeN; i++) {
        nodes.push_back({ rand() % 400, rand() % 200 });
    }
}

std::vector<int> RandPath() {
    std::vector<int> path(nodeN);
    std::iota(path.begin(), path.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(path.begin(), path.end(), g);
    return path;
}
