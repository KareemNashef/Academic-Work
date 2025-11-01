#include "Commands.h"

// ============================== BASELINE ==============================

const std::string WHITESPACE = " \n\r\t\f\v";

#if 0
#define FUNC_ENTRY()\
cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()\
cout << __PRETTY_FUNCTION__ << "<-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

std::string _ltrim(const std::string& s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string _rtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string _trim(const std::string& s)
{
	return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args)
{
	FUNC_ENTRY()
		int i = 0;

	std::istringstream iss(_trim(std::string(cmd_line)).c_str());
	for (std::string s; iss >> s;)
	{
		args[i] = (char*)malloc(s.length() + 1);
		if (args[i] == nullptr)
		{
			for (int j = 0; j < i; j++)
				free(args[j]);
			return -1;
		}

		memset(args[i], 0, s.length() + 1);
		strcpy(args[i], s.c_str());
		args[++i] = NULL;
	}

	return i;

	FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line)
{
	const std::string str(cmd_line);
	return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line)
{
	const std::string str(cmd_line);
	// find last character other than spaces
	unsigned int idx = str.find_last_not_of(WHITESPACE);
	// if all characters are spaces then return
	if (idx == std::string::npos)
	{
		return;
	}

	// if the command line does not end with &then return
	if (cmd_line[idx] != '&')
	{
		return;
	}

	// replace the &(background sign) with space and then remove all tailing spaces.
	cmd_line[idx] = ' ';
	// truncate the command line string up to the last non-space character
	cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// ============================== HELPING FUNCTIONS &VARIABLES ==============================

// GetArguments - Takes in a CommandLine and pointer to a number and returns the following arguments that are expected after this command
char** GetArguments(const char* CommandLine, int* ArgumentCount)
{
	// Allocating the arguments list and zeroing out the contents
	char** Arguments = (char**)malloc(COMMAND_MAX_ARGS * sizeof(char*));
	if (Arguments == nullptr)
		return nullptr;
	for (int i = 0; i < COMMAND_MAX_ARGS; i++)
		Arguments[i] = nullptr;

	// Sending the list and the command for parsing 
	*ArgumentCount = _parseCommandLine(CommandLine, Arguments);
	if (*ArgumentCount == -1)
	{
		free(Arguments);
		return nullptr;
	}

	return Arguments;
}

// FreeArguments - Frees the arguments created in GetArguments
void FreeArguments(char** Arguments, int ArgumentCount)
{
	// Freeing each argument
	for (int i = 0; i < ArgumentCount; i++)
	{
		if (Arguments[i])
			free(Arguments[i]);
	}

	free(Arguments);
}

// IsNumber - Takes a string and returns true if the string is a number
bool IsNumber(std::string inString) {
	std::string::const_iterator iter = inString.begin();
	bool first = true;
	while (iter != inString.end()) {

		if (first) {
			if (*iter == '-') {
				++iter;
				continue;
			}
		}
		if (std::isdigit(*iter) == false)
			break;

		++iter;
	}
	return !inString.empty() && iter == inString.end();
}

// ============================== GENERAL CLASSES ==============================

Command::Command(const char* cmd_line) :
	CommandLine(cmd_line) {}

const char* Command::GetCommand()
{
	return CommandLine;
}

BuiltInCommand::BuiltInCommand(const char* cmd_line) :
	Command(cmd_line)
{
	// Checking if the given command needs to be run in the background and removing the &sign
	if (_isBackgroundComamnd(cmd_line))
	{
		char CopyCommand[COMMAND_ARGS_MAX_LENGTH];
		strcpy(CopyCommand, CommandLine);
		_removeBackgroundSign(CopyCommand);
		CommandLine = CopyCommand;
	}
}

JobEntry::JobEntry(int inID, std::string inCMD, int inPID, time_t inTime, bool inStatus) :
	JobID(inID),
	JobCommand(inCMD),
	JobPID(inPID),
	CreationTime(inTime),
	RunStatus(inStatus) {}

JobsList::JobsList() :
	List(),
	MaxID(1) {}

void JobsList::addJob(Command* cmd, int inPID, bool RunStatus)
{
	// Removing the finished jobs from the list
	MainJobsList.removeFinishedJobs();

	List.push_back(JobEntry(MaxID, std::string(cmd->GetCommand()), inPID, time(nullptr), RunStatus));
	MaxID++;
}

void JobsList::killAllJobs()
{
	// Iterating over all the entries in the list and deleting them
	for (auto& Iter : List)
	{
		std::cout << Iter.JobPID << ": " << Iter.JobCommand << std::endl;
		kill(Iter.JobPID, SIGKILL);
	}
}

void JobsList::removeFinishedJobs()
{
	// Making sure that the list has elements
	if (List.size() == 0)
		return;

	// Iterating over all the entries in the list and deleting the dead ones
	for (auto Iter = List.begin(); Iter != List.end(); ++Iter)
	{
		int WaitStatus;
		int WaitReturn = waitpid((*Iter).JobPID, &WaitStatus, WNOHANG);
		if (WaitReturn == -1 || WaitReturn == (*Iter).JobPID)
		{
			List.erase(Iter);
			--Iter;
		}
	}

	// Adjusting the new max ID
	int NewMax = 0;
	for (auto& Iter : List)
	{
		if (NewMax < Iter.JobID)
			NewMax = Iter.JobID;
	}

	MaxID = NewMax + 1;
}

JobEntry* JobsList::getJobById(int jobId)
{
	// Removing the finished jobs from the list
	MainJobsList.removeFinishedJobs();

	for (auto& Iter : List)
	{
		if (Iter.JobID == jobId)
			return &Iter;
	}

	return nullptr;
}

JobEntry* JobsList::getJobByPid(int jobPid)
{
	// Removing the finished jobs from the list
	MainJobsList.removeFinishedJobs();

	for (auto& Iter : List)
	{
		if (Iter.JobPID == jobPid)
			return &Iter;
	}

	return nullptr;
}

void JobsList::removeJobById(int jobId)
{
	for (auto Iter = List.begin(); Iter != List.end(); ++Iter)
	{
		if ((*Iter).JobID == jobId)
		{
			List.erase(Iter);
			return;
		}
	}

	// Removing the finished jobs from the list
	MainJobsList.removeFinishedJobs();
}

JobEntry* JobsList::getLastJob()
{
	// Removing the finished jobs from the list
	MainJobsList.removeFinishedJobs();

	JobEntry* Last = nullptr;
	int Max = 0;

	for (auto& Iter : List)
	{
		if (Iter.JobID > Max)
		{
			Max = Iter.JobID;
			Last = &Iter;
		}
	}

	return Last;
}

JobEntry* JobsList::getLastStoppedJob()
{
	// Removing the finished jobs from the list
	MainJobsList.removeFinishedJobs();

	JobEntry* Last = nullptr;
	int Max = 0;

	for (auto& Iter : List)
	{
		if (Iter.JobID > Max && Iter.RunStatus == false)
		{
			Max = Iter.JobID;
			Last = &Iter;
		}
	}

	return Last;
}

// ============================== BUILT IN COMMANDS ==============================

// ===== chprompt =====
ChangePromptCommand::ChangePromptCommand(const char* cmd_line) :
	BuiltInCommand(cmd_line) {}

void ChangePromptCommand::execute()
{
	// Getting the following arguments and their count
	int ArgumentCount;
	char** Arguments = GetArguments(CommandLine, &ArgumentCount);
	if (Arguments == nullptr)
	{
		perror("smash error: malloc failed");
		return;
	}

	// Reverting\Replacing the smash prompt
	if (ArgumentCount == 1)
		SmashPrompt = "smash";
	else
		SmashPrompt = Arguments[1];

	// Freeing the used arguments
	FreeArguments(Arguments, ArgumentCount);
}

// ===== showpid =====
ShowPidCommand::ShowPidCommand(const char* cmd_line) :
	BuiltInCommand(cmd_line) {}

void ShowPidCommand::execute()
{
	std::cout << "smash pid is " << SmashPID << std::endl;
}

// ===== pwd =====
GetCurrDirCommand::GetCurrDirCommand(const char* cmd_line) :
	BuiltInCommand(cmd_line) {}

void GetCurrDirCommand::execute()
{
	// Getting the CWD and printing it
	char* CWD = (char*)malloc(sizeof(char) * PATH_MAX);
	getcwd(CWD, PATH_MAX);
	std::cout << CWD << std::endl;
	free(CWD);
}

// ===== cd =====
ChangeDirCommand::ChangeDirCommand(const char* cmd_line) :
	BuiltInCommand(cmd_line) {}

void ChangeDirCommand::execute()
{
	// Getting the following arguments and their count
	int ArgumentCount;
	char** Arguments = GetArguments(CommandLine, &ArgumentCount);
	if (Arguments == nullptr)
	{
		perror("smash error: malloc failed");
		return;
	}

	// Getting the CWD
	char* CWD = (char*)malloc(sizeof(char) * PATH_MAX);
	getcwd(CWD, PATH_MAX);

	// Checking the number and type of arguments
	if (ArgumentCount > 2)
	{
		std::cerr << "smash error: cd: too many arguments" << std::endl;
		free(CWD);
	}
	else if (strcmp(Arguments[1], "-") == 0)
	{
		// Checking if the last working directory exists
		if (LastDirectory == nullptr)
		{
			std::cerr << "smash error: cd: OLDPWD not set" << std::endl;
			free(CWD);
		}
		else
		{
			// Switching the directory
			if (chdir(LastDirectory) == SYS_FAIL)
			{
				perror("smash error: chdir failed");
				free(CWD);
			}
			else
			{
				free(LastDirectory);
				LastDirectory = CWD;
			}
		}
	}
	else
	{
		// Changing the directory
		if (chdir(Arguments[1]) == SYS_FAIL)
		{
			perror("smash error: chdir failed");
			free(CWD);
		}
		else
		{
			if (LastDirectory != nullptr)
				free(LastDirectory);
			
			LastDirectory = CWD;
		}
	}

	// Freeing the used arguments
	FreeArguments(Arguments, ArgumentCount);
}

// ===== jobs =====
JobsCommand::JobsCommand(const char* cmd_line) :
	BuiltInCommand(cmd_line) {}

void JobsCommand::execute()
{
	// Removing the finished jobs from the list
	MainJobsList.removeFinishedJobs();

	// Iterating over the list and printing out the jobs
	for (auto iter = MainJobsList.List.begin(); iter != MainJobsList.List.end(); ++iter)
	{
		JobEntry current = *iter;

		std::cout << "[" << current.JobID << "] " << current.JobCommand << " : " << current.JobPID << " " << difftime(time(nullptr), current.CreationTime) << " secs";

		if (current.RunStatus == true)
			std::cout << std::endl;
		else
			std::cout << " (stopped)" << std::endl;
	}
}

// ===== fg =====
ForegroundCommand::ForegroundCommand(const char* cmd_line) :
	BuiltInCommand(cmd_line) {}

void ForegroundCommand::execute()
{
	// Getting the following arguments and their count
	int ArgumentCount;
	char** Arguments = GetArguments(CommandLine, &ArgumentCount);
	if (Arguments == nullptr)
	{
		perror("smash error: malloc failed");
		return;
	}

	// Getting the wanted job
	JobEntry* Current;

	if (ArgumentCount == 1)
	{
		Current = MainJobsList.getLastJob();
		if (Current == nullptr)
		{
			std::cerr << "smash error: fg: jobs list is empty" << std::endl;
			FreeArguments(Arguments, ArgumentCount);
			return;
		}

	}
	else if (ArgumentCount == 2 && IsNumber(Arguments[1]))
	{
		Current = MainJobsList.getJobById(std::stoi(Arguments[1]));
		if (Current == nullptr)
		{
			std::cerr << "smash error: fg: job-id " << std::stoi(Arguments[1]) << " does not exist" << std::endl;
			FreeArguments(Arguments, ArgumentCount);
			return;
		}
	}
	else
	{
		std::cerr << "smash error: fg: invalid arguments" << std::endl;
		FreeArguments(Arguments, ArgumentCount);
		return;
	}
	
	// Checking if the job is stopped and continuing it
	if (Current->RunStatus == false)
	{
		if (kill(Current->JobPID, SIGCONT) == SYS_FAIL)
		{
			perror("smash error: kill failed");
			FreeArguments(Arguments, ArgumentCount);
			return;
		}
	}

	// Sending the data to the global variables
	CurrentJobPID = Current->JobPID;
	CurrentCommand = Current->JobCommand;
	
 	// Printing the job and its PID
	std::cout << CurrentCommand << " : " << CurrentJobPID << std::endl;

	// Waiting for the job to finish
	int WaitStatus;
	if (waitpid(CurrentJobPID, &WaitStatus, WUNTRACED) == SYS_FAIL)
	{
		perror("smash error: waitpid failed");
	}

	// Resetting the data to the global variables
	CurrentJobPID = -1;
	CurrentCommand = "";

	FreeArguments(Arguments, ArgumentCount);
}

// ===== bg =====
BackgroundCommand::BackgroundCommand(const char* cmd_line) :
	BuiltInCommand(cmd_line) {}

void BackgroundCommand::execute()
{
	// Getting the following arguments and their count
	int ArgumentCount;
	char** Arguments = GetArguments(CommandLine, &ArgumentCount);
	if (Arguments == nullptr)
	{
		perror("smash error: malloc failed");
		return;
	}

	// Getting the wanted job
	JobEntry* Current;

	if (ArgumentCount == 1)
	{
		Current = MainJobsList.getLastStoppedJob();
		if (Current == nullptr)
		{
			std::cerr << "smash error: bg: there is no stopped jobs to resume" << std::endl;
			FreeArguments(Arguments, ArgumentCount);
			return;
		}
	}
	else if (ArgumentCount == 2 && IsNumber(Arguments[1]))
	{
		Current = MainJobsList.getJobById(std::stoi(Arguments[1]));
		if (Current == nullptr)
		{
			std::cerr << "smash error: bg: job-id " << std::stoi(Arguments[1]) << " does not exist" << std::endl;
			FreeArguments(Arguments, ArgumentCount);
			return;
		}
		else if (Current->RunStatus == true)
		{
			std::cerr << "smash error: bg: job-id " << std::stoi(Arguments[1]) << " is already running in the background" << std::endl;
			FreeArguments(Arguments, ArgumentCount);
			return;
		}
	}
	else
	{
		std::cerr << "smash error: bg: invalid arguments" << std::endl;
		FreeArguments(Arguments, ArgumentCount);
		return;
	}

	// Continuing the stopped job and adjusting the status
	if (kill(Current->JobPID, SIGCONT) == SYS_FAIL)
	{
		perror("smash error: kill failed");
		FreeArguments(Arguments, ArgumentCount);
		return;
	}

	Current->RunStatus = true;

	// Printing the job and its PID
	std::cout << Current->JobCommand << " : " << Current->JobPID << std::endl;

	FreeArguments(Arguments, ArgumentCount);
}

// ===== quit =====
QuitCommand::QuitCommand(const char* cmd_line) :
	BuiltInCommand(cmd_line) {}

void QuitCommand::execute()
{
	// Getting the following arguments and their count
	int ArgumentCount;
	char** Arguments = GetArguments(CommandLine, &ArgumentCount);
	if (Arguments == nullptr)
	{
		perror("smash error: malloc failed");
		return;
	}

	// Removing the finished jobs from the list
	MainJobsList.removeFinishedJobs();

	// Checking for the kill argument
	if (ArgumentCount >= 2)
	{
		if (strcmp(Arguments[1], "kill") == 0)
		{
			std::cout << "smash: sending SIGKILL signal to " << MainJobsList.List.size() << " jobs:" << std::endl;
			MainJobsList.killAllJobs();
		}
	}

	FreeArguments(Arguments, ArgumentCount);
	exit(0);
}

// ===== kill =====
KillCommand::KillCommand(const char* cmd_line) :
	BuiltInCommand(cmd_line) {}

void KillCommand::execute()
{
	// Getting the following arguments and their count
	int ArgumentCount;
	char** Arguments = GetArguments(CommandLine, &ArgumentCount);
	if (Arguments == nullptr)
	{
		perror("smash error: malloc failed");
		return;
	}

	// Checking the arguments validity
	if (ArgumentCount != 3 || (std::string(Arguments[1]).at(0) != '-') || IsNumber(Arguments[1]) == false || IsNumber(Arguments[2]) == false)
	{
		std::cerr << "smash error: kill: invalid arguments" << std::endl;
		FreeArguments(Arguments, ArgumentCount);
		return;
	}

	// Checking if we got a valid signum
	if (std::stoi(Arguments[1] + 1) > 31 || std::stoi(Arguments[1] + 1) < 1)
	{
		std::cerr << "smash error: kill: invalid arguments" << std::endl;
		FreeArguments(Arguments, ArgumentCount);
		return;
	}


	// Getting the job entry and making sure the id exists
	JobEntry* Current = MainJobsList.getJobById(std::stoi(Arguments[2]));
	if (Current == nullptr)
	{
		std::cerr << "smash error: kill: job-id " << std::stoi(Arguments[2]) << " does not exist" << std::endl;
		FreeArguments(Arguments, ArgumentCount);
		return;
	}

	// Getting the signal number and sending it to the job
	if (kill(Current->JobPID, std::stoi(Arguments[1] + 1)) == SYS_FAIL)
	{
		perror("smash error: kill failed");
		FreeArguments(Arguments, ArgumentCount);
		return;
	}

	std::cout << "signal number " << std::stoi(Arguments[1] + 1) << " was sent to pid " << Current->JobPID << std::endl;

	// Adjusting the job status
	if (std::stoi(Arguments[1] + 1) == SIGCONT)
		Current->RunStatus = true;
	if (std::stoi(Arguments[1] + 1) == SIGSTOP)
		Current->RunStatus = false;

	FreeArguments(Arguments, ArgumentCount);
}

// ============================== EXTERNAL COMMANDS ==============================

ExternalCommand::ExternalCommand(const char* cmd_line) :
	Command(cmd_line) {}

void ExternalCommand::execute()
{
	// Trimming the command
	char GivenCommand[COMMAND_ARGS_MAX_LENGTH];
	strcpy(GivenCommand, _trim(CommandLine).c_str());

	// Checking if the command needs to be run in the background
	bool Background = _isBackgroundComamnd(GivenCommand);
	if (Background)
		_removeBackgroundSign(GivenCommand);

	// Forking
	int PID = fork();

	// Child process
	if (PID == 0)
	{
		// Changing the group ID
		if (setpgrp() == SYS_FAIL)
		{
			perror("smash error: setpgrp failed");
			return;
		}

		// Executing the commands
		char* Args[] = { (char*)"/bin/bash",(char*)"-c", GivenCommand, nullptr };

		if (execv("/bin/bash", Args) == SYS_FAIL)
		{
			perror("smash error: execv failed");
			return;
		}
	}

	// Parent process
	else if (Background == true)
	{
		// Adding the job to the list and running it in the background
		MainJobsList.addJob(this, PID,true);
	}
	else
	{
		// Sending the data to the global variables
		CurrentJobPID = PID;
		CurrentCommand = CommandLine;

		// Waiting for the child to finish
		int WaitStatus;
		if (waitpid(PID, &WaitStatus, WUNTRACED) == SYS_FAIL)
		{
			perror("smash error: waitpid failed");
			return;
		}
	}
}

// ============================== SPECIAL COMMANDS ==============================

// ===== pipe =====
PipeCommand::PipeCommand(const char* cmd_line) :
	Command(cmd_line) {}

void PipeCommand::execute()
{
	// Parsing the commands and pipe character
	std::string Command1, PipeChar, Command2;

	if (std::string(CommandLine).find("|&") != std::string::npos)
		PipeChar = "|&";
	else
		PipeChar = "|";

	Command1 = std::string(CommandLine).substr(0, std::string(CommandLine).find(PipeChar));
	Command2 = std::string(CommandLine).substr(std::string(CommandLine).find(PipeChar) + PipeChar.length(), std::string(CommandLine).length());

	Command1 = _trim(Command1);
	Command2 = _trim(Command2);
	
	// Creating the pipe
	int PipeDescriptor[2];
	pipe(PipeDescriptor);

	// Creating the first command process
	int FirstChild = fork();
	if (FirstChild == SYS_FAIL)
	{
		perror("smash error: fork failed");
		if (close(PipeDescriptor[0]) == SYS_FAIL || close(PipeDescriptor[1]) == SYS_FAIL)
			perror("smash error: close failed");
		return;
	}

	// Configuring the first command's channels
	if (FirstChild == 0)
	{
		// Changing the group ID
		if (setpgrp() == SYS_FAIL)
		{
			perror("smash error: setpgrp failed");
			if (close(PipeDescriptor[0]) == SYS_FAIL || close(PipeDescriptor[1]) == SYS_FAIL)
				perror("smash error: close failed");
			return;
		}

		// Checking which pipe character we got
		if (PipeChar == "|")
		{
			// Switching the display channel
			if (dup2(PipeDescriptor[1], 1) == SYS_FAIL)
			{
				perror("smash error: dup2 failed");
				if (close(PipeDescriptor[0]) == SYS_FAIL || close(PipeDescriptor[1]) == SYS_FAIL)
					perror("smash error: close failed");
				return;
			}
		}
		else
		{
			// Switching the error channel
			if (dup2(PipeDescriptor[1], 2) == SYS_FAIL)
			{
				perror("smash error: dup2 failed");
				if (close(PipeDescriptor[0]) == SYS_FAIL || close(PipeDescriptor[1]) == SYS_FAIL)
					perror("smash error: close failed");
				return;
			}
		}

		// Closing the channels after switching
		if (close(PipeDescriptor[0]) == SYS_FAIL || close(PipeDescriptor[1]) == SYS_FAIL)
			perror("smash error: close failed");

		// Executing the first command and exiting
		executeCommand(Command1.c_str());
		exit(0);
	}

	// Creating the first command process
	int SecondChild = fork();
	if (SecondChild == SYS_FAIL)
	{
		perror("smash error: fork failed");
		if (close(PipeDescriptor[0]) == SYS_FAIL || close(PipeDescriptor[1]) == SYS_FAIL)
			perror("smash error: close failed");
		return;
	}

	// Configuring the second command's channels
	if (SecondChild == 0)
	{
		// Changing the group ID
		if (setpgrp() == SYS_FAIL)
		{
			perror("smash error: setpgrp failed");
			if (close(PipeDescriptor[0]) == SYS_FAIL || close(PipeDescriptor[1]) == SYS_FAIL)
				perror("smash error: close failed");
			return;
		}

		// Switching the read channel
		if (dup2(PipeDescriptor[0], 0) == SYS_FAIL)
		{
			perror("smash error: dup2 failed");
			if (close(PipeDescriptor[0]) == SYS_FAIL || close(PipeDescriptor[1]) == SYS_FAIL)
				perror("smash error: close failed");
			return;
		}

		// Closing the channels after switching
		if (close(PipeDescriptor[0]) == SYS_FAIL || close(PipeDescriptor[1]) == SYS_FAIL)
			perror("smash error: close failed");

		// Executing the first command and exiting
		executeCommand(Command2.c_str());
		exit(0);
	}

	// Closing the channels in the parent process
	if (close(PipeDescriptor[0]) == SYS_FAIL || close(PipeDescriptor[1]) == SYS_FAIL)
		perror("smash error: close failed");

	// Waiting for the children to finish
	if (waitpid(FirstChild, nullptr, WUNTRACED) == SYS_FAIL || waitpid(SecondChild, nullptr, WUNTRACED) == SYS_FAIL)
		perror("smash error: waitpid failed");
}

// ===== redirection =====
RedirectionCommand::RedirectionCommand(const char* cmd_line) :
	Command(cmd_line) {}

void RedirectionCommand::execute()
{
	// Parsing the commands and redirection character
	std::string Command1, RedirectionChar, Command2;

	if (std::string(CommandLine).find(">>") != std::string::npos)
		RedirectionChar = ">>";
	else
		RedirectionChar = ">";

	Command1 = std::string(CommandLine).substr(0, std::string(CommandLine).find(RedirectionChar));
	Command2 = std::string(CommandLine).substr(std::string(CommandLine).find(RedirectionChar) + RedirectionChar.length(), std::string(CommandLine).length());

	Command1 = _trim(Command1);
	Command2 = _trim(Command2);

	// Closing the display output
	int STDout = dup(1);

	if (close(1) == SYS_FAIL)
	{
		perror("smash error: close failed");
		return;
	}

	// Checking if we need to append and redirecting the output
	int Fileout;
	if (RedirectionChar.length() == 1)
		Fileout = open(Command2.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0655);
		
	else
		Fileout = open(Command2.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0655);

	if (Fileout == SYS_FAIL)
	{
		perror("smash error: open failed");

		if (dup2(STDout, 1) == SYS_FAIL)
			perror("smash error: dup2 failed");

		return;
	}

	// Executing the command with the redirected output
	executeCommand(Command1.c_str());

	// Closing the file output
	if (close(Fileout) == SYS_FAIL)
		perror("smash error: close failed");

	// Redirecting the output back to the display
	if (dup2(STDout, 1) == SYS_FAIL)
		perror("smash error: dup2 failed");

	if (close(STDout) == SYS_FAIL)
		perror("smash error: close failed");
}

// ===== fare =====
FareCommand::FareCommand(const char* cmd_line) :
	BuiltInCommand(cmd_line) {}

void FareCommand::execute()
{
	// Getting the following arguments and their count
	int ArgumentCount;
	char** Arguments = GetArguments(CommandLine, &ArgumentCount);
	if (Arguments == nullptr)
	{
		perror("smash error: malloc failed");
		return;
	}

	// Checking the arguments validity
	if (ArgumentCount != 4)
	{
		std::cerr << "smash error: fare: invalid arguments" << std::endl;
		FreeArguments(Arguments, ArgumentCount);
		return;
	}

		// Opening the file that we need to work on and temporary file to write in
	std::ifstream InputFile(Arguments[1]);
	if (!InputFile) {
		std::cerr << "smash error: open failed: No such file or directory" << std::endl;
		FreeArguments(Arguments, ArgumentCount);
		return;
	}
	std::ofstream OutputFile("TempFile");

	// Simplifying the source and destination for readability
	std::string ToFind = Arguments[2];
	std::string Replacement = Arguments[3];

	std::string CurrentLine;
	int Length1 = ToFind.length();
	int Length2 = Replacement.length();
	int Instances = 0;
	int PrevPosition = 0;

	// Replacing every occurence of toFind with Replacement and storing the output in a temp file
	while (getline(InputFile, CurrentLine)) {
		while (true) {

			// Getting the correct position in the line and ignoring previously changed strings
			size_t Position = (CurrentLine.substr(PrevPosition)).find(ToFind);
			
			if (Position != std::string::npos) {
				Position += PrevPosition;
				CurrentLine.replace(Position, Length1, Replacement);
				Instances++;
				PrevPosition = Position + Length2;
			}
			else break;
		}

		OutputFile << CurrentLine << '\n';
	}

	// Deleting the previous file
	remove(Arguments[1]);

	// Renaming the output file
	rename("TempFile", Arguments[1]);

	// Printing the result
	std::cout << "replaced " << Instances << " instances of the string \"" << Arguments[2] << "\"" << std::endl;

	FreeArguments(Arguments, ArgumentCount);
}

// ===== setcore =====

// ===== timeout =====

// ============================== SMASH IMPLEMENTATION ==============================

Command* CreateCommand(const char* cmd_line)
{
	// Trimming the input string and getting the first word
	std::string inCommand = _trim(std::string(cmd_line));
	std::string FirstWord = inCommand.substr(0, inCommand.find_first_of(" \n"));

	// Comparing the first word to the list of valid commands
	if (inCommand.find(">") != std::string::npos || inCommand.find(">>") != std::string::npos)
	{
		return new RedirectionCommand(cmd_line);
	}
	else if (inCommand.find("|") != std::string::npos || inCommand.find("|&") != std::string::npos)
	{
		return new PipeCommand(cmd_line);
	}
	else if (FirstWord == "chprompt")
	{
		return new ChangePromptCommand(cmd_line);
	}
	else if (FirstWord == "showpid")
	{
		return new ShowPidCommand(cmd_line);
	}
	else if (FirstWord == "pwd")
	{
		return new GetCurrDirCommand(cmd_line);
	}
	else if (FirstWord == "cd")
	{
		return new ChangeDirCommand(cmd_line);
	}
	else if (FirstWord == "jobs")
	{
		return new JobsCommand(cmd_line);
	}
	else if (FirstWord == "fg")
	{
		return new ForegroundCommand(cmd_line);
	}
	else if (FirstWord == "bg")
	{
		return new BackgroundCommand(cmd_line);
	}
	else if (FirstWord == "quit")
	{
		return new QuitCommand(cmd_line);
	}
	else if (FirstWord == "kill")
	{
		return new KillCommand(cmd_line);
	}
	else if (FirstWord == "fare")
	{
		return new FareCommand(cmd_line);
	}
	else
	{
		return new ExternalCommand(cmd_line);
	}
}

void executeCommand(const char* cmd_line)

{
	// Creating the appropriate command and executing it
	Command* CMD = CreateCommand(cmd_line);
	CMD->execute();
	delete CMD;

	// Reseting the current data in case it was changed
	CurrentJobPID = -1;
	CurrentCommand = "";
}

SmallShell::SmallShell()
{
	SmashPrompt = "smash";
	SmashPID = getppid();
	LastDirectory = nullptr;
	CurrentJobPID = -1;
}

SmallShell::~SmallShell() {}

// ===== Global Variables =====

// SmashPrompt - Saves the current smash prompt
std::string SmashPrompt = "";

// SmashPrompt - Saves the current smash pid
int SmashPID = -1;

// LastDirectory - Saves the last directory when - was used
char* LastDirectory = nullptr;

// CurrentJobPID - Saves the PID of the currently running job
int CurrentJobPID = -1;

// CurrentCommand - Saves the command of the currently running job
std::string CurrentCommand = "";

// MainJobsList - Saves the current jobs list
JobsList MainJobsList = JobsList();
