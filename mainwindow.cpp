#include "mainwindow.h"
#include <QVBoxLayout>
#include <QWidget>

// Constructeur de la fenêtre principale
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // On crée le manager et on charge les fichiers de données (embarqués dans l'exécutable via Ressource.qrc)
    manager = new RouteManager();
    manager->loadCities(":/villes.csv");
    manager->loadRoads(":/temps.csv");

    // Création des composants de l'interface graphique
    comboDepart = new QComboBox(this);
    comboArrivee = new QComboBox(this);
    boutonCalculer = new QPushButton("Calculer l'itinéraire", this);
    labelResultat = new QLabel("Choisissez vos villes", this);
    tableauDistances = new QTableWidget(this); // On crée la grille pour les distances

    // On lance l'algo (Floyd-Warshall) pour calculer tous les chemins à l'avance
    manager->runAlgo();

    // On remplit les listes déroulantes avec le nom de chaque ville
    // On cache l'ID numérique en deuxième paramètre pour le récupérer facilement après
    for (const City& laVille : manager->getVilles()) {
        comboDepart->addItem(QString::fromStdString(laVille.nom), laVille.id);
        comboArrivee->addItem(QString::fromStdString(laVille.nom), laVille.id);
    }

    // Gestion du layout pour organiser l'affichage proprement
    QWidget* zoneCentrale = new QWidget(this);
    setCentralWidget(zoneCentrale);

    QVBoxLayout* disposition = new QVBoxLayout(zoneCentrale);

    disposition->addWidget(comboDepart);
    disposition->addWidget(comboArrivee);
    disposition->addWidget(boutonCalculer);
    disposition->addWidget(labelResultat);
    disposition->addWidget(tableauDistances); // On ajoute le tableau en bas de l'interface

    // Connexion du bouton : quand on clique, ça appelle la fonction auClicCalculer
    connect(boutonCalculer, &QPushButton::clicked, this, &MainWindow::auClicCalculer);
}

// Destructeur : on nettoie la mémoire
MainWindow::~MainWindow() {
    delete manager;
}

// Fonction déclenchée par le bouton de calcul
void MainWindow::auClicCalculer()
{
    // On récupère les IDs des deux villes sélectionnées dans l'interface
    int idDepart = comboDepart->currentData().toInt();
    int idArrivee = comboArrivee->currentData().toInt();

    // On demande au manager la liste des IDs des villes à traverser
    std::vector<int> chemin = manager->getChemin(idDepart, idArrivee);

    // Variable pour construire le texte final qu'on affichera dans l'interface
    QString resultat = "Itinéraire : ";

    // Liste de textes pour stocker les noms des villes de l'itinéraire (pour les en-têtes du tableau)
    QStringList listeNoms;

    // On parcourt la liste des IDs du chemin pour retrouver le nom des villes
    for (size_t i = 0; i < chemin.size(); i++) {
        int idVille = chemin[i];

        // On cherche l'ID dans le tableau global des villes pour choper le nom correspondant
        for (const City& laVille : manager->getVilles()) {
            if (laVille.id == idVille) {
                resultat += QString::fromStdString(laVille.nom);

                // On ajoute le nom de la ville actuelle dans notre liste pour le tableau
                listeNoms << QString::fromStdString(laVille.nom);

                // On met une flèche entre les villes, sauf après la dernière
                if (i < chemin.size() - 1) {
                    resultat += " -> ";
                }
            }
        }
    }

    // On récupère la valeur du temps total du trajet
    double tempsTotal = manager->getTempo(idDepart, idArrivee);

    // On ajoute le temps à la ligne en-dessous du texte
    resultat += "\nTemps de trajet : " + QString::number(tempsTotal) + " min";

    // On dimensionne le tableau pour qu'il ait autant de lignes/colonnes que de villes dans le trajet
    tableauDistances->setRowCount(chemin.size());
    tableauDistances->setColumnCount(chemin.size());

    // On applique la liste des noms sur les en-têtes (en haut et à gauche)
    tableauDistances->setHorizontalHeaderLabels(listeNoms);
    tableauDistances->setVerticalHeaderLabels(listeNoms);

    // Double boucle pour remplir chaque case de la grille (lignes et colonnes)
    for (size_t l = 0; l < chemin.size(); l++) {
        for (size_t c = 0; c < chemin.size(); c++) {
            // On récupère les deux IDs correspondants à la case actuelle
            int idA = chemin[l];
            int idB = chemin[c];

            // On demande le temps optimal entre ces deux villes au manager
            double temps = manager->getTempo(idA, idB);

            // On convertit le temps en texte
            QString texteTemps = QString::number(temps);

            // On injecte le texte dans la bonne case de la grille Qt
            tableauDistances->setItem(static_cast<int>(l), static_cast<int>(c), new QTableWidgetItem(texteTemps));
        }
    }

    // Mise à jour de l'affichage à l'écran
    labelResultat->setText(resultat);
}