// Copyright 2023 Chubenko Andrey

#include "../../../modules/task_3/chubenko_a_dijkstra_omp/dijkstra.h"


std::vector<int> get_dists_dijkstra(std::vector<int> graph,
    int size, int start) {
    std::vector<int> dists(size, INT_MAX);
    std::vector<bool> used(size, false);
    dists[start] = 0;
    for (int i = 0; i < size; i++) {
        int min_dist = INT_MAX;
        int min_vertex = -1;
        for (int j = 0; j < size; j++) {
            if (!used[j] && dists[j] < min_dist) {
                min_dist = dists[j];
                min_vertex = j;
            }
        }
        if (min_dist == INT_MAX) {
            break;
        }
        used[min_vertex] = true;
        for (int j = 0; j < size; j++) {
            if (graph[min_vertex * size + j] != 0) {
                int new_dist = dists[min_vertex] + graph[min_vertex * size + j];
                if (new_dist < dists[j]) {
                    dists[j] = new_dist;
                }
            }
        }
    }
    return dists;
}
/** TBB **/
std::vector<int> get_dists_dijkstra_parallel(std::vector<int> graph,
                int size, int start) {
    std::vector<int> dists(size, INT_MAX);
    std::vector<bool> used(size, false);
    dists[start] = 0;

    
    for (int i = 0; i < size; i++) {
        int min_dist = INT_MAX;
        int min_vertex = -1;

        tbb::parallel_for(tbb::blocked_range<int>(0, size), [&](const tbb::blocked_range<int>& r){
            for (int j = 0; j < size; j++) {
                tbb::spin_mutex::scoped_lock lock(mtx);
                if (!used[j] && dists[j] < min_dist) {
                    min_dist = dists[j];
                    min_vertex = j;
                }
            }
        });

        if (min_dist != INT_MAX) {
            used[min_vertex] = true;
            tbb::parallel_for(tbb::blocked_range<int>(0, size), [&](const tbb::blocked_range<int>& r){
                for (int j = 0; j < size; j++) {
                    if (!used[j] && graph[min_vertex * size + j] != 0) {
                        tbb::spin_mutex::scoped_lock lock(mtx);
                        {
                            int new_dist = dists[min_vertex] +
                                graph[min_vertex * size + j];
                            if (new_dist < dists[j]) {
                                dists[j] = new_dist;
                            }
                        }
                    }
                }
            });
        }
    }

    return dists;
}

/**OMP**/
/*
std::vector<int> get_dists_dijkstra_parallel(std::vector<int> graph,
                int size, int start) {
    std::vector<int> dists(size, INT_MAX);
    std::vector<bool> used(size, false);
    dists[start] = 0;    
    for (int i = 0; i < size; i++) {
        int min_dist = INT_MAX;
        int min_vertex = -1;

        #pragma omp parallel for
        for (int j = 0; j < size; j++) {
            #pragma omp critical
            if (!used[j] && dists[j] < min_dist) {
                min_dist = dists[j];
                min_vertex = j;
            }
        }

        if (min_dist != INT_MAX) {
            used[min_vertex] = true;
            #pragma omp parallel for
            for (int j = 0; j < size; j++) {
                if (!used[j] && graph[min_vertex * size + j] != 0) {
                    #pragma omp critical
                    {
                        int new_dist = dists[min_vertex] +
                            graph[min_vertex * size + j];
                        if (new_dist < dists[j]) {
                            dists[j] = new_dist;
                        }
                    }
                }
            }
        }
    }
	
    return dists;
}
*/
