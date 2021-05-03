//
// Created by max on 2021/4/23.
//

#ifndef ASGD_SESSIONMANAGER_H
#define ASGD_SESSIONMANAGER_H

#include <set>
#include "Session.h"
#include "Model.h"
#include "Params.h"

class SessionManager {
public:
    SessionManager();

    int assignNewId();

    Params &params();

    void start(const SessionPtr &s);

    void stop(const SessionPtr &s);

private:
    std::set<SessionPtr> sessions;
    std::atomic_int curId;
    Params globalParams;
};


#endif //ASGD_SESSIONMANAGER_H
