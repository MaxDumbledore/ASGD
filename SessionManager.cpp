//
// Created by max on 2021/4/23.
//

#include "SessionManager.h"

void SessionManager::start(const SessionPtr &s) {
    sessions.insert(s);
    s->start();
}

void SessionManager::stop(const SessionPtr &s) {
    s->stop();
    sessions.erase(s);
}

SessionManager::SessionManager() : curId(0) {
    globalParams.setParams(MnistCNN().parameters());
}

int SessionManager::assignNewId() {
    return curId++;
}

GlobalParams &SessionManager::params() {
    return globalParams;
}
