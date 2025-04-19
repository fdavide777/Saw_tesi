#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <unordered_map>
#include <cmath>    // sqrt
#include <numeric>
#include <array>
#include<unordered_set>

using namespace std::literals::chrono_literals;
using namespace std;

random_device rd; //creo un generatore di numeri casuali
static mt19937 gen(rd()); //inizializzo il generatore
uniform_int_distribution<> distrib3(0, 3); //distribuzione uniforme tra 0 e 3
uniform_int_distribution<> distrib2(0, 2); //distribuzione uniforme tra 0 e 2
uniform_int_distribution<> distrib1(0, 1); //distribuzione uniforme tra 0 e 1
uniform_int_distribution<> distrib7(1, 7); //distribuzione uniforme tra 0 e 7

void printVec(vector<vector<int>>& A){
    for(auto& riga : A){
        for(auto& elem : riga){
            cout << elem << ", ";
        }
        cout << endl;
    }  
}

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


void spostamento(int& x, int& y, int direzione){
    switch(direzione){
        case 0: x++; break;//destra
        case 1: y++; break; // su
        case 2: x--; break; // sinistra
        case 3: y--; break; // giù
    }
    //return {x, y}; //restituisco la nuova posizione
}


vector<vector<int>> simpleSaw(const int simple_N){
    unordered_set<pair<int, int>, pair_hash> visite;

    int x = 0;
    int y = 0;
    vector<vector<int>> cammino_ss = {{x,y}};
    visite.insert({x,y});

    static const int direzione_iniziale [4] = {0,1,2,3}; // 4 direzioni possibili all'inizio
    int indice_in = distrib3(gen);//indice random iniziale, poi devo scegliere solo tra 3 indici
    spostamento(x, y, direzione_iniziale[indice_in]);
    visite.insert({x,y});
    cammino_ss.push_back({x,y});

    int counter = 1; //contatore per il numero di passi
    int direzione_corrente = indice_in;//indice per la direzione, deve avere solo 3 scelte...

    while (counter < simple_N){ 
        static const int offset [3] = {-1, 0, 1}; //per evitare il reversal
        int pick = distrib2(gen);//pick casuale tra 0 e 2
        
        int direzione = (direzione_corrente + offset[pick] + 4) % 4; //scelgo una direzione casuale tra le tre possibili
        direzione_corrente = direzione; //aggiorno l'indice e quindi la lista di possibili prossimi indici 
        
        spostamento(x, y, direzione);

        if (visite.find({x, y}) == visite.end()){//se la posizione non è occupata{
            visite.insert({x,y});;//se la posizione non è occupata, la occupo
            counter++;
            cammino_ss.push_back({x,y});
        }
        else//posizione occupata
        {   visite.clear();
            cammino_ss.clear();
            x = 0;
            y = 0;
            cammino_ss.push_back({x,y});
            visite.insert({x,y});
            int indice_in = distrib3(gen);//indice random iniziale, poi devo scegliere solo tra 3 indici
            spostamento(x, y, direzione_iniziale[indice_in]);
            cammino_ss.push_back({x,y});
            visite.insert({x,y});
            counter = 1;
            direzione_corrente = indice_in;
        } 
    }
    return cammino_ss;
}


vector<vector<int>> dimerizzazione(int dim_N){
    int x;
    int y;
    
    unordered_set<pair<int, int>, pair_hash> visite;
    
    vector<vector<int>> sum_vec;
    vector<vector<int>> new_vec;

    int N0 = 10;
    if(dim_N <= N0){
        new_vec=simpleSaw(dim_N);
        return new_vec;
    }
    else{
        bool output = false;
        while(!output){
            int n1 = dim_N/2;
            int n2 = dim_N - n1;

            bool buono = true;

            vector<vector<int>> v1 = dimerizzazione(n1);
            vector<vector<int>> v2 = dimerizzazione(n2);

            for (int i = 0; i < v1.size(); i++){//attacco prima v2 a v1, quindi mappo v1
                x = v1[i][0];
                y = v1[i][1];
                visite.insert({x,y});
            }

            sum_vec = v1;
            for(int ii = 1; ii < v2.size() && buono; ii++){//va saltato il primo elemento (0,0) perchè è sicuramente occupato dalla fine di v1 una volta shiftato
                int new_x = v2[ii][0] + v1.back()[0];//v2 va shiftato per collegarli correttamente
                int new_y = v2[ii][1] + v1.back()[1];
                if(visite.find({new_x, new_y}) == visite.end()){
                    sum_vec.push_back({new_x, new_y});
                }
                else{
                    buono = false;
                }
            }

            if(!buono){//se non va bene, provo a fare il contrario
                visite.clear();
                for (int i = 0; i < v2.size(); i++){
                    x = v2[i][0];
                    y = v2[i][1];
                    visite.insert({x,y});
                }
                
                sum_vec = v2;
                buono = true;

                for(int ii=1; ii < v1.size() && buono; ii++){//va saltato il primo elemento (0,0) perchè è sicuramente occupato dalla fine di v1 una volta shiftato
                    int new_x = v1[ii][0] + v2.back()[0];
                    int new_y = v1[ii][1] + v2.back()[1];
                    if(visite.find({new_x, new_y}) == visite.end()){
                        sum_vec.push_back({new_x, new_y});
                    }
                    else{
                        visite.clear();
                        buono = false;
                    }
                }
            }
            if(buono){
                output = true;
            }
        }
        new_vec = sum_vec;
    }    
return new_vec;
}

template <typename T>
void exportMatrix(const vector<vector<T>> & matrice, const string & nomefile){
    ofstream file(nomefile);//creo il file di output
    if (file.is_open()){
        file << "media/ " << "std/ " << "passi/ " << endl;
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
    unordered_map<int, vector<int>> mappa;
    for (int ii = 5; ii <= passo_max; ii++){//fino al passo massimo
        auto start_time = chrono::high_resolution_clock::now();
        cout << "passo " << ii << endl;
        for(int jj = 0; jj < misure; jj++){// ripetizioni volte per ogni passo
            vector<vector<int>> vettore = dimerizzazione(ii);
            int dx = vettore[0][0] - vettore[vettore.size()-1][0];
            int dy = vettore[0][1] - vettore[vettore.size()-1][1];
            int r2 = dx*dx + dy*dy;
            mappa[ii].push_back(r2);
        }
        auto time = chrono::high_resolution_clock::now() - start_time;    
        cout << "Generato in " << time/chrono::seconds(1) << " secondi\n";
    }
    for (auto& kk : mappa){
        int passo = kk.first;
        vector<int> r2 = mappa[passo];

        double media = mean(r2);
        double dev = standardDev(r2);

        stats.push_back({media, dev, static_cast<double>(passo)}); //aggiungo alla matrice le statistiche
    }
    return stats;
}
   


int main(){
    const int passo_max = 59; //numero massimo di passi
    const int misure = 10000; //numero di ripetizioni per ogni iterazione
    const string percorso_stat = "C:/Users/Prova/Desktop/UNIVERSITa/TESI/codice/Dimerizzazione.txt";

    auto start_time = chrono::high_resolution_clock::now();
    cout << endl << endl;
    vector<vector<double>> stats = raccoltaDati(misure, passo_max); //raccolgo le statistiche
    exportMatrix(stats, percorso_stat); //esporto le statistiche in un file csv

    auto time = chrono::high_resolution_clock::now() - start_time;    
    cout << "Tempo di esecuzione " << time/chrono::seconds(1) << "secondi.\n";
    
    return 0;
}

