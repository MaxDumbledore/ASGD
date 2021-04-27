//
// Created by max on 2021/4/23.
//

#include "SessionManager.h"

void SessionManager::start(const SessionPtr &s) {
    sessions.insert(s);
    s->start();
}

void SessionManager::stop(const SessionPtr &s) {
    sessions.erase(s);
}