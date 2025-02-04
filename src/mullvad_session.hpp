#pragma once

#include "cpr/session.h"


class MullvadSession : public cpr::Session {
public:
    MullvadSession();

    cpr::Response Get(const cpr::Url &url);
    bool am_i_mullvad();
};
