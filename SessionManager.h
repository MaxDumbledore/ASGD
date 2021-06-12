//
// Created by max on 2021/4/23.
//

#ifndef ASGD_SESSIONMANAGER_H
#define ASGD_SESSIONMANAGER_H

#include <set>
#include "Model.h"
#include "Params.h"
#include "Session.h"

/**
 * @note this class is not thread-safe
 * Attention! if we use multi-thread for the IO-context, we need to make
 * sessions thread-safe.
 */

class SessionManager {
   public:
    SessionManager();

    Params& params();

    void insert(const SessionPtr& s);

    void startAll();

    void stop(const SessionPtr& s);

    void finishAll();

   private:
    std::set<SessionPtr> sessions;
    Params globalParams;
};

#endif  // ASGD_SESSIONMANAGER_H
