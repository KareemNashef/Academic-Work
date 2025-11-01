from Agent import Agent, AgentGreedy
from WarehouseEnv import WarehouseEnv, manhattan_distance
import random
import time

# Constants
PADDING = 0.1
BoardSize = 5
BatteryThreshold = 5
MaxDistance = 8

# Weights
Weights = {
    'CreditWeight':                  18,
    'CreditBonus':                   10,
    'CreditPenalty':                0.5,
    'BatteryWeight':                  5,
    'ChargeIncentiveFactor':          1,
    'StationChargeWeight':           17,
    'DirectionWeight':              300,
    'PackageGetWeight':              15,
    'DeliveryBonus':                  5,
    'DeliveryPriority':               1,
    'GeneralDeliveryIncentive':      35,
    'OpponentBeatBonus':              0,
    'OpponentBlockBonus':             0,
    'EndgameBonus':                   0
}

# Functions
def find_best_package(env: WarehouseEnv, robot_id: int):

    # Get the robot
    robot = env.get_robot(robot_id)
    opponent_robot = env.get_robot((robot_id + 1) % 2)

    # Get the packages
    package1 = env.packages[0]
    package2 = env.packages[1]

    # Calculate the value of each package
    package1_value = manhattan_distance(package1.position, package1.destination)
    package2_value = manhattan_distance(package2.position, package2.destination)

    # Calculate the distance between the robot and each package
    package1_distance = manhattan_distance(robot.position, package1.position)
    package2_distance = manhattan_distance(robot.position, package2.position)

    # Check if both packages are on the board
    if env.packages[0].on_board and env.packages[1].on_board:
        
        # Prioritize the closest package if the credit is 0
        if robot.credit <= opponent_robot.credit:
            if package1_distance < package2_distance:
                return package1, package1_distance
            else:
                return package2, package2_distance

        # Find the best package
        if package1_value - package1_distance > package2_value - package2_distance:
            return package1, package1_distance
        else:
            return package2, package2_distance

    # Check if only one package is on the board
    elif env.packages[0].on_board or env.packages[1].on_board:

        # Return the package that is on the board
        if env.packages[0].on_board:
            return package1, package1_distance
        else:
            return package2, package2_distance
    
    # No package is on the board
    else:
        return None, float("inf")

def find_nearest_station(env: WarehouseEnv, robot_id: int):

    # Get the robots
    Player = env.get_robot(robot_id)
    Opponent = env.get_robot((robot_id + 1) % 2)

    # Initialize the nearest station distance
    nearest_station_distance = float("inf")

    # Find the nearest station
    for station in env.charge_stations:

        # Make sure the opponent does not block the station
        if station.position == Opponent.position:
            continue

        # Calculate the distance between the robot and the station
        distance = manhattan_distance(Player.position, station.position)

        # Update the nearest station distance
        nearest_station_distance = min(nearest_station_distance, distance)

    # Return the nearest station distance
    return nearest_station_distance

def calculate_delivery_incentive(package_value, distance):
    return package_value * (BoardSize * 3 - distance) / BoardSize

# =================================== Heuristic ===================================

