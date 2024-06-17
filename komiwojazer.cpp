#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>
#include <limits>
#include <fstream>

// Constants
const int NODE_COUNT = 40;
const int ITERATIONS = 400;
const double INITIAL_TEMPERATURE = 10000.0;
const double TEMPERATURE_DECREASE_RATE = 0.976;

// Class declarations
class Node {
public:
    int x, y;
    Node(int x, int y) : x(x), y(y) {}
};

class Path {
public:
    std::vector<int> nodes;

    Path(int size) : nodes(size) {
        std::iota(nodes.begin(), nodes.end(), 0);
        std::random_device rd;
        std::mt19937 g(rd());
        //for (int i = nodes.size() - 1; i > 0; --i) {
        //    int j = rand() % (i + 1);
        //    std::swap(nodes[i], nodes[j]);
        //}
        std::shuffle(nodes.begin(), nodes.end(), g);
    }

    void swapNodes(int a, int b) {
        std::swap(nodes[a], nodes[b]);
    }

    double calculateDistance(const std::vector<Node>& nodeCoordinates) const {
        double distance = 0.0;
        for (size_t i = 0; i < nodes.size(); i++) {
            auto& n0 = nodeCoordinates[nodes[i]];
            auto& n1 = nodeCoordinates[nodes[(i + 1) % nodes.size()]];
            distance += std::sqrt((n0.x - n1.x) * (n0.x - n1.x) + (n0.y - n1.y) * (n0.y - n1.y));
        }
        return distance;
    }

    void reverseSubPath(int a, int b) {
        while (a < b) {
            swapNodes(a, b);
            a++;
            b--;
        }
    }
};

class SimulatedAnnealing {
public:
    std::vector<Node> nodeCoordinates;
    Path bestPath;
    double bestDistance;
    double temperature;
    double temperatureDecreaseRate;

    std::vector<double> distances;


    SimulatedAnnealing(int nodeCount, double initialTemperature, double temperatureDecreaseRate)
        : bestPath(nodeCount), temperature(initialTemperature), temperatureDecreaseRate(temperatureDecreaseRate) {
        for (int i = 0; i < nodeCount; i++) {
            nodeCoordinates.emplace_back(rand() % 400, rand() % 200);
        }
        bestDistance = bestPath.calculateDistance(nodeCoordinates);
        //distances.push_back(bestDistance);

    }

    void run(int iterations) {
        Path currentPath = bestPath;
        double currentDistance = bestDistance;

        for (int i = 0; i < iterations; ++i) {
            for (int j = 0; j < 100; ++j) {
                auto move = getRandomSwapIndices();
                currentPath.reverseSubPath(move.first, move.second);
                double newDistance = currentPath.calculateDistance(nodeCoordinates);

                if (newDistance < currentDistance || acceptMove(newDistance, currentDistance)) {
                    currentDistance = newDistance;
                    if (newDistance < bestDistance) {
                        bestDistance = newDistance;
                        distances.push_back(bestDistance);
                        bestPath = currentPath;
                    }
                }
                else {
                    currentPath.reverseSubPath(move.first, move.second);
                }
            }
            temperature *= temperatureDecreaseRate;
        }
    }
    void saveDistancesToFile(const std::string& filename) const {
        std::ofstream file(filename);
        for (size_t i = 0; i < distances.size(); ++i) {
            file << i << "," << distances[i] << "\n";
        }
        file.close();
    }

    void printBestDistance() const {
        std::cout << "Best path distance: " << bestDistance << std::endl;
    }

private:
    std::pair<int, int> getRandomSwapIndices() const {
        int a = rand() % NODE_COUNT;
        int b = rand() % (NODE_COUNT - 1);
        if (b >= a) b++;
        return (a > b) ? std::make_pair(b, a) : std::make_pair(a, b);
    }

    bool acceptMove(double newDistance, double currentDistance) const {
        double probability = std::exp((currentDistance - newDistance) / temperature);
        return static_cast<double>(rand()) / RAND_MAX < probability;
    }
};

int main() {
    SimulatedAnnealing sa(NODE_COUNT, INITIAL_TEMPERATURE, TEMPERATURE_DECREASE_RATE);
    sa.run(ITERATIONS);
    sa.printBestDistance();
    sa.saveDistancesToFile("distances.csv");
    return 0;
}