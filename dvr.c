#include <stdio.h>

#define MAX_NODES 10

struct node {
    unsigned int dist[MAX_NODES];
    unsigned int from[MAX_NODES];
} rt[MAX_NODES];

int main() {
    int costmat[MAX_NODES][MAX_NODES];
    int nodes, i, j, k, count = 0;

    printf("\nEnter the number of nodes: ");
    scanf("%d", &nodes);

    printf("\nEnter the cost matrix:\n");
    for (i = 0; i < nodes; i++) {
        for (j = 0; j < nodes; j++) {
            scanf("%d", &costmat[i][j]);
            costmat[i][i] = 0;
            rt[i].dist[j] = costmat[i][j];
            rt[i].from[j] = j;
        }
    }

    do {
        count = 0;
        for (i = 0; i < nodes; i++) {
            for (j = 0; j < nodes; j++) {
                for (k = 0; k < nodes; k++) {
                    if (rt[i].dist[j] > costmat[i][k] + rt[k].dist[j]) {
                        rt[i].from[j] = k;
                        rt[i].dist[j] = costmat[i][k] + rt[k].dist[j];
                        count++;
                    }
                }
            }
        }
    } while (count != 0);

    for (i = 0; i < nodes; i++) {
        printf("\n\nFor router %d:\n", i + 1);
        for (j = 0; j < nodes; j++) {
            printf("\tNode %d via %d distance %d\n", j + 1, rt[i].from[j] + 1, rt[i].dist[j]);
        }
    }
    
    printf("\n\n");
    return 0;
}
