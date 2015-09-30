#include "ftnoir_tracker_joystick.h"
#include "opentrack/plugin-api.hpp"

static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
    auto self = ( TrackerControls* )pContext;
    auto name = QString(pdidInstance->tszInstanceName);
    auto guid = guid_to_string(pdidInstance->guidInstance);
    TrackerControls::joys cur { name, guid };
    self->_joys.append(cur);

    self->ui.joylist->addItem(name + " " + guid);

    return DIENUM_CONTINUE;
}

TrackerControls::TrackerControls() : tracker(nullptr)
{
    ui.setupUi( this );

    // Connect Qt signals to member-functions
    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(doOK()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(doCancel()));

    {
        auto hr = CoInitialize( nullptr );
        LPDIRECTINPUT8 g_pDI = nullptr;

        if( FAILED( hr = DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION,
                                             IID_IDirectInput8, ( VOID** )&g_pDI, NULL ) ) )
            goto fin;

        if( FAILED( hr = g_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL,
                                             EnumJoysticksCallback,
                                             this,
                                             DIEDFL_ATTACHEDONLY )))
            goto fin;

fin:
        if (g_pDI)
            g_pDI->Release();
    }

    tie_setting(s.joy_1, ui.joy_1);
    tie_setting(s.joy_2, ui.joy_2);
    tie_setting(s.joy_3, ui.joy_3);
    tie_setting(s.joy_4, ui.joy_4);
    tie_setting(s.joy_5, ui.joy_5);
    tie_setting(s.joy_6, ui.joy_6);
}

void TrackerControls::doOK() {
    int idx = ui.joylist->currentIndex();
    joys def { "", "" };
    auto val = _joys.value(idx, def);
    s.guid = val.guid;
    s.joyid = val.name;
    s.b->save();
    if (tracker)
        tracker->reload();
    this->close();
}

void TrackerControls::doCancel() {
    s.b->reload();
    this->close();
}