def smart_heuristic(env: WarehouseEnv, robot_id: int):

    # Get the players
    Player = env.get_robot(robot_id)
    Opponent = env.get_robot((robot_id + 1) % 2)

    # Initialize the key elements
    Score = 0
    MovesLeft = env.num_steps
    BestPackage, BestPackageDistance = find_best_package(env, robot_id)
    NearestStationDistance = find_nearest_station(env, robot_id)

    # 1. Credit difference
    CreditDifference = Player.credit - Opponent.credit
    Score += CreditDifference * Weights['CreditWeight']

    # 1.1. Bonus for keeping battery above threshold
    if Player.battery > BatteryThreshold:
        Score += CreditDifference * Weights['CreditBonus']
    else:
        Score -= CreditDifference * Weights['CreditPenalty']

    # 2. Battery level difference
    Score += (Player.battery - Opponent.battery) * Weights['BatteryWeight']

    # 3. General insentive to charge battery
    charge_incentive = Weights['ChargeIncentiveFactor'] / max(Player.battery, 1)
    Score += (MaxDistance - NearestStationDistance) * charge_incentive

    # 4. Package opportunities
    # 4.1. The player has no package
    if Player.package is None:

        # 4.1.1. Battery is too low
        if Player.battery < BestPackageDistance:

            # Incentvise to charge if possible
            if Player.credit > 0:
                Score += Weights['DirectionWeight'] - NearestStationDistance * Weights['StationChargeWeight']

            # Penalize if not possible
            else:
                Score -= Weights['DirectionWeight'] / 4 - Player.battery

        # 4.1.2. Battery is high enough
        else:

            # Prioritize getting the best package
            Score += Weights['DirectionWeight'] - BestPackageDistance * Weights['PackageGetWeight']
            
            # Bonus if we can reach and deliver the package
            PackageValue = manhattan_distance(BestPackage.position, BestPackage.destination) * 2
            if BestPackageDistance + (PackageValue / 2) < Player.battery:
                Score += calculate_delivery_incentive(PackageValue, BestPackageDistance) * Weights['DeliveryBonus']

    # 4.2. The player has a package
    else:

        # Delivery information
        DistToDestination = manhattan_distance(Player.position, Player.package.destination)
        PackageValue = manhattan_distance(Player.package.position, Player.package.destination) * 2

        # General delivery insentive
        Score += (MaxDistance - DistToDestination) * Weights['GeneralDeliveryIncentive']

        # Case 4.2.1: The battery is too low
        if DistToDestination >= Player.battery:

            # Check if we can charge
            if Player.credit > 0:
                ChargePriority = PackageValue / (NearestStationDistance + 1)  # Avoid division by zero
                Score += (ChargePriority * 25) + (50 - NearestStationDistance) * 2

            # Can't charge - penalize based on distance to station
            else:
                Score -= NearestStationDistance * 15

        # Case 4.2.2: The battery is high enough
        else:

            # Prioritize delivering the package
            Score += calculate_delivery_incentive(PackageValue, DistToDestination) * 30
            Score += Weights['DeliveryPriority'] - 4 * DistToDestination


        # 4.2.1. Opponent has package
        if Opponent.package is not None:

            # Calculate the opponent's delivery distance
            OpponentDeliveryDist = manhattan_distance(Opponent.position, Opponent.package.destination)

            # Check if we can deliver
            if DistToDestination < Player.battery:
                Score += (BoardSize * 3 - DistToDestination) * 50

                # Extra bonus if we'll deliver first
                if DistToDestination < OpponentDeliveryDist:
                    Score += Weights['OpponentBeatBonus']

    # 5. Opponent blocking - only if we're ahead
    if manhattan_distance(Player.position, Opponent.position) == 1 and Player.credit > Opponent.credit:
        Score += Weights['OpponentBlockBonus']

    # 6. Endgame
    if MovesLeft < MaxDistance:

        # Reward having more credits
        Score += CreditDifference * Weights['EndgameBonus']

    return Score

# =================================== Greedy ===================================

class AgentGreedyImproved(AgentGreedy):
    def heuristic(self, env: WarehouseEnv, robot_id: int):
        return smart_heuristic(env, robot_id)

# =================================== MiniMax ===================================

