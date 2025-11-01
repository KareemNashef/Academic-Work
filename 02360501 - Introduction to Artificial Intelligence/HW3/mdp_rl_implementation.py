from mdp import Action, MDP
from simulator import Simulator
from typing import Dict, List, Tuple
import numpy as np


def value_iteration(mdp, U_init, epsilon=10 ** (-3)):
    # Given the mdp, the initial utility of each state - U_init,
    #   and the upper limit - epsilon.
    # run the value iteration algorithm and
    # return: the utility for each of the MDP's state obtained at the end of the algorithms' run.

    U_final = [row.copy() for row in U_init]
    U_prime = [row.copy() for row in U_init]
    delta = float('inf')

    # Algorithm's main loop
    while delta >= epsilon * ((1-mdp.gamma) / mdp.gamma):

        U_final = [row.copy() for row in U_prime]
        delta = 0

        # For each state in the MDP
        for r in range(mdp.num_row):
            for c in range(mdp.num_col):
                state = (r, c)

                # Skip terminal states and walls
                if mdp.board[r][c] == "WALL" or state in mdp.terminal_states:

                    # Set the utility for terminal states
                    if state in mdp.terminal_states:
                        U_prime[r][c] = float(mdp.get_reward(state))

                    continue

                # Calculate the max action-value for this state
                max_utility = float('-inf')
                for action in mdp.actions.keys():
                    
                    # Get the action's transition probabilities
                    transition_probabilities = mdp.transition_function[action]
                    action_utility = 0

                    # Calculate the expected utility for this action
                    for action_index, action_probability in enumerate(transition_probabilities):

                        # Get the next state
                        current_action = list(Action)[action_index]
                        next_state = mdp.step(state, current_action)

                        # Calculate the expected utility
                        action_utility += action_probability * U_final[next_state[0]][next_state[1]]

                    # Update the max utility
                    max_utility = max(max_utility, action_utility)

                # Calculate the new utility for this state
                U_prime[r][c] = float(mdp.get_reward(state)) + mdp.gamma * max_utility

                # Update delta
                delta = max(delta, abs(U_prime[r][c] - U_final[r][c]))
    
    # Return the final utility
    return U_final


def get_policy(mdp, U):
    # Given the mdp and the utility of each state - U (which satisfies the Belman equation)
    # return: the policy

    # Initialize the policy
    policy = [[None for _ in range(mdp.num_col)] for _ in range(mdp.num_row)]

    # For each state in the MDP
    for r in range(mdp.num_row):
        for c in range(mdp.num_col):
            state = (r, c)

            # Skip terminal states and walls
            if mdp.board[r][c] == "WALL" or state in mdp.terminal_states:
                policy[r][c] = None
                continue

            # Initialize the best action and max utility
            best_action = None
            max_utility = float('-inf')

            # For each action in the MDP
            for action in mdp.actions.keys():
                
                # Get the action's transition probabilities
                transition_probabilities = mdp.transition_function[action]
                action_utility = 0

                # Calculate the expected utility for this action
                for action_index, action_probability in enumerate(transition_probabilities):

                    # Get the next state
                    current_action = list(Action)[action_index]
                    next_state = mdp.step(state, current_action)

                    # Calculate the expected utility
                    action_utility += action_probability * U[next_state[0]][next_state[1]]

                # Update the best action and max utility
                if action_utility > max_utility:
                    max_utility = action_utility
                    best_action = action

            # Update the policy
            policy[r][c] = best_action

    # Return the policy
    return policy


def policy_evaluation(mdp, policy):
    # Given the mdp, and a policy
    # return: the utility U(s) of each state s
    
    # U = (I-gamma*P)^-1 * R

    num_states = mdp.num_row * mdp.num_col
    U = np.zeros(num_states)                    # Utility vector
    I = np.eye(num_states)                      # Identity matrix
    P = np.zeros((num_states, num_states))      # Transition probability matrix
    R = np.zeros(num_states)                    # Reward vector

    # For each state in the MDP
    for r in range(mdp.num_row):
        for c in range(mdp.num_col):
            state = (r, c)
            state_index = r * mdp.num_col + c

            # Skip walls
            if mdp.board[r][c] == "WALL":
                continue  

            # Handle terminal states
            if state in mdp.terminal_states:
                U[state_index] = mdp.get_reward(state)
                R[state_index] = mdp.get_reward(state)
                continue

            # Set the reward for this state
            R[state_index] = mdp.get_reward(state)

            # Get the action for this state
            action = policy[r][c]
            if isinstance(action, str):
                action = Action[action]
            
            # Get the transition probabilities for this action
            transition_probabilities = mdp.transition_function[action]
            for action_index, action_probability in enumerate(transition_probabilities):

                # Get the next state and its index
                current_action = list(Action)[action_index]
                next_state = mdp.step(state, current_action)
                next_state_index = next_state[0] * mdp.num_col + next_state[1]

                # Update the transition probability matrix
                P[state_index][next_state_index] += action_probability

    # Applying the formula U = (I-gamma*P)^-1 * R
    U = np.linalg.inv(I - mdp.gamma * P).dot(R)
    U = np.reshape(U, (mdp.num_row, mdp.num_col))
    
    # Return the utility
    return U


