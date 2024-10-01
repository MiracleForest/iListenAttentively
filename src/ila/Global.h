#pragma once

#include <ll/api/Logger.h>
#include <ll/api/event/Emitter.h>
#include <ll/api/event/Event.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/memory/Hook.h>

#define SelfLogger(...) iListenAttentively::getInstance().getSelf().getLogger()