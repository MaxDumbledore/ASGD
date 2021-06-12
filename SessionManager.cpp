//
// Created by max on 2021/4/23.
//

#include "SessionManager.h"

void SessionManager::insert(const SessionPtr& s) {
    sessions.insert(s);
}

void SessionManager::startAll() {
    for (auto& i : sessions)
        i->start();
}

void SessionManager::stop(const SessionPtr& s) {
    s->stop();
    sessions.erase(s);
}

/**
 * @brief Construct a new Session Manager:: Session Manager object
 * Remind that in original article we need the participate 0 to generate initial
 * message. For convenience we just let Server to do that.
 */

SessionManager::SessionManager() {
    globalParams.setData(MnistCNN().parameters());
}

Params& SessionManager::params() {
    return globalParams;
}

void SessionManager::finishAll() {
    for (auto& i : sessions)
        i->sendFinalParams();
}