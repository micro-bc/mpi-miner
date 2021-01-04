#include <iostream>
#include <mpi.h>
#include <chrono>
#include "block.h"
#include "blockchain.h"

using namespace std;

#define MASTER 0
#define GEN_DIFF 6
#define DIFF 8
#define SOL_TAG 1

void listenSolution(u_long &nonce, bool &aborted) {
    MPI_Recv(&nonce, 1, MPI_LONG, MPI_ANY_SOURCE, SOL_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    aborted = true;
}

int main(int argc, char** argv) {
    int id, nproc;
    srand(time(0));
    setbuf(stdout, NULL);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    if (id == MASTER)
        cout << "Comm size: " << nproc << '\n';

    u_long nonceStart = UINT64_MAX / nproc * id;

    // Chain init
    blockchain bc;
    if (id == MASTER) {
        auto start = chrono::steady_clock::now();
        bc.init(GEN_DIFF);
        auto end = chrono::steady_clock::now();

        block genesis = bc.chain[0];

        cout << genesis.to_string() << '\n';

        float t = chrono::duration_cast<chrono::milliseconds>(end-start).count();
        cout << t << " ms\n"
             << genesis.nonce / t << " hash/ms\n"
             << genesis.nonce / t/1000 << " Mhash/s\n";
    }
    
    char *buffer;
    thread abortThr;
    bool aborted;
    while (true) {
        block bl;
        if (id == MASTER) {
            bl = bc.getNextBlock();
            bl.data = "Block " + to_string(bl.index);
        }
        
        MPI_Bcast(&bl.index, 1, MPI_LONG, MASTER, MPI_COMM_WORLD);
        MPI_Bcast(&bl.prev_hash, 1, MPI_LONG, MASTER, MPI_COMM_WORLD);
        MPI_Bcast(&bl.timestamp, 1, MPI_LONG, MASTER, MPI_COMM_WORLD);
        int n = bl.data.length() + 1;
        MPI_Bcast(&n, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
        if (id == MASTER) buffer = (char*)bl.data.c_str();
        else buffer = new char[n];
        MPI_Bcast(buffer, n, MPI_CHAR, MASTER, MPI_COMM_WORLD);
        if (id != MASTER) {
            bl.data = string(buffer);
            delete[] buffer;
        }
        
        aborted = false;
        abortThr = thread(listenSolution, ref(bl.nonce), ref(aborted));

        auto start = chrono::steady_clock::now();
        bl.mineBlockParalel(DIFF, nonceStart);
        auto end = chrono::steady_clock::now();

        if (!aborted) {
            for (int i = 0; i < nproc; ++i)
                MPI_Send(&bl.nonce, 1, MPI_LONG, i, SOL_TAG, MPI_COMM_WORLD);
        }

        abortThr.join();

        if (id == MASTER) {
            float t = chrono::duration_cast<chrono::milliseconds>(end-start).count();
            cout << bl.to_string() << '\n' << t << " ms\n";
        }

        break;
    }

    MPI_Finalize();
    return 0;
}