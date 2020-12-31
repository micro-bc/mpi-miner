#include <iostream>
#include <mpi.h>
#include <thread>
#include <string>
#include "block.h"
#include "blockchain.h"
#include "util.h"

using namespace std;

#define MASTER 0

int main(int argc, char** argv) {
    int id, nproc;
    srand(time(0));
    setbuf(stdout, NULL);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    uint proc_count = thread::hardware_concurrency();

    if (nproc == 1) {
        cout << "MASTER is only a producer\n";
        // return 1;
    } else return 1;

    if (id == MASTER) {
        blockchain bc;

        block genesis = bc.chain[0];

        cout << genesis.to_string() << '\n';
    }
    else {

    }

    MPI_Finalize();
    return 0;
}