def policy_iteration(mdp, policy_init):
    # Given the mdp, and the initial policy - policy_init
    # run the policy iteration algorithm
    # return: the optimal policy
    
    # Initialize the policy
    optimal_policy = policy_init

    # Algorithm's main loop
    while True:
        
        # Update the utility
        U = policy_evaluation(mdp, optimal_policy)
        unchanged = True

        # For each state in the MDP
        for r in range(mdp.num_row):
            for c in range(mdp.num_col):
                state = (r, c)

                # Skip terminal states and walls
                if mdp.board[r][c] == "WALL" or state in mdp.terminal_states:
                    continue

                # Get the max action-value for this state
                best_action = None
                max_utility = float('-inf')

                for action in mdp.actions.keys():

                    action_utility = 0
                    transition_probabilities = mdp.transition_function[action]

                    # Calculate the expected utility for this action
                    for action_index, action_probability in enumerate(transition_probabilities):

                        # Get the next state
                        current_action = list(Action)[action_index]
                        next_state = mdp.step(state, current_action)

                        # Calculate the expected utility
                        action_utility += action_probability * U[next_state[0]][next_state[1]]

                    # Update the best action and max utility
                    if action_utility > max_utility:
                        max_utility = action_utility
                        best_action = action

                # Find the current value of the current policy
                action = optimal_policy[r][c]
                if isinstance(action, str):
                    action = Action[action]
                
                # Calculate the current utility
                transition_probabilities = mdp.transition_function[action]
                current_utility = 0
                for action_index, action_probability in enumerate(transition_probabilities):

                    # Get the next state
                    current_action = list(Action)[action_index]
                    next_state = mdp.step(state, current_action)

                    # Calculate the expected utility
                    current_utility += action_probability * U[next_state[0]][next_state[1]]
                
                # Update the policy
                if current_utility < max_utility:
                    optimal_policy[r][c] = best_action
                    unchanged = False

        # If the policy hasn't changed, break
        if unchanged:
            break

    # Return the optimal policy
    return optimal_policy


def mc_algorithm(
        sim,
        num_episodes,
        gamma,
        num_rows=3,
        num_cols=4,
        actions=[Action.UP, Action.DOWN, Action.LEFT, Action.RIGHT],
        policy=None,
):
    # Given a simulator, the number of episodes to run, the number of rows and columns in the MDP, the possible actions,
    # and an optional policy, run the Monte Carlo algorithm to estimate the utility of each state.
    # Return the utility of each state.

    # Initialize the value function and state visit counts
    value_function = np.zeros((num_rows, num_cols))
    state_visits = np.zeros((num_rows, num_cols))
    Returns = np.zeros((num_rows, num_cols))

    # Generate and iterate over the episodes
    for _, episode_gen in enumerate(sim.replay(num_episodes)):

        # Initialize the discounted return and the visited states
        G = 0
        first_visit = {}

        # Turn the episode generator into a steps list
        steps = [(index, step) for index, step in enumerate(episode_gen)]

        # Find the first visit of each state
        for step_index, step in steps:
            state, reward, _, _ = step
            if state not in first_visit:
                first_visit[state] = step_index
        
        # Iterate over the steps in the episode in reverse order
        for step_index, step in reversed(steps):

            # Unpack the step
            state, reward, _, _ = step
            
            G = gamma * G + reward
            
            # Skip if the step if this is not the first visit
            if first_visit[state] != step_index:
                continue
            
            # Update the number of visits to the state
            state_visits[state] += 1

            # Update the episode return
            Returns[state] += G

            # Update the value function
            value_function[state] = Returns[state] / state_visits[state]

    # Return the value function
    return value_function
