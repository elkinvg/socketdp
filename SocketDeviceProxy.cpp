#include "SocketDeviceProxy.h"

socketDeviceProxy::socketDeviceProxy(string &name) :
    DeviceProxy(name)
{
}

Tango::DevLong socketDeviceProxy::numAvalaibleBites() {
    Tango::DeviceData output;
    Tango::DevLong avBites = 0;
    try {
        output = command_inout("AvalaibleBytes");
        output >> avBites;
    }
    catch (Tango::DevFailed &e) {}
    return avBites;
}

bool socketDeviceProxy::checkSocketState(TANGO_BASE_CLASS *mainDev, Tango::DevFailed &exc, bool changeStateAndStatus) {
    bool isSocketOn;
    bool isAsynchExc = false;
    bool isExc = true;
    Tango::DevState stateSocket;
    Tango::DeviceData outputCom;
    try {
        if (!_isCommandSent) {
            requestState_id = command_inout_asynch("State");
            _isCommandSent = true;
        }
        outputCom = command_inout_reply(requestState_id);
        outputCom >> stateSocket;
        if (stateSocket == Tango::ON) {
            isSocketOn = true;
        }
        else if (stateSocket == Tango::OFF || stateSocket == Tango::FAULT)
        {
            isSocketOn = false;
            mainDev->set_state(Tango::FAULT);
            mainDev->set_status("Socket state is OFF or FAULT");
        }
        requestState_id = command_inout_asynch("State");
        _isCommandSent = true;
        isExc = false;
    }
    catch (Tango::AsynReplyNotArrived& e) {
        exc = e;
        isSocketOn = false;
        if (changeStateAndStatus) {
            mainDev->set_state(Tango::FAULT);
            mainDev->set_status(getMessFromException(e));
        }
        isAsynchExc = true;
    }
    catch (Tango::DevFailed &e) {
        exc = e;
        isSocketOn = false;
        if (changeStateAndStatus) {
            mainDev->set_state(Tango::FAULT);
            mainDev->set_status(getMessFromException(e));
        }
    }
    if (!isAsynchExc && isExc) {
        try {
            requestState_id = command_inout_asynch("State");
            _isCommandSent = true;
        }
        catch (Tango::DevFailed &e) {
            _isCommandSent = false;
        }
    }

    return isSocketOn;
}

string socketDeviceProxy::getMessFromException(Tango::DevFailed &e) {
    auto lnh = e.errors.length();
    string status_f;
    for (unsigned int i = 0; i<lnh; i++) {
        status_f += e.errors[i].desc;
    }
    return status_f;
}
