//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(log_H)
#define log_H

#include <vector>
#include <ostream>
#include <cstdarg>

// Helper Macros
# define BIT_ENABLED(WORD, BIT) (((WORD) & (BIT)) != 0)
# define BIT_DISABLED(WORD, BIT) (((WORD) & (BIT)) == 0)
# define SET_BITS(WORD, BITS) (WORD |= (BITS))
# define CLR_BITS(WORD, BITS) (WORD &= ~(BITS))

// Log Masks
// There are 2 log levels of each kind:
//  - debug:  TRACE and DEBUG
//  - normal: INFO and NOTICE
//  - error:  ERROR and CRITICAL
enum LogPriority
{
  LG_TRACE     =   01,
  LG_DEBUG     =   02,
  LG_INFO      =   04,
  LG_NOTICE    =  010,
  LG_ERROR     =  020,
  LG_CRITICAL  =  040,
  
  LG_MAX = LG_CRITICAL
};

// This is the default log mask. 
// We need to enable TRACE manually (not included on DEBUG mode)
#if defined(DEBUG)
#define DEFAULT_LOG_MASK LG_DEBUG | LG_INFO | LG_NOTICE | LG_ERROR | LG_CRITICAL
#define DEFAULT_LOG_OUTPUT std::cerr
#else
#define DEFAULT_LOG_MASK LG_INFO | LG_NOTICE | LG_ERROR | LG_CRITICAL
#define DEFAULT_LOG_OUTPUT std::cout
#endif

// Messages bigger than this will be cropped
#define MAX_MESSAGE_SIZE 8192

#define log(priority, format, ...) Logger::instance()->write(priority, format"\n", ## __VA_ARGS__)
#define lognonl(priority, format, ...) Logger::instance()->write(priority, format, ## __VA_ARGS__)

class Logger
{
    private:
        static Logger * instance_;
        
        unsigned long priorityMask_;
        std::vector<std::ostream *> output_;

        Logger();
        
    public:
        static Logger * instance();
        
        bool priorityEnabled(LogPriority logPriority) const;
        bool priorityDisabled(LogPriority logPriority) const;
        
        void enbaledPriority(LogPriority logPriority);
        void disablePriority(LogPriority logPriority);
        
        unsigned long getPriorityMask() const;
        void setPriorityMask(unsigned long logMask);
        
        void addLogOutput(std::ostream *);
        
        void write(LogPriority logPriority, const char * format, ...);
};

#endif // log_H
