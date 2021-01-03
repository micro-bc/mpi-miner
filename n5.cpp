#include <iostream>
#include <mpi.h>
#include <chrono>
#include "block.h"
#include "blockchain.h"

using namespace std;

#define MASTER 0

int main(int argc, char** argv) {
    int id, nproc;
    srand(time(0));
    setbuf(stdout, NULL);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    if (nproc == 1) {
        cout << "MASTER is only a producer\n";
        // return 1;
    } else return 1;

    if (id == MASTER) {
        auto start = chrono::steady_clock::now();
        blockchain bc;
        auto end = chrono::steady_clock::now();

        block genesis = bc.chain[0];

        cout << genesis.to_string() << '\n';

        float t = chrono::duration_cast<chrono::milliseconds>(end-start).count();
        cout << t << " ms\n"
             << genesis.nonce / t << " hash/ms\n"
             << genesis.nonce / t/1000 << " Mhash/s\n";
    }
    else {

    }

    MPI_Finalize();
    return 0;
}