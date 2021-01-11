#include <iostream>
#include <mpi.h>
#include <chrono>
#include "block.h"
#include "blockchain.h"

using namespace std;

#define MASTER 0
#define GEN_DIFF 7
#define DIFF 8
#define SOL_TAG 1

void listenSolution(u_long &result, bool &aborted) {
    MPI_Recv(&result, 1, MPI_LONG, MPI_ANY_SOURCE, SOL_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    aborted = true;
}

int main(int argc, char** argv) {
    int id, nproc;
    
    int n;
    string data_to_hash;
    u_long result;
    
    thread abortThr;
    bool aborted;

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
    
    while (true) {
        block bl;
        if (id == MASTER) {
            bl = bc.getNextBlock();
            bl.data = "Block " + to_string(bl.index);
            bl.diff = DIFF;

            data_to_hash = bl.getDataToHash();
            n = data_to_hash.length();
        }
        
        MPI_Bcast(&n, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
        if (id != MASTER) data_to_hash.resize(n);

        MPI_Bcast((char *)data_to_hash.c_str(), n, MPI_CHAR, MASTER, MPI_COMM_WORLD);
        result = 0;
        
        aborted = false;
        abortThr = thread(listenSolution, ref(result), ref(aborted));

        auto start = chrono::steady_clock::now();
        block::mineBlockParalel(data_to_hash, DIFF, result, nonceStart);
        auto end = chrono::steady_clock::now();

        if (!aborted) {
            for (int i = 0; i < nproc; ++i)
                MPI_Send(&result, 1, MPI_LONG, i, SOL_TAG, MPI_COMM_WORLD);
        }
        abortThr.join();

        if (id == MASTER) {
            bl.nonce = result;
            bl.hash = bl.getHash();

            float t = chrono::duration_cast<chrono::milliseconds>(end-start).count();
            cout << bl.to_string() << " (" << t << " ms)\n";
            bc.chain.emplace_back(bl);
        }
    }

    MPI_Finalize();
    return 0;
}