#include "iListenAttentively.h"
#include "Global.h"
#include <ll/api/mod/RegisterHelper.h>
#include <memory>

namespace ila
{

static std::unique_ptr<iListenAttentively> instance;

iListenAttentively& iListenAttentively::getInstance() { return *instance; }

bool iListenAttentively::load() { return true; }

bool iListenAttentively::enable() { return true; }

bool iListenAttentively::disable() { return true; }

} // namespace ila

LL_REGISTER_MOD(ila::iListenAttentively, ila::instance);