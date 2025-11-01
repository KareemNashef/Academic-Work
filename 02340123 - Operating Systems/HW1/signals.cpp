#include "signals.h"
using namespace std;

// ============================== SIGNALS HANDLING ==============================

void ctrlZHandler(int sig_num)
{
	// Printing the event
	cout << "smash: got ctrl-Z" << endl;

	// Checking if there's a job that is currently running
	if (CurrentJobPID != -1)
	{
		// Checking if the job already exists in the JobList
		JobEntry* toStop = MainJobsList.getJobByPid(CurrentJobPID);
		if (toStop == nullptr) {
			// Adding the job to the job list and setting it as stopped
			MainJobsList.addJob(CreateCommand(CurrentCommand.c_str()), CurrentJobPID, false);
		}
		else {
			// Adjusting the run status of the job
			toStop->RunStatus = false;
		}
		

		// Sending the Stop signal to the process
		if (kill(CurrentJobPID, SIGSTOP))
		{
			perror("smash error: kill failed");
			return;
		}

		// Printing the result of the kill
		cout << "smash: process " << CurrentJobPID << " was stopped" << endl;

		// Resetting the global variables
		CurrentJobPID = -1;
		CurrentCommand = "";
	}
}

void ctrlCHandler(int sig_num)
{
	// Printing the event
	cout << "smash: got ctrl-C" << endl;

	// Checking if there's a job that is currently running
	if (CurrentJobPID != -1)
	{
		// Sending the Kill signal to the process
		if (kill(CurrentJobPID, SIGINT))
		{
			perror("smash error: kill failed");
			return;
		}

		// Printing the result of the kill
		cout << "smash: process " << CurrentJobPID << " was killed" << endl;

		// Resetting the global variables
		CurrentJobPID = -1;
		CurrentCommand = "";
	}
}

void alarmHandler(int sig_num)
{
	// TODO: Add your implementation
}