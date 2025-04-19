//circa 400 sec per 1e6 cammini da 50

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <unordered_map>
#include <cmath>    // sqrt
#include <numeric>
#include<unordered_set>

using namespace std::literals::chrono_literals;
using namespace std;

random_device rd; //creo un generatore di numeri casuali
static mt19937 gen(rd()); //inizializzo il generatore
uniform_int_distribution<> distrib3(0, 3); //distribuzione uniforme tra 0 e 3
uniform_int_distribution<> distrib2(0, 2); //distribuzione uniforme tra 0 e 2

void printVec(vector<vector<int>>& A){
    for(auto& riga : A){
        for(auto& elem : riga){
            cout << elem << ", ";
        }
        cout << endl;
    }  
}

void spostamento(int& x, int& y, int direzione){
    switch(direzione){
        case 0: x++; break;//destra
        case 1: y++; break; // su
        case 2: x--; break; // sinistra
        case 3: y--; break; // giù
    }
    //return {x, y}; //restituisco la nuova posizione
}

struct risultati{
    int r2;
    int tries;
}; //struttura per memorizzare i risultati

struct pair_hash {
    template <class T1, class T2>
    size_t operator() (const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        
        // Combina gli hash
        return hash1
               ^ (hash2 + 0x9e3779b9 + (hash1 << 6)
                  + (hash1 >> 2)); 
    }
};

risultati simpleSaw(const int Nn){
    unordered_set<pair<int, int>, pair_hash> visite;
    //vector<vector<int>> storico;

    risultati risultati;

    static const int direzione_iniziale [4] ={0,1,2,3}; // 4 direzioni possibili all'inizio
    //vector<int> posizione = {0,0};//inizializzo la posizione
    int x = 0;
    int y = 0;
    visite.insert({x,y}); //la posizione iniziale è occupata

    int indice_in = distrib3(gen);//indice random iniziale, poi devo scegliere solo tra 3 indici
    spostamento(x, y, direzione_iniziale[indice_in]);
    visite.insert({x,y}); 
    //storico.push_back({x,y}); 

    int counter = 1; //contatore per il numero di passi
    int tries = 1; 

    int kk = indice_in;//indice per la direzione, deve avere solo 3 scelte...

    while (counter < Nn){ 
        static const int offset [3] = {-1, 0, 1}; //per evitare il reversal
        int pick = distrib2(gen);//pick casuale tra 0 e 2
        
        int direzione = (kk + offset[pick] + 4) % 4; //scelgo una direzione casuale tra le tre possibili
        kk = direzione; //aggiorno l'indice e quindi la lista di possibili prossimi indici 
        
        spostamento(x, y, direzione);

        if (visite.find({x,y}) == visite.end()){//se la posizione non è occupata{
            visite.insert({x,y});;//se la posizione non è occupata, la occupo
            //storico.push_back({x,y}); 
            counter++;
        }
        else//posizione occupata
        {   visite.clear();
           // storico.clear(); 
            x = 0;
            y = 0;
            visite.insert({x,y}); //la posizione iniziale è occupata
            //storico.push_back({x,y}); 
            int indice_in = distrib3(gen);//indice random iniziale, poi devo scegliere solo tra 3 indici
            spostamento(x, y, direzione_iniziale[indice_in]);
            visite.insert({x,y});
            //storico.push_back({x,y}); 
            counter = 1;
            kk = indice_in;
            tries += 1;
        } 
    }
    //printVec(storico);
    int r2 = x*x + y*y;
    risultati.r2 = r2;
    risultati.tries = tries;
    return risultati;
}

template <typename T>
void exportMatrix(const vector<vector<T>> & matrice, const string & nomefile){
    ofstream file(nomefile);//creo il file di output
    if (file.is_open()){
        file << "media/ " << "std/ " << "tries/ " << "std tries/ " << "passi/ " << endl;
        for (auto riga : matrice){
            for (int i = 0; i < riga.size()-1; i++){
                file << riga[i] << " , "; //scrivo la matrice nel file
            }
        file << riga.back(); //scrivo l'ultimo elemento senza la virgola
        file << endl;
        }
        file.close(); //chiudo il file
    }
    else{
        cout << "Impossibile aprire il file" << endl;
    }
}

template<typename T> double mean(vector<T>& vettore){
    double sum = accumulate(vettore.begin(), vettore.end(), 0.0);
    double media = sum / vettore.size();
    return media;
}

template<typename T> double standardDev (vector<T>& vettore){
    double pre_dev = 0.0;
    double media = mean(vettore);
    for (int ii : vettore){
        pre_dev += (ii-media)*(ii-media);
    }
    double dev = sqrt(pre_dev/((vettore.size()-1)*vettore.size()));
    return dev;
}

vector<vector<double>> raccoltaDati(int misure, int passo_max){
    vector<vector<double>> stats;
    unordered_map<int, pair<vector<int>, vector<int>>> mappa;
    for (int ii = 5; ii <= passo_max; ii++){//fino al passo massimo
        auto start_time = chrono::high_resolution_clock::now();
        cout << "passo " << ii << endl;
        for(int jj = 0; jj < misure; jj++){// ripetizioni volte per ogni passo
            risultati results = simpleSaw(ii);
            mappa[ii].first.push_back(results.r2);
            mappa[ii].second.push_back(results.tries);
        }
        auto time = chrono::high_resolution_clock::now() - start_time;    
        cout << "Generato in " << time/chrono::seconds(1) << " secondi\n";
    }
    for (auto& kk : mappa){
        int passo = kk.first;
        vector<int> r2 = mappa[passo].first;
        vector<int> tries_tot = mappa[passo].second;

        double media = mean(r2);
        double media_tries = mean(tries_tot);

        double dev = standardDev(r2);
        double dev_eff = standardDev(tries_tot);

        stats.push_back({media, dev, media_tries, dev_eff, static_cast<double>(passo)}); //aggiungo alla matrice le statistiche
    }
    return stats;
}
   


int main(){
    const int passo_max = 50; //numero massimo di passi
    const int misure = 1000000; //numero di ripetizioni per ogni iterazione
    const string percorso_stat = "C:/Users/Prova/Desktop/UNIVERSITa/TESI/codice/SimpleSampling.txt";

    auto start_time = chrono::high_resolution_clock::now();
    cout << endl << endl;
    vector<vector<double>> stats = raccoltaDati(misure, passo_max); //raccolgo le statistiche
    exportMatrix(stats, percorso_stat); //esporto le statistiche in un file csv
    //risultati results = simpleSaw(10);
    auto time = chrono::high_resolution_clock::now() - start_time;    
    cout << "Tempo di esecuzione " << time/chrono::milliseconds(1) << " secondi.\n";
    
    return 0;
}

