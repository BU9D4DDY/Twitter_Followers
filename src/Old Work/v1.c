#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

typedef struct graph
{
    int id;
    int following;
    int follower;
    //int distance;
    struct graph *next;
    struct graph **adj;
} GRAPH;

static GRAPH *head;

GRAPH *createGraph(int);
GRAPH *add2Graph(int, int *);
void makeAdj(GRAPH *, GRAPH *);
void destroyGraph();
void readCSV(int *);
int **createUserFollower(int **, int);
void maxHeapify(int **, int, int);
int **heapSort(int **, int);

int main()
{
    double time_spent = 0.0;
    clock_t begin = clock();

    FILE *fp;
    fp = fopen("twitter_results1.txt", "w");

    /* -------------------------> Add Logo <---------------------------*/

    int totalUser = 0, **userFollowerArray, i;
    readCSV(&totalUser);
    printf("Total Number Of Users: %d users\n", totalUser);

    userFollowerArray = createUserFollower(userFollowerArray, totalUser);
    userFollowerArray = heapSort(userFollowerArray, totalUser);
    printf("\n");
    /* ------------> To Be Updated <------------------------*/
    printf("Ordered by follower number\n");
    for (i = totalUser - 1; i > totalUser-21 ; i--)
    {
        printf("%-12d : %-6d\n", userFollowerArray[0][i], userFollowerArray[1][i]);
        fprintf(fp, "%-12d : %-6d\n", userFollowerArray[0][i], userFollowerArray[1][i]);
    }
    /* ------------------------------------------------------*/
    /*
    *The elapsed time is 516.372 seconds
    *The elapsed time is 8.6062 minutee
    */

    for (i = 0; i < 2; i++)
    {
        free(userFollowerArray[i]);
    }
    free(userFollowerArray);

    destroyGraph();
    
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("The elapsed time is %f seconds", time_spent);
    
    
    return 0;
}

/**********************************
##### CREATING A GRAPH OBJECT #####
**********************************/

GRAPH *createGraph(int id)
{
    GRAPH *p = (GRAPH *)malloc(sizeof(GRAPH));
    if (p == NULL)
    {
        fprintf(stderr, "createGRAPH(), malloc error\n");
        exit(EXIT_FAILURE);
    }
    p->id = id;
    p->following = 0;
    p->follower = 0;
    //p->distance = INT_MAX;
    p->next = NULL;
    p->adj = NULL;
    return p;
}

/***********************************************************************
##### check id whether if it has already been in the graph or not. #####
##### If not, function will create new node and icrease totalUser. #####
***********************************************************************/

GRAPH *add2Graph(int id, int *totalUser)
{
    GRAPH *p, *t;
    // spin as long as the graph end hasn't been reached
    // or the new id hasn't already inlisted in graph before
    for (p = head; p != NULL && p->id != id; t = p, p = p->next)
        ;

    // in case empty graph or reached the end
    if (p == NULL)
    {
        *totalUser += 1;
        p = createGraph(id);
        if (head == NULL) // empty graph
        {
            head = p;
        }
        else // graph end
        {
            t->next = p;
        }
    }
    return p;
}

/********************************************************************************************************
##### MAking connection between nodes. If nodes are already connected(linear search) --> Do Nothing #####
##### If follower node's does not have a adjacent, function will allocate one block with malloc(),  #####
##### if it has, it will reallocate +1 block with realloc() Following node's follower number and    #####
##### follower node's following number will increase.                                               #####
********************************************************************************************************/

void makeAdj(GRAPH *follower, GRAPH *followed)
{
    int i;
    for (i = 0; i < follower->following; i++)
    {
        if (follower->adj[i] == followed)
        {
            return;
        }
    }
    follower->following++;
    if (follower->adj == NULL)
    {
        follower->adj = (GRAPH **)malloc(sizeof(GRAPH *));
        if (follower->adj == NULL)
        {
            fprintf(stderr, "makeAdj(), malloc error\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // reallocate the size of adjlist with the new numbers of followed ids by this follower
        follower->adj = (GRAPH **)realloc(follower->adj, follower->following * sizeof(GRAPH *));
    }
    follower->adj[follower->following - 1] = followed;
    followed->follower++;
}

/***********************************************************************************
##### Freeing allocated memory for graph From the beginning to end             #####
##### Deallocating adjancency array of all nodes, then deallocating all nodes. #####
************************************************************************************/

void destroyGraph()
{
    GRAPH *p;
    for (p = head; p != NULL; p = p->next)
    {
        free(p->adj);
        p->adj = NULL;
    }
    while (head)
    {
        p = head;
        head = head->next;
        free(p);
    }
}

/************************************************************************
##### Reading CSV file and Forwarding data to graph Creator handler #####
************************************************************************/

void readCSV(int *totalUser)
{
    FILE *fp;
    char filename[20] = "twitter.csv";
    char comma;
    int follower, followed;
    GRAPH *fr = NULL, *fd = NULL;

    //printf("\n>Insert The Data File Name: ");
    //scanf("%s", &filename);

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "%s file could not opened\n", filename);
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%s", &filename);

    while (!feof(fp))
    {
        fscanf(fp, "%d %c %d", &follower, &comma, &followed);
        fr = add2Graph(follower, totalUser);
        fd = add2Graph(followed, totalUser);
        makeAdj(fr, fd);
        fr = NULL;
        fd = NULL;
    }
    fclose(fp);
}

/*****************************************************************************
##### Collecting All Users IDs And Their Followers Numbers in a 2D Array #####
*****************************************************************************/

int **createUserFollower(int **userArray, int totalUser)
{
    GRAPH *p;
    int i, j, k;
    userArray = (int **)malloc(sizeof(int *) * 2);
    if (userArray == NULL)
    {
        fprintf(stderr, "createUserFollower(), malloc error\n");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < 2; i++)
    {
        userArray[i] = (int *)malloc(sizeof(int) * totalUser);
        if (userArray[i] == NULL)
        {
            fprintf(stderr, "createUserFollower(), malloc error\n");
            exit(EXIT_FAILURE);
        }
    }
    for (i = 0, p = head; i < totalUser; i++, p = p->next)
    {
        userArray[0][i] = p->id;
        userArray[1][i] = p->follower;
    }
    return userArray;
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
