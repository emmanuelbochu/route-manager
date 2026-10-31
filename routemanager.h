#ifndef ROUTEMANAGER_H
#define ROUTEMANAGER_H

#include <vector>
#include "city.h"

class RouteManager
{
private:
    std::vector<City> listeVilles;
    std::vector<std::vector<double>> matriceTemps;
    std::vector<std::vector<int>> stockage;

public:
    RouteManager();

    bool loadCities(std::string fichier);
    bool loadRoads(std::string fichier);
    void runAlgo();
    double getTempo(int idA, int idB);
    std::vector<City> getVilles();
    std::vector<int> getChemin(int depart, int arrivee);
};

#endif // ROUTEMANAGER_H