def minimax(state, depth, maximizing_player, agent_id, allocated_time):

    # check the timer
    if time.time() > allocated_time:
        raise Exception("Time limit exceeded")
    
    # check if the depth is 0 or the state is terminal
    if depth <= 0 or state.done():
        return smart_heuristic(state, agent_id), None

    # maximizing player
    if maximizing_player:

        # initialize the max evaluation and the taken action
        max_eval = float('-inf')
        best_action = None

        # get the legal actions
        actions = state.get_legal_operators(agent_id)
        
        # loop over the legal actions
        for action in actions:

            # clone the state
            child = state.clone()

            # apply the action to the child state
            child.apply_operator(agent_id, action)

            # get the evaluation of the child state
            eval, _ = minimax(child, depth - 1, False, agent_id, allocated_time)
            
            # update the max evaluation and the taken action
            if eval > max_eval:
                max_eval = eval
                best_action = action

        # return the max evaluation or the taken action
        return max_eval, best_action

    # minimizing player
    else:

        # initialize the min evaluation and the taken action
        min_eval = float('inf')
        best_action = None

        # get the current player
        other_agent = (agent_id + 1) % 2

        # get the legal actions
        actions = state.get_legal_operators(other_agent)
        
        # loop over the legal actions
        for action in actions:

            # clone the state
            child = state.clone()

            # apply the action to the child state
            child.apply_operator(other_agent, action)

            # get the evaluation of the child state
            eval, _ = minimax(child, depth - 1, True, agent_id, allocated_time)

            # update the min evaluation and the taken action
            if eval < min_eval:
                min_eval = eval
                best_action = action

        # return the min evaluation
        return min_eval, best_action

class AgentMinimax(Agent):

    def run_step(self, env: WarehouseEnv, agent_id, time_limit):
        
        # initialize the allowed time
        allocated_time = time.time() + time_limit - PADDING

        # initialize the best action and depth
        latest_action = None
        depth = 1

        # loop over minimax with increasing depth in a try-except block
        try:
            while True:
                
                # check the timer
                if time.time() > allocated_time:
                    raise Exception("Time limit exceeded")
                
                # get the latest action and update the depth
                _, latest_action = minimax(env, depth, True, agent_id, allocated_time)
                
                # increment the depth
                depth += 1

        except:
            return latest_action

# =================================== AlphaBeta ===================================

def alphabeta(state, depth, maximizing_player, agent_id, allocated_time, alpha, beta):

    # check the timer
    if time.time() > allocated_time:
        raise Exception("Time limit exceeded")
    
    # check if the depth is 0 or the state is terminal
    if depth <= 0 or state.done():
        return smart_heuristic(state, agent_id), None
    
    # maximizing player
    if maximizing_player:

        # initialize the max evaluation and the taken action
        max_eval = float('-inf')
        best_action = None

        # get the legal actions
        actions = state.get_legal_operators(agent_id)
        
        # loop over the legal actions
        for action in actions:

            # clone the state
            child = state.clone()

            # apply the action to the child state
            child.apply_operator(agent_id, action)

            # get the evaluation of the child state
            eval, _ = alphabeta(child, depth - 1, False, agent_id, allocated_time, alpha, beta)
            
            # update the max evaluation and the taken action
            if eval > max_eval:
                max_eval = eval
                best_action = action

            # update the alpha value
            alpha = max(alpha, eval)

            # check if the beta value is less than or equal to the alpha value
            if beta <= alpha:
                break

        # return the max evaluation or the taken action
        return max_eval, best_action

    # minimizing player
    else:

        # initialize the min evaluation and the taken action
        min_eval = float('inf')
        best_action = None


        # get the current player
        other_agent = (agent_id + 1) % 2

        # get the legal actions
        actions = state.get_legal_operators(other_agent)
        
        # loop over the legal actions
        for action in actions:

            # clone the state
            child = state.clone()

            # apply the action to the child state
            child.apply_operator(other_agent, action)

            # get the evaluation of the child state
            eval, _ = alphabeta(child, depth - 1, True, agent_id, allocated_time, alpha, beta)

            # update the min evaluation and the taken action
            if eval < min_eval:
                min_eval = eval
                best_action = action
                
            # update the beta value
            beta = min(beta, eval)

            # check if the beta value is less than or equal to the alpha value
            if beta <= alpha:
                break
            
        # return the min evaluation
        return min_eval, best_action
    
