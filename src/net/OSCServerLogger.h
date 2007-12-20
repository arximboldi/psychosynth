
#ifndef OSCSERVERLOGGER_H
#define OSCSERVERLOGGER_H

#include "net/OSCServer.h"

class OSCServerLogger : public OSCServerListener
{
public:
    virtual bool handleServerStartListening(OSCServer* server) {
	Logger::instance().log("oscserver", Log::INFO, "Server listening.");
	return false;
    }
    
    virtual bool handleServerStopListening(OSCServer* server, OSCServerError err) {
	switch(err) {
	case SE_NONE:
	    Logger::instance().log("oscserver", Log::INFO, "Server no longer listening.");
	    break;
	    
	case SE_PORT_BINDING:
	    Logger::instance().log("oscserver", Log::INFO, "Could not bind port.");
	    break;

	default:
	    break;
	}

	return false;
    }
    
    virtual bool handleServerClientConnect(OSCServer* server, int client_id) {
	Logger::instance().log("oscserver", Log::INFO, "Client connected.");
	return false;
    }
    
    virtual bool handleServerClientDisconnect(OSCServer* server, int client_id,
					OSCServerClientError cause) {
	switch(cause) {
	case SCE_NONE:
	    Logger::instance().log("oscserver", Log::INFO, "Client disconnected.");
	    break;

	case SCE_CLIENT_TIMEOUT:
	    Logger::instance().log("oscserver", Log::INFO, "Client timeout.");
	    break;
	    
	default:
	    break;
	}
	
	return false;
    }
};

#endif /* OSCSERVERLOGGER_H */
