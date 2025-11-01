#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>
#include <fstream>

#define COMMAND_ARGS_MAX_LENGTH 200
#define COMMAND_MAX_ARGS 20
#define SYS_FAIL -1

// ============================== GENERAL CLASSES ==============================

class Command
{
protected:
	const char* CommandLine;
public:
	Command(const char* cmd_line);
	virtual~Command() = default;
	virtual void execute() = 0;

	const char* GetCommand();
};

class BuiltInCommand : public Command
{
public: BuiltInCommand(const char* cmd_line);
	  virtual~BuiltInCommand() {}
};

class JobEntry
{
public:
	int JobID;
	std::string JobCommand;
	int JobPID;
	time_t CreationTime;
	bool RunStatus;

	JobEntry(int inID, std::string inCMD, int inPID, time_t inTime, bool inStatus);
};

class JobsList
{
public:

	std::vector<JobEntry> List;
	int MaxID;

	JobsList();
	~JobsList() = default;
	void addJob(Command* cmd, int inPID, bool RunStatus);
	void killAllJobs();
	void removeFinishedJobs();
	JobEntry* getJobById(int jobId);
	JobEntry* getJobByPid(int jobId);
	void removeJobById(int jobId);
	JobEntry* getLastJob();
	JobEntry* getLastStoppedJob();
	// TODO: Add extra methods or modify exisitng ones as needed
};

// ============================== BUILT IN COMMANDS ==============================

// ===== chprompt =====
class ChangePromptCommand : public BuiltInCommand
{
public: ChangePromptCommand(const char* cmd_line);
	  virtual~ChangePromptCommand() {}

	  void execute() override;
};

// ===== showpid =====
class ShowPidCommand : public BuiltInCommand
{
public: ShowPidCommand(const char* cmd_line);
	  virtual~ShowPidCommand() {}

	  void execute() override;
};

// ===== pwd =====
class GetCurrDirCommand : public BuiltInCommand
{
public: GetCurrDirCommand(const char* cmd_line);
	  virtual~GetCurrDirCommand() {}

	  void execute() override;
};

// ===== cd =====
class ChangeDirCommand : public BuiltInCommand
{
public: ChangeDirCommand(const char* cmd_line);
	  virtual~ChangeDirCommand() {}

	  void execute() override;
};

// ===== jobs =====
class JobsCommand : public BuiltInCommand
{
	// TODO: Add your data members
public: JobsCommand(const char* cmd_line);
	  virtual~JobsCommand() {}

	  void execute() override;
};

// ===== fg =====
class ForegroundCommand : public BuiltInCommand
{
	// TODO: Add your data members
public: ForegroundCommand(const char* cmd_line);
	  virtual~ForegroundCommand() {}

	  void execute() override;
};

// ===== bg =====
class BackgroundCommand : public BuiltInCommand
{
	// TODO: Add your data members
public: BackgroundCommand(const char* cmd_line);
	  virtual~BackgroundCommand() {}

	  void execute() override;
};

// ===== quit =====
class QuitCommand : public BuiltInCommand
{
public: QuitCommand(const char* cmd_line);
	  virtual~QuitCommand() {}

	  void execute() override;
};

// ===== kill =====
class KillCommand : public BuiltInCommand
{
public: KillCommand(const char* cmd_line);
	  virtual~KillCommand() {}

	  void execute() override;
};

// ============================== EXTERNAL COMMANDS ==============================

class ExternalCommand : public Command
{
public: ExternalCommand(const char* cmd_line);
	  virtual~ExternalCommand() {}

	  void execute() override;
};

// ============================== SPECIAL COMMANDS ==============================

// ===== pipe =====
class PipeCommand : public Command
{
	// TODO: Add your data members
public: PipeCommand(const char* cmd_line);
	  virtual~PipeCommand() {}

	  void execute() override;
};

// ===== redirection =====
class RedirectionCommand : public Command
{
	// TODO: Add your data members
public: explicit RedirectionCommand(const char* cmd_line);
	  virtual~RedirectionCommand() {}

	  void execute() override;
};

// ===== fare =====
class FareCommand : public BuiltInCommand
{
	/*Optional */
	// TODO: Add your data members
public: FareCommand(const char* cmd_line);
	  virtual~FareCommand() {}

	  void execute() override;
};

// ===== setcore =====
class SetcoreCommand : public BuiltInCommand
{
	/*Optional */
	// TODO: Add your data members
public: SetcoreCommand(const char* cmd_line);
	  virtual~SetcoreCommand() {}

	  void execute() override;
};

// ===== timeout =====
class TimeoutCommand : public BuiltInCommand
{
	/*Optional */
	// TODO: Add your data members
public: explicit TimeoutCommand(const char* cmd_line);
	  virtual~TimeoutCommand() {}

	  void execute() override;
};

// ============================== SMASH IMPLEMENTATION ==============================

Command* CreateCommand(const char* cmd_line);

void executeCommand(const char* cmd_line);

class SmallShell
{
public:
	SmallShell();
	~SmallShell();
};

// ===== Global Variables =====

// SmashPrompt - Saves the current smash prompt
extern std::string SmashPrompt;

// SmashPrompt - Saves the current smash pid
extern int SmashPID;

// LastDirectory - Saves the last directory when - was used
extern char* LastDirectory;

// CurrentJobPID - Saves the PID of the currently running job
extern int CurrentJobPID;

// CurrentCommand - Saves the command of the currently running job
extern std::string CurrentCommand;

// MainJobsList - Saves the current jobs list
extern JobsList MainJobsList;


#endif	//SMASH_COMMAND_H_