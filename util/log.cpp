//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "log.h"

#include <iostream>
#include <cstdio>

// TODO: this singleton isn't Thread-safe

Logger::Logger()
{
    priorityMask_ = DEFAULT_LOG_MASK;
    output_.push_back(&DEFAULT_LOG_OUTPUT);
}

Logger * Logger::instance_ = 0;

// Returns the instance of Logger utility (not thread-safe)
Logger * Logger::instance()
{
    if (instance_ == 0)
    {
        instance_ = new Logger();
    }
    
    return instance_;
}

// Returns true if Logger is logging on logPriority
bool Logger::priorityEnabled(LogPriority logPriority) const
{
    return BIT_ENABLED(this->priorityMask_, logPriority);
}

// Returns true if Logger is NOT logging on logPriority
bool Logger::priorityDisabled(LogPriority logPriority) const
{
    return BIT_DISABLED(this->priorityMask_, logPriority);
}

// Start logging on logPriority
void Logger::enbaledPriority(LogPriority logPriority)
{
    SET_BITS(this->priorityMask_, logPriority);
}

// Stop logging on logPriority
void Logger::disablePriority(LogPriority logPriority)
{
    CLR_BITS(this->priorityMask_, logPriority);
}

// Gets the Logger's priority mask
unsigned long Logger::getPriorityMask() const
{
    return this->priorityMask_;
}

// Sets the Logger's priority mask
void Logger::setPriorityMask(unsigned long logMask)
{
    this->priorityMask_ = logMask;
}

// Adds stream to the logger's output chain
void Logger::addLogOutput(std::ostream * stream)
{
    this->output_.push_back(stream);
}

// Logs the message on all output
void Logger::write(LogPriority logPriority, const char * format, ...)
{
    if (this->priorityEnabled(logPriority) == true)
    {
        char message[MAX_MESSAGE_SIZE] = { 0, };
	va_list ap;

	va_start(ap, format);
	vsnprintf(message, MAX_MESSAGE_SIZE, format, ap);
	va_end(ap);

        for (unsigned out = 0; out < this->output_.size(); out++)
        {
            *(this->output_[out]) << message;
        }
    }
}
