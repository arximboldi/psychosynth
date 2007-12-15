
#ifndef OSCSERVERLOGGER_H
#define OSCSERVERLOGGER_H

#include "net/OSCServer.h"

class OSCServerLogger : public OSCServerListener
{
public:
    virtual bool handleStartListening(OSCServer* server) {
	Logger::instance().log("oscserver", Log::INFO, "Server listening.");
	return false;
    }
    
    virtual bool handleStopListening(OSCServer* server) {
	Logger::instance().log("oscserver", Log::INFO, "Server no longer listening.");
	return false;
    }
    
    virtual bool handleClientConnect(OSCServer* server, int client_id) {
	Logger::instance().log("oscserver", Log::INFO, "Client connected.");
	return false;
    }
    
    virtual bool handleClientDisconnect(OSCServer* server, int client_id) {
	Logger::instance().log("oscserver", Log::INFO, "Client disconnected.");
	return false;
    }
    
    virtual bool handleClientTimeout(OSCServer* server, int client_id) {
	Logger::instance().log("oscserver", Log::INFO, "Client timeout.");
	return false;
    }
};

#endif /* OSCSERVERLOGGER_H */
