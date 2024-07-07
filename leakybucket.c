#include <stdio.h>

int main() {
    int incoming, outgoing, bucket_size, n, store = 0;

    printf("Enter bucket size, outgoing rate, and number of inputs: ");
    scanf("%d %d %d", &bucket_size, &outgoing, &n);

    while (n > 0) {
        printf("\nEnter the incoming packet size: ");
        scanf("%d", &incoming);
        printf("Incoming packet size: %d\n", incoming);

        if (incoming <= (bucket_size - store)) {
            store += incoming;
            printf("Bucket buffer size: %d out of %d\n", store, bucket_size);
        } else {
            printf("Dropped %d packets\n", incoming - (bucket_size - store));
            printf("Bucket buffer size: %d out of %d\n", store, bucket_size);
            store = bucket_size;
        }

        store = store - outgoing;
        if (store < 0)
            store = 0;

        printf("After outgoing, %d packets left out of %d in buffer\n", store, bucket_size);
        n--;
    }

    return 0;
}
