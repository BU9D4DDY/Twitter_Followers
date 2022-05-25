import time
class GRAPH:
    def __init__(self):
        self.Following_adjlist = []  # make empty array (list) to hold the following Id for each Id O(1)
        self.Followers_adjlist = []  # make empty array (list) to hold the followers Id for each Id O(1)
        self.indexes = {}  # Each node will have an index to be accessed with    O(1)
        self.followers = {}  # a dictionary holds number of followers for each node the graph          O(1)
                                                  # (key : value) pair as {node_id : num_of_followers}
                                                  
    def isValid(self, node):  # O(V)
        if node in self.indexes:  # if node already present in the graph before
            return 1
        else:
            return 0


    def MakeAdjacent(self, follower, followed):  # O(V)
        if followed not in self.Following_adjlist[self.indexes[follower]]:  # if followed not in the follower adjlist //O(a) where a is the following array elements for specific Id
            self.Following_adjlist[self.indexes[follower]].append(followed) #O(1)
            self.followers[followed] = self.followers[followed] + 1 #O(1)
            self.Followers_adjlist[self.indexes[followed]].append(follower) #O(1)

        #if follower not in self.Followers_adjlist[self.indexes[followed]]: # if follower not in the followed adjlist //O(a) where a is the followers array elements for specific Id


    def AddNode(self, node_id):  # O(V) because of validation
        if self.isValid(node_id):  # if node is already in graph #O(V)
            return 0
        else:
            self.indexes.update({node_id: len(self.Following_adjlist)})  # len --> O(1)
            self.Following_adjlist.append([])                            # amortized O(1)
            self.Followers_adjlist.append([])                           # amortized O(1)
            self.followers.update({node_id: 0})                #O(1)


    def Get_Influencers(self):  ## heapsort O(V log(V) )
        p = sorted(self.followers.items(), key=lambda x: x[1], reverse=True)  ## heapsort O(V log(V) )

        answer = ""
        i = 0
        while (answer.lower() != "n"):
            print("The Influencer with id: {0:<10} has {1:<5} followers ".format(p[i][0], p[i][1]))
            i += 1
            answer = input("Do you want another top follower (Y/N)? : ")


    # Suggests friends
    def Suggest_Followers(self, node_id, num_of_suggestions):  # ----> O(a^2)
        node = self.indexes[node_id]  # getting node index
        children = [self.indexes[n] for n in self.Following_adjlist[node]]  # O(a)
        connections = {}
        for child in children:  # ----> O(a)
            grandchildren = self.Following_adjlist[child]
            for vertex in grandchildren: #O(a^2)
                if self.indexes[vertex] in children or vertex == node_id:
                    continue
                # If connection exists add mutual friends number
                if vertex in connections.keys():
                    connections[vertex] += 1
                # Initialise to 1
                else:
                    connections[vertex] = 1

        connections = sorted(list(connections.items()), key=lambda x: x[1], reverse=True)  ## heapsort O(V log(V) )
        if not connections: #O(a^2) //List is empty
            connections = {}
            children = [self.indexes[n] for n in self.Followers_adjlist[node]]  # O(a)
            for child in children:  # ----> O(a^2)
                grandchildren = self.Following_adjlist[child] # O(a)

                for vertex in grandchildren: # O(a^2)
                    if vertex == node_id:
                        continue
                    # If connection exists add mutual friends number
                    if vertex in connections.keys():
                        connections[vertex] += 1
                    # Initialise to 1
                    else:
                        connections[vertex] = 1

            connections = sorted(list(connections.items()), key=lambda x: x[1], reverse=True)  ## heapsort O(V log(V) )
            
            
        if num_of_suggestions > len(connections):
            num_of_suggestions = len(connections)
        z = range(num_of_suggestions)
        for i in z:
            print("You can follow {0:<10} you both have {1:<4} in common".format(connections[i][0], connections[i][1]))


if __name__ == '__main__':
    start_time = time.time()
    G = GRAPH()  #O(1)
    with open("twitter.csv", 'r') as f:
        for line in f:  # O(VE)
            words = line.split(',')
            G.AddNode(int(words[0])) #O(V)
            G.AddNode(int(words[1])) #O(V)
            G.MakeAdjacent(int(words[0]), int(words[1])) #O(V)
    
    print("\n>> Filling The Graph Takes: %s seconds ---\n" % (time.time() - start_time))

    G.Get_Influencers()  # ----> O(V log v)
    print("------------------------------")

    
    node_id = int(input(">> Input an id to suggest some friends: "))
    num = int(input(">> Enter The maximum number of suggestions: "))
    
    if G.isValid(node_id):
        start_tim = time.time()
        print()
        G.Suggest_Followers(node_id, num)  # ----> O(a^2)
        print("\n>> Suggesting Followers Takes: %s seconds ---" % (time.time() - start_tim))
        print()
    else:
        print("Invalid User Id !!!")