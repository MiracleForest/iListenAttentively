#include "Global.h"
#include "Entry.h"

#include <ll/api/mod/RegisterHelper.h>
#include <memory>

ll::Logger logger("iListenAttentively");

namespace iListenAttentively {

static std::unique_ptr<Entry> instance;

Entry& Entry::getInstance() { return *instance; }

bool Entry::load() { return true; }

bool Entry::enable() { return true; }

bool Entry::disable() { return true; }

} // namespace iListenAttentively

LL_REGISTER_MOD(iListenAttentively::Entry, iListenAttentively::instance);