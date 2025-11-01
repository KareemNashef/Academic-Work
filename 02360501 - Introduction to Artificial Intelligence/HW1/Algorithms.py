import numpy as np

from HaifaEnv import HaifaEnv
from typing import List, Tuple
import heapdict

##### Helper Classes and Functions #####

# Node class - represents a cell on the map
class Node():
    def __init__(self, state, parent = None, action = None, cost = 0):
        self.state = state          # The state in the space
        self.parent = parent        # The parent node
        self.action = action        # The action that led to this node
        self.cost = cost            # The cost of using this node

# findActions - returns a list of actions led to the given node
def findActions(node):
    actions = []
    while node.parent != None:
        actions.append(node.action)
        node = node.parent
    actions.reverse()
    return actions

########################################

class BFSGAgent():

    def search(self, env: HaifaEnv) -> Tuple[List[int], float, int]:

        # Get the initial node
        current_node = Node(env.get_initial_state())

        # Check if the initial node is the goal
        if env.is_final_state(current_node.state):
            return findActions(current_node), 0, 0      # TODO - Check if this is needed

        # Initialize the variables needed for the search
        queue = [current_node]
        seen = [current_node.state]
        expanded = 0

        # Iterate until the queue is empty
        while queue:

            # Pop the current node from the queue and expand it
            current_node = queue.pop(0)
            expanded += 1

            # Go over the possible actions and successors
            for action, (next_state, cost, terminated) in env.succ(current_node.state).items():

                # Check if we're in a hole
                if next_state is None:
                    continue

                # Check if we've already seen this state
                if next_state in seen:
                    continue

                # Add the state to the seen list
                seen.append(next_state)

                # Create a node for this state
                next_node = Node(next_state, current_node, action, current_node.cost + cost)

                # Check if this state is the goal
                if env.is_final_state(next_node.state):
                    return findActions(next_node), next_node.cost, expanded
                
                # Otherwise add it to the queue
                queue.append(next_node)

        # If we didn't find a solution
        return [], float('inf'), expanded
        

##### Heuristic Search #####

# HaifaHeuristic - returns the heuristic value of a node using its coordinates, list of goals and passway value
def HaifaHeuristic(coords, goals_coords, passway):

    # Calculate Manhattan distance to each goal
    manhattan_distances = [abs(coords[0] - goal[0]) + abs(coords[1] - goal[1]) for goal in goals_coords]
    
    # Find the minimum Manhattan distance
    min_distance = min(manhattan_distances)
    
    # Return the minimum of the Manhattan distance and the passway value
    return min(min_distance, passway)

# heuristicSearch - uses the given weight to search for a solution using the given heuristic
def heuristicSearch(env: HaifaEnv, c_weight: float, h_weight: float) -> Tuple[List[int], float, int]:

    # Get the initial node
    current_node = Node(env.get_initial_state())

    # Check if the initial node is the goal
    if env.is_final_state(current_node.state):
        return findActions(current_node), 0, 0      # TODO - Check if this is needed

    # Get a list of all the goal states in coordinates
    goals_coords = [env.to_row_col(goal_state) for goal_state in env.get_goal_states()]

    # Get the passway value
    passway = 100

    # Initialize the queue
    queue = heapdict.heapdict()

    # Calculate the weighted value for the node

    # Special case for Greedy and infinite cost
    if c_weight == 0 and current_node.cost == float('inf'):
        node_value = h_weight * HaifaHeuristic(env.to_row_col(current_node.state), goals_coords, passway)
    else:
        node_value = c_weight * current_node.cost + h_weight * HaifaHeuristic(env.to_row_col(current_node.state), goals_coords, passway)

    # Add the initial node to the queue
    queue[current_node] = (node_value, current_node.state)

    # Initialize the variables needed
    visited = set()
    expanded = 0

    # Iterate until the queue is empty
    while queue:

        # Pop the current node from the queue and expand it
        current_node = queue.popitem()[0]
        expanded += 1

        # Go over the possible actions and successors
        for action, (next_state, cost, terminated) in env.succ(current_node.state).items():

            # Check if we're in a hole
            if next_state is None:
                continue

            # Check if we've already seen this state
            if next_state in visited:
                continue

            # Add the state to the seen list
            visited.add(next_state)

            # Create a node for this state
            next_node = Node(next_state, current_node, action, current_node.cost + cost)

            # Check if this state is the goal
            if env.is_final_state(next_node.state):
                return findActions(next_node), next_node.cost, expanded
            
            # Calculate the weighted value for the node
            
            # Special case for Greedy and infinite cost
            if c_weight == 0 and next_node.cost == float('inf'):
                node_value = h_weight * HaifaHeuristic(env.to_row_col(next_node.state), goals_coords, passway)
            else:
                node_value = c_weight * next_node.cost + h_weight * HaifaHeuristic(env.to_row_col(next_node.state), goals_coords, passway)

            # Add the node to the queue
            queue[next_node] = (node_value, next_node.state)


    # If we didn't find a solution
    return [], float('inf'), expanded

############################

class GreedyAgent():
      
    def search(self, env: HaifaEnv) -> Tuple[List[int], float, int]:

        # Use the heuristic search function with c_weight = 0 and h_weight = 1
        return heuristicSearch(env, 0, 1)



class WeightedAStarAgent():
  
    def search(self, env: HaifaEnv, h_weight) -> Tuple[List[int], float, int]:
        
        # Use the heuristic search function with c_weight = 1 and h_weight = h_weight
        return heuristicSearch(env, 1 - h_weight, h_weight)



class AStarAgent():
    
    def search(self, env: HaifaEnv) -> Tuple[List[int], float, int]:
        
        # Use the heuristic search function with c_weight = 1 and h_weight = 1
        return heuristicSearch(env, 1, 1)

