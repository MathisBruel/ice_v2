#pragma once
#include <machine.hpp>
#include "ContentOpsBoundary/ContentContext.h"

using Config = hfsm2::Config::ContextT<ContentContext*>;
using Machine = hfsm2::MachineT<Config>;
using BoundaryFSM = Machine; 