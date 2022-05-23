#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

typedef struct graph
{
    int id;
    // int following;
    // int follower;
    int index;
    struct graph *next;
    // struct graph **adj;
} GRAPH;

static GRAPH *head;

GRAPH *createGraph(int, int);
int add2Graph(int);
void makeAdj(int, int, int, int);
void destroyGraph();
void readCSV();
// int **createUserFollower(int **, int);
void maxHeapify(int **, int, int);
int **heapSort(int **, int);

int array_current_size;
int **A;
int **userArray;
int totalUser, i;

int main()
{
    /* ---------------------  Initialization ---------------------- */

    FILE *fp;
    totalUser = 0;
    double time_spent = 0.0;

    clock_t begin = clock();

    fp = fopen("twitter_results2.txt", "w");

    A = (int **)malloc(sizeof(int *) * 1);
    if (A == NULL)
    {
        fprintf(stderr, "Adjacency Matrix Malloc error\n");
        exit(EXIT_FAILURE);
    }
    A[0] = (int *)malloc(sizeof(int) * 1);
    if (A[0] == NULL)
    {
        fprintf(stderr, "Adjacency Matrix Malloc error\n");
        exit(EXIT_FAILURE);
    }
    A[0][0] = 0;
    array_current_size = 1;

    userArray = (int **)malloc(sizeof(int *) * 2);
    if (userArray == NULL)
    {
        fprintf(stderr, "userArray malloc error\n");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < 2; i++)
    {
        userArray[i] = (int *)malloc(sizeof(int) * 1);
        if (userArray[i] == NULL)
        {
            fprintf(stderr, "userArray malloc error\n");
            exit(EXIT_FAILURE);
        }
        userArray[i][0] = 0;
    }

    /* -------------------------> Add Intro Logo <---------------------------*/
    /* ---------------------  Action Starts Here  ---------------------- */

    readCSV();
    printf("\n > Total Number Of Users: %d users\n", totalUser);

    userArray = heapSort(userArray, totalUser);
    printf("\n > Users Ordered by Total Followers number\n");
    for (i = totalUser - 1; i >= 0; i--)
    {
        printf("%-12d : %-6d\n", userArray[0][i], userArray[1][i]);
        fprintf(fp, "%-12d : %-6d\n", userArray[0][i], userArray[1][i]);
    }

    for (int i = 0; i < totalUser; i++)
    {
        for (int j = 0; j < totalUser; j++)
        {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }

    destroyGraph();

    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("The elapsed time is %f seconds", time_spent);

    return 0;
}

/**********************************
##### CREATING A GRAPH OBJECT #####
**********************************/

GRAPH *createGraph(int id, int index)
{
    GRAPH *p = (GRAPH *)malloc(sizeof(GRAPH));
    if (p == NULL)
    {
        fprintf(stderr, "createGRAPH(), malloc error\n");
        exit(EXIT_FAILURE);
    }
    p->id = id;
    // p->following = 0;
    // p->follower = 0;
    p->index = index;
    p->next = NULL;
    // p->adj = NULL;
    return p;
}

/***********************************************************************
##### check id whether if it has already been in the graph or not. #####
##### If not, function will create new node and icrease totalUser. #####
***********************************************************************/

int add2Graph(int id)
{
    GRAPH *p, *t;
    // spin as long as the graph end hasn't been reached
    // or the new id hasn't already inlisted in graph before
    for (p = head; p != NULL && p->id != id; t = p, p = p->next)
        ;

    // in case empty graph or reached the end and haven't found the node
    if (p == NULL)
    {
        p = createGraph(id, totalUser);
        totalUser++;
        if (head == NULL) // empty graph
        {
            head = p;
        }
        else // graph end
        {
            t->next = p;
        }
    }
    return p->index;
}

void resizeArray(int id_Index)
{
    int M = id_Index + 100, N = array_current_size;

    // Reallocate the array of pointer to int (increase number of columns)
    A = (int **)realloc(A, sizeof(int **) * M);
    if (A == NULL)
    {
        fprintf(stderr, "Adjacency Matrix Reallocation error\n");
        destroyGraph();
        exit(EXIT_FAILURE);
    }

    // Reallocate the previously allocated row to increase the size
    for (int i = 0; i < N; ++i)
    {
        // *(A+i) same as A[i]
        *(A + i) = (int *)realloc(*(A + i), sizeof(int) * M);
        if (*(A + i) == NULL)
        {
            fprintf(stderr, "Adjacency Matrix Reallocation error\n");
            destroyGraph();
            exit(EXIT_FAILURE);
        }
    }

    for (int i = N; i < M; ++i)
    {
        *(A + i) = (int *)malloc(sizeof(int) * M);
    }

    for (int i = 0; i < N; ++i)
    {
        for (int j = N; j < M; ++j)
        {
            *(*(A + i) + j) = 0; // filling new columns of old rows
        }
    }

    for (int i = N; i < M; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            *(*(A + i) + j) = 0;
        }
    }

    // userArray = (int **)realloc(userArray, M * sizeof *userArray);

    *(userArray + 0) = (int *)realloc(*(userArray + 0), sizeof(int) * M);
    *(userArray + 1) = (int *)realloc(*(userArray + 1), sizeof(int) * M);
    if (*(userArray + 0) == NULL || *(userArray + 1) == NULL)
    {
        fprintf(stderr, "userArray Reallocation error\n");
        destroyGraph();
        exit(EXIT_FAILURE);
    }
    // Since the number of columns is  M>N  we can allocate  M−N  new rows.
    // Now its again a perfect matrix

    // Fill the  M−N  new empty entries for each of the  N  “old rows”

    for (int j = N; j < M; ++j)
    {
        *(*(userArray + 0) + j) = 0; // filling new columns of old rows
        *(*(userArray + 1) + j) = 0;
    }

    // Fill the  N  empty entries for each of the  M−N  “new rows”

    // update current size for further operations
    array_current_size = M;
}

