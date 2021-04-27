//
// Created by max on 2021/4/23.
//

#ifndef ASGD_SESSIONMANAGER_H
#define ASGD_SESSIONMANAGER_H

#include <set>
#include "Session.h"

class SessionManager {
public:
    SessionManager() = default;

    void start(const SessionPtr &s);

    void stop(const SessionPtr &s);

private:
    std::set<SessionPtr> sessions;
};


#endif //ASGD_SESSIONMANAGER_H
