/*
 * main.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#include "Agent.h"
#include "Configuration.h"
#include "Logger.h"

#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

#include <string>

extern const char* __progname;

static
struct option sLongOptions[] = {
		{ "conf", required_argument, 0, 'c' },
		{ "server", required_argument, 0, 's' },
		{ "local", required_argument, 0, 'l' },
		{ "stdout", no_argument, 0, 0 },
		{ "tag", required_argument, 0, 't' },
		{ "nosoftware", no_argument, 0, 0 },
		{ "daemonize", no_argument, 0, 'd' },
		{ "wait", required_argument, 0, 'w' },
		{ "help", no_argument, 0, 'h' },
		{ "logger", required_argument, 0, 0 },
		{ "verbose", no_argument, 0, 'v' },
		{ "version", no_argument, 0, 0 },
		{ "agent-string", required_argument, 0, 0 },
		{ "use-current-time-in-device-ID", no_argument, 0, 0 },
		{ 0, 0, 0, 0 }
};


static void
PrintHelpAndExit()
{
	std::cout << __progname << " " << Agent::Version() << std::endl;
	if (geteuid() != 0) {
		std::cout << "WARNING: This program should be run as root." << std::endl;
		std::cout << std::endl;
	}
	std::cout << "Usage:" << std::endl;
	std::cout << "  -h, --help                         Print usage" << std::endl;
	std::cout << "  -c, --conf <config_file>           Specify configuration file" << std::endl;
	std::cout << "  -s, --server <server>              Specify OCSInventory server url" << std::endl;
	std::cout << "                                     If the server needs authentication, use the standard syntax <user>:<password>@<host>" << std::endl;
	std::cout << "  -l, --local <folder>               Don't send inventory, instead save a local copy in the specified file or folder" << std::endl;
	std::cout << "      --stdout                       Don't send inventory, print it to stdout" << std::endl;
	std::cout << std::endl;
	std::cout << "  -t, --tag <TAG>                    Specify tag. Will be ignored by server if a value already exists" << std::endl;
	std::cout << "      --nosoftware                   Do not retrieve installed software" << std::endl;
	std::cout << std::endl;
	std::cout << "      --agent-string <string>        Specify custom HTTP agent string" << std::endl;
	std::cout << std::endl;
	std::cout << "  -d, --daemonize                    Detach from running terminal" << std::endl;
	std::cout << "  -w, --wait <s>                     Wait for the specified amount of seconds before building the inventory" << std::endl;
	std::cout << std::endl;
	std::cout << "      --logger <backend>             Specify error log backend (STDERR / SYSLOG)." << std::endl;
	std::cout << "                                     Default is standard error if attached to a terminal, otherwise syslog. " << std::endl;
	std::cout << "  -v, --verbose                      Verbose mode" << std::endl;
	std::cout << "      --version                      Print version and exit" << std::endl;
	std::cout << std::endl;
	std::cout << "  --use-current-time-in-device-ID    Use current time in the device ID, instead of the BIOS Date." << std::endl;
	std::cout << "                                     No need to use this option unless you know why you need it." << std::endl;
	std::cout << std::endl;
	std::cout << "The -l and -s option are mutually exclusive." << std::endl;
	std::cout << "If no server or output file is specified, ";
	std::cout << "either via the -s/-l option or via configuration file (option -c), ";
	std::cout << "the program will exit without doing anything." << std::endl;
	std::cout << std::endl;
	std::cout << "Examples:" << std::endl;
	std::cout << "  Use the configuration file /etc/ocsinventory-ng.conf :" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --conf /etc/ocsinventory-ng.conf" << std::endl;
	std::cout << std::endl;
	std::cout << "  Send inventory to server http://ocsinventory-ng/ocsinventory :" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --server http://ocsinventory-ng/ocsinventory" << std::endl;
	std::cout << std::endl;
	std::cout << "  Send inventory to server https://ocsinventory-ng/ocsinventory which requires http basic authentication :" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --server https://user:password@ocsinventory-ng/ocsinventory" << std::endl;
	std::cout << std::endl;
	std::cout << "  Save a local inventory to /var/tmp/inventoryFile.xml :" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --local /var/tmp/inventoryFile.xml" << std::endl;
	std::cout << std::endl;
	std::cout << "  Save a local inventory to /var/tmp/<device_id>.xml :" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --local /var/tmp/" << std::endl;
	std::cout << std::endl;
	std::cout << "  Print inventory to standard output :" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --stdout" << std::endl;

	::exit(0);
}


static void
PrintVersionAndExit()
{
	std::cout << __progname << " " << Agent::Version() << std::endl;
	::exit(0);
}


static void
HandleArgs(int argc, char **argv)
{
	bool verbose = false;
	bool daemonize = false;
	Configuration* config = Configuration::Get();
	int optIndex = 0;
	int c = 0;
	while ((c = ::getopt_long(argc, argv, "c:s:dDt:l:hvw:",
			sLongOptions, &optIndex)) != -1) {
		switch (c) {
			case 'c':
				config->Load(optarg);
				break;
			case 's':
				config->SetServer(optarg);
				break;
			case 'D':
			case 'd':
				daemonize = true;
				config->SetVolatileKeyValue("DAEMONIZE", "true");
				break;
			case 't':
				config->SetVolatileKeyValue("TAG", optarg);
				break;
			case 'l':
				config->SetOutputFileName(optarg);
				break;
			case 'h':
				PrintHelpAndExit();
				break;
			case 'v':
				verbose = true;
				break;
			case 'w':
				config->SetVolatileKeyValue("waittime", optarg);
				break;
			case 0:
			{
				std::string optName = sLongOptions[optIndex].name;
				if (optName == "nosoftware")
					config->SetVolatileKeyValue("nosoftware", "true");
				else if (optName == "stdout" && !daemonize)
					config->SetVolatileKeyValue("stdout", "true");
				else if (optName == "use-current-time-in-device-ID")
					config->SetUseCurrentTimeInDeviceID(true);
				else if (optName == "agent-string")
					config->SetVolatileKeyValue(CONF_AGENT_STRING, optarg);
				else if (optName == "logger")
					Logger::Get(optarg);
				else if (optName == "version")
					PrintVersionAndExit();
				break;
			}
		}
	}

	// Get the already initialized logger, if any, otherwise the default one
	Logger& logger = Logger::GetDefault();
	if (verbose)
		logger.SetLevel(LOG_DEBUG);
	else
		logger.SetLevel(LOG_INFO);

	bool stdout = config->KeyValue("stdout") == CONF_VALUE_TRUE;
	if (!stdout && config->ServerURL().empty()
		&& config->LocalInventory() && config->OutputFileName().empty()) {
		PrintHelpAndExit();
	}
}


static void
Daemonize()
{
	pid_t processID = ::fork();
	if (processID < 0) {
		Logger::GetDefault().Log(LOG_ERR, "Failed to daemonize. Exiting...");
		// Return failure in exit status
		::exit(1);
	}

	// Exit the parent process
	if (processID > 0)
		::exit(0);

	::umask(0);
	if (::chdir("/") < 0)
		; // Ignore

	//set new session
	pid_t sid = ::setsid();
	if (sid < 0)
		::exit(1);

	::close(STDIN_FILENO);
	::close(STDOUT_FILENO);
	::close(STDERR_FILENO);
}


int
main(int argc, char **argv)
{
	HandleArgs(argc, argv);

	if (Configuration::Get()->KeyValue("DAEMONIZE") == CONF_VALUE_TRUE)
		Daemonize();

	Logger& logger = Logger::GetDefault();

	try {
		Agent agent;
		agent.Run();
#if DEBUG
		Configuration::Get()->Print();
		std::cout << "Agent String: " << Agent::AgentString() << std::endl;
#endif
	} catch (std::exception& ex) {
		logger.Log(LOG_ERR, ex.what());
		return 1;
	} catch (...) {
		logger.Log(LOG_ERR, "Unhandled exception.");
		return 1;
	}

	Configuration::Get()->Save();

	return 0;
}

