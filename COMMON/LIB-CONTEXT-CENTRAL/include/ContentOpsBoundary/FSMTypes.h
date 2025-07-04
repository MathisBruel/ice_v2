#pragma once
#include <machine.hpp>
#include "ContentOpsBoundary/COB_Context.h"

using Config = hfsm2::Config::ContextT<COB_Context*>;
using Machine = hfsm2::MachineT<Config>;
using BoundaryFSM = Machine; 