class AgentAlphaBeta(Agent):

    def run_step(self, env: WarehouseEnv, agent_id, time_limit):

        # initialize the allowed time
        allocated_time = time.time() + time_limit - PADDING

        # initialize the best action, depth, alpha, and beta
        latest_action = None
        depth = 1
        alpha = float('-inf')
        beta = float('inf')

        # loop over alphabeta with increasing depth in a try-except block
        try:
            while True:

                # check the timer
                if time.time() > allocated_time:
                    raise Exception("Time limit exceeded")
                
                # get the latest action and update the depth
                _, latest_action = alphabeta(env, depth, True, agent_id, allocated_time, alpha, beta)
                
                # increment the depth
                depth += 1

        except:
            return latest_action

# =================================== Expectimax ===================================

def expectimax(state, depth, maximizing_player, agent_id, allocated_time):

    # check the timer
    if time.time() > allocated_time:
        raise Exception("Time limit exceeded")
    
    # check if the depth is 0 or the state is terminal
    if depth <= 0 or state.done():
        return smart_heuristic(state, agent_id), None
    
    # maximizing player
    if maximizing_player:

        # initialize the max evaluation and the taken action
        max_eval = float('-inf')
        best_action = None

        # get the legal actions
        actions = state.get_legal_operators(agent_id)
        
        # loop over the legal actions
        for action in actions:

            # clone the state
            child = state.clone()

            # apply the action to the child state
            child.apply_operator(agent_id, action)

            # get the evaluation of the child state
            eval, _ = expectimax(child, depth - 1, False, agent_id, allocated_time)
            
            # update the max evaluation and the taken action
            if eval > max_eval:
                max_eval = eval
                best_action = action

        # return the max evaluation or the taken action
        return max_eval, best_action

    # chance player
    else:

        # initialize the average evaluation and the taken action
        total_eval = 0

        # get the current player
        other_agent = (agent_id + 1) % 2

        # get the legal actions and create children states
        actions = state.get_legal_operators(other_agent)

        # Calculate probabilities
        probabilities = {}
        total_weight = len(actions)
        
        for action in actions:
            if action == "drop_off" or action == "move east":
                probabilities[action] = 2  # Double weight
                total_weight += 1
            else:
                probabilities[action] = 1
        
        # Normalize probabilities
        for action in probabilities:
            probabilities[action] /= total_weight
        
        # loop over the legal actions
        for action in actions:

            # clone the state
            child = state.clone()

            # apply the action to the child state
            child.apply_operator(other_agent, action)

            # get the evaluation of the child state
            eval, _ = expectimax(child, depth - 1, True, agent_id, allocated_time)

            # update the total evaluation
            total_eval += eval * probabilities[action]

        # return the average evaluation
        return total_eval, None

class AgentExpectimax(Agent):

    def run_step(self, env: WarehouseEnv, agent_id, time_limit):
        
        # initialize the allowed time
        allocated_time = time.time() + time_limit - PADDING

        # initialize the best action and depth
        latest_action = None
        depth = 1

        # loop over expectimax with increasing depth in a try-except block
        try:
            while True:

                # check the timer
                if time.time() > allocated_time:
                    raise Exception("Time limit exceeded")
                
                # get the latest action and update the depth
                _, latest_action = expectimax(env, depth, True, agent_id, allocated_time)
                
                # increment the depth
                depth += 1

        except:
            return latest_action

# =================================== HardCoded ===================================

# here you can check specific paths to get to know the environment
class AgentHardCoded(Agent):
    def __init__(self):
        self.step = 0
        # specifiy the path you want to check - if a move is illegal - the agent will choose a random move
        self.trajectory = ["move north", "move east", "move north", "move north", "pick_up", "move east", "move east",
                           "move south", "move south", "move south", "move south", "drop_off"]

    def run_step(self, env: WarehouseEnv, robot_id, time_limit):
        if self.step == len(self.trajectory):
            return self.run_random_step(env, robot_id, time_limit)
        else:
            op = self.trajectory[self.step]
            if op not in env.get_legal_operators(robot_id):
                op = self.run_random_step(env, robot_id, time_limit)
            self.step += 1
            return op

    def run_random_step(self, env: WarehouseEnv, robot_id, time_limit):
        operators, _ = self.successors(env, robot_id)

        return random.choice(operators)