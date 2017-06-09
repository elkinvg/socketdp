#include "tango.h"

class socketDeviceProxy : public Tango::DeviceProxy
{
public:
    socketDeviceProxy(string &name);
    virtual ~socketDeviceProxy(){};
    Tango::DevLong numAvalaibleBites();
    bool checkSocketState(TANGO_BASE_CLASS *mainDev, Tango::DevFailed &exc, bool changeStateAndStatus);
private:
    string getMessFromException(Tango::DevFailed &e);
    long requestState_id;
    bool _isCommandSent{ false };
};