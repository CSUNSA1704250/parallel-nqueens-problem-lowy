#include <iostream>
#include <vector>
#include <pthread.h>
#include <thread>
#include <omp.h>
#include <chrono>
#include <fstream>
#include <string>
#include <typeinfo>

template <typename T>
T v_abs( T x_num ){
    return x_num >= 0 ? x_num : -x_num;
}

void print( std::vector<int>& le_vector ){
    std::cout << "[ ";
    for( auto x : le_vector )
        std::cout << x << ' ';
    std::cout << "]\n";
}

bool check( std::vector<int> vector_reinas , int column ){
    for( int i = 0 ; i < column ; i++ )
        if( vector_reinas[column] == vector_reinas[i] || (v_abs( vector_reinas[column] - vector_reinas[i] ) == v_abs( column - i ) ) )
            return false;
    return true;
}

void n_queens( std::vector<int> vector_reinas , int actual_column , int queens_size , std::vector<std::string>& reinas_solution ){
    if(actual_column == queens_size){
        std::string res;
        for(auto x : vector_reinas){
            res.append(std::to_string(x+1));
            res.append(" ");
        }
        reinas_solution.push_back(res);
    }else{
        #pragma omp parallel for private(i)
        for( int i = 0 ; i < queens_size ; i++ ){
            vector_reinas[actual_column] = i;
            #pragma omp task
            {
                if( check(vector_reinas , actual_column) ){
                    n_queens(vector_reinas , actual_column+1 , queens_size , reinas_solution);
                }
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
    //std::cout << type << std::endl;
    std::vector<int> vector_reinas(nro_reinas,-1);
    std::vector<std::string> reinas_solutions;
    std::fstream archive;
    //auto start = std::chrono::steady_clock::now();
    n_queens(vector_reinas , 0 , nro_reinas , reinas_solutions );
    //auto finish = std::chrono::steady_clock::now();
    //std::cout << "Tiempo transcurrido con " << nro_reinas << " : " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << std::endl;
    //return 0;
    if(type == "all"){
        archive.open("solutions.txt",std::ios::out);
        archive << "#Solutions for " << nro_reinas << " queens\n";
        archive << reinas_solutions.size() << "\n";
        int line_couter = 1;
        for(auto x : reinas_solutions){
            archive << line_couter << " " << x << '\n';
            line_couter++;
        }
    }
        
    if(type == "find"){
        archive.open("solution.dot",std::ios::out);
        std::string random_solution = reinas_solutions[rand()%(reinas_solutions.size()-1)];
        archive << "Digraph D {\n";
        archive << "\t node[shape=plaintext]\n";
        archive << "\t some_node [ \n";
        archive << "\t\t label=< \n";
        archive << "\t\t\t <table border=\"0\" cellborder=\"1\" cellspacing=\"0\">";
        for(int i = 0 ; i < nro_reinas ; i++ ){
            archive << "<tr> ";
            for( int j = 0 ; j < nro_reinas ; j++ ){
                if( j == ((int)random_solution[i*2]-48)-1 ){
                    
                    archive << "<td>&#9813;</td> ";
                }else
                    archive << "<td> </td> ";
            }
            archive << " </tr>\n";
        }
        archive << "</table>>];}";
    }

    return 0;
}
