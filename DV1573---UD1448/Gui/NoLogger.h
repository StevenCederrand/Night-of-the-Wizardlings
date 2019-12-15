#ifndef NOLOGGER_H
#define NOLOGGER_H
#include <Pch/Pch.h>

class NoLogger : public CEGUI::Logger
{
	void logEvent(const CEGUI::String&, CEGUI::LoggingLevel)
	{

	}

	void setLogFilename(const CEGUI::String&, bool)
	{

	}
};

//NoLogger nL;

#endif