/********************************************************************************************************
##### MAking connection between nodes. If nodes are already connected(linear search) --> Do Nothing #####
##### If follower node's does not have a adjacent, function will allocate one block with malloc(),  #####
##### if it has, it will reallocate +1 block with realloc() Following node's follower number and    #####
##### follower node's following number will increase.                                               #####
********************************************************************************************************/

void makeAdj(int follower, int followed, int followed_Id, int follower_Id)
{
    if (follower > followed && follower > array_current_size - 1)
    {
        resizeArray(follower);
    }
    else if (followed > follower && followed > array_current_size - 1)
    {
        resizeArray(followed);
    }

    if (A[follower][followed] != 1)
        userArray[1][followed] += 1;

    A[follower][followed] = 1;
    if (userArray[0][followed] != followed_Id)
        userArray[0][followed] = followed_Id;
    if (userArray[0][follower] != follower_Id)
        userArray[0][follower] = follower_Id;
}

/***********************************************************************************
##### Freeing allocated memory for graph From the beginning to end             #####
##### Deallocating adjancency array of all nodes, then deallocating all nodes. #####
************************************************************************************/

void destroyGraph()
{
    GRAPH *p;
    while (head)
    {
        p = head;
        head = head->next;
        free(p);
    }

    for (int i = 0; i < array_current_size; i++)
    {
        free(A[i]);
    }
    free(A);

    for (int i = 0; i < 2; i++)
    {
        free(userArray[i]);
    }
    free(userArray);
}

/************************************************************************
##### Reading CSV file and Forwarding data to graph Creator handler #####
************************************************************************/

void readCSV()
{
    FILE *fp;
    char filename[20] = "a.csv";
    char comma;
    int follower, followed;
    int fr, fd;

    // printf(">Insert The Data File Name: ");
    // scanf("%s", &filename);

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "%s file could not opened\n", filename);
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%s", &filename);
    totalUser = 0;
    while (!feof(fp))
    {
        fscanf(fp, "%d %c %d", &follower, &comma, &followed);
        fr = add2Graph(follower); // returns follower index
        fd = add2Graph(followed); // returns followed index
        makeAdj(fr, fd, followed, follower);
    }
    fclose(fp);
}

/*****************************************************************************************************
##### Heapifying the lowest smallest trees and gradually move up until we reach the root element #####
*****************************************************************************************************/

void maxHeapify(int **arr, int n, int i)
{
    // Find largest among root, left child and right child
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[1][left] > arr[1][largest])
    {
        largest = left;
    }

    if (right < n && arr[1][right] > arr[1][largest])
    {
        largest = right;
    }

    // Swap and continue heapifying if root is not largest
    if (largest != i)
    {
        int temp = arr[0][i];
        arr[0][i] = arr[0][largest];
        arr[0][largest] = temp;

        temp = arr[1][i];
        arr[1][i] = arr[1][largest];
        arr[1][largest] = temp;

        maxHeapify(arr, n, largest);
    }
}

/*****************************************************************************************************
##### Sorting The Array As Max-Heap According to Number of Followers (arr[1][i])#####
*****************************************************************************************************/

int **heapSort(int **arr, int n)
{
    int i;
    // Build max heap
    for (i = n / 2 - 1; i >= 0; i--)
    {
        maxHeapify(arr, n, i);
    }

    // Heap sort swapping
    for (i = n - 1; i > 0; i--)
    {
        int temp = arr[0][0];
        arr[0][0] = arr[0][i];
        arr[0][i] = temp;

        temp = arr[1][0];
        arr[1][0] = arr[1][i];
        arr[1][i] = temp;

        // Heapify root element to get highest element at root again
        maxHeapify(arr, i, 0);
    }
    return arr;
}
