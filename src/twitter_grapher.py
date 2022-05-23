import time
class Graph:
    def __init__(self, nodes, node_id):  
        self.adjlist = [[] for i in range(nodes)]    # make empty array (list) to hold all the edges
        self.indexes = {i:j for i,j in zip(node_id, range(nodes))}  #Each node will have an index to be accessed with
        self.followers = {i:0 for i in node_id}      # a dictionary holds number of followers for each node in a 
                                                     # (key : value) pair as {node_id : num_of_followers}

    def isValid(self, node):
        if node in self.indexes:         # if node already present in the graph before
            return 1
        else:
            return 0

    def MakeAdjacent(self, follower, followed):#O(E)
        if followed not in self.adjlist[self.indexes[follower]]: # if followed not in the follower adjlist
            self.adjlist[self.indexes[follower]].append(followed)
            self.followers[followed] = self.followers[followed] + 1


    def AddNode(self, node_id): #O(E)
        if self.isValid(node_id): #if node is already in graph #O(E)
            return 0
        else:
            self.indexes.update({node_id: len(self.adjlist)}) #len --> O(1)
            self.adjlist.append([])
            self.followers.update({node_id: 0})

    def Get_Influencers(self):  ## heapsort O(V log(V) )
        p = sorted(self.followers.items(), key=lambda x: x[1], reverse=True)  ## heapsort O(V log(V) )

        answer = ""
        i = 0
        while (answer.lower() != "n"):
            print("The Influencer with id: {0:<10} has {1:<5} followers ".format(p[i][0], p[i][1]))
            i += 1
            answer = input("Do you want another top follower (Y/N)? : ")


    # Suggests friends
    def Suggest_Followers(self, node_id, num_of_suggestions):   # ----> O(V^2)
        node = self.indexes[node_id]     # getting node index
        children = [self.indexes[n] for n in self.adjlist[node]]  # O(V)
        connections = {}
        for child in children:   # ----> O(V^2)
            grandchildren = self.adjlist[child]
            for vertex in grandchildren:
                if self.indexes[vertex] in children or vertex == node_id:
                    continue
                # If connection exists add mutual friends number
                if vertex in connections.keys():
                    connections[vertex] += 1
                # Initialise to 1
                else:
                    connections[vertex] = 1

        connections = sorted(list(connections.items()), key=lambda x: x[1], reverse=True) ## heapsort O(V log(V) )

        z = range(num_of_suggestions)
        for i in z:
            print("You can follow {0:<10} you both have {1:<4} in common".format(connections[i][0], connections[i][1]))
        
def main():
    start_time = time.time()
    G = Graph(0, [])
    with open('./Data/twitter.csv', 'r') as f:
        results = []
        for line in f:  #O(E)
            words = line.split(',')
            G.AddNode(int(words[0]))
            G.AddNode(int(words[1]))
            G.MakeAdjacent(int(words[0]), int(words[1]))
    print("--- %s seconds ---" % (time.time() - start_time))

    G.Get_Influencers()      # ----> O(V log v)
    print("------------------------------")
    
    id = int(input(">> Input an id to suggest some friends: "))
    num = int(input(">> Enter The maximum number of suggestions: "))
    print()
    G.Suggest_Followers(id, num) # ----> O(V^2)
    print()

if __name__ == '__main__':
    main()