//
// Created by max on 2021/4/23.
//

#ifndef ASGD_SESSIONMANAGER_H
#define ASGD_SESSIONMANAGER_H

#include <set>
#include "Session.h"
#include "Model.h"
#include "GlobalParams.h"

class SessionManager {
public:
    SessionManager();

    int assignNewId();

    GlobalParams &params();

    void start(const SessionPtr &s);

    void stop(const SessionPtr &s);

private:
    std::set<SessionPtr> sessions;
    std::atomic_int curId;
    GlobalParams globalParams;
};


#endif //ASGD_SESSIONMANAGER_H
