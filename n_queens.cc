#include <iostream>
#include <thread>
#include <omp.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

void check(int queens[], int row, int column, int& solutions , std::ostringstream& globalText , int& nro_reinas) {
    for(int i = 0; i < row; i++) {
        if (queens[i] == column) return;
            
        if (abs(queens[i] - column) == (row- i)) return;
    }
    queens[row] = column;
    if(row == nro_reinas-1) {
        #pragma omp critical
            solutions++;
        std::ostringstream oss;
        for (int row = 0; row < nro_reinas; row++) {
            for (int column = 0; column < nro_reinas; column++) {
                if (queens[row] == column) {
                    oss << column+1;
                }
            }
            oss  << ' ';
        }
        #pragma omp critical
        globalText << solutions << " " << oss.str() << std::endl;
    }
    else {
        for(int i = 0; i < nro_reinas; i++) {
            check(queens, row + 1, i,solutions,globalText,nro_reinas);
        }
    }
}

void n_queens(int& solutions , std::ostringstream& globalText, int nro_reinas) {
    #pragma omp parallel 
    #pragma omp single
    {
        for(int i = 0; i < nro_reinas; i++) {
            #pragma omp task
            { 
                check(new int[nro_reinas], 0, i,solutions,globalText,nro_reinas);
            }
        }
    }
}

int main( int argc , char** argv ){
    if(argc < 5){
        return -1;
    }
    int nro_reinas = atoi(argv[4]);
    std::string type = argv[2];
    int solutions = 0;
    std::ostringstream globalText;
    n_queens(solutions,globalText,nro_reinas);
    std::fstream archive;
    archive.open("solutions.txt",std::ios::out);
    archive << "#Solutions for " << nro_reinas << " queens\n";
    archive << solutions << std::endl;
    archive << globalText.str();
    archive.close();
    //std::cout << "Tamanho: " << nro_reinas << std::endl; 
    //std::cout << "Total soluciones: " << solutions << std::endl; 
    if(type == "find"){
        std::fstream archivedot;
        archivedot.open("solution.dot",std::ios::out);
        std::ifstream archive("solutions.txt");
        std::string line;
        getline(archive,line);
        getline(archive,line);
        getline(archive,line);
        archivedot << "Digraph D {\n";
        archivedot << "\t node[shape=plaintext]\n";
        archivedot << "\t some_node [ \n";
        archivedot << "\t\t label=< \n";
        archivedot << "\t\t\t <table border=\"0\" cellborder=\"1\" cellspacing=\"0\">";
        for(int i = 0 ; i < nro_reinas ; i++ ){
            archivedot << "<tr> ";
            for( int j = 2 ; j < line.size() ; j+=2 ){
                if( j == ((int)line[i*2]-48)-1 ){
                    archivedot << "<td>&#9813;</td> ";
                }else
                    archivedot << "<td> </td> ";
            }
            archivedot << " </tr>\n";
        }
        archivedot << "</table>>];}";
    }

    return 0;
}
