#include "context.hpp"

namespace isl {

std::unordered_map<isl_ctx*, std::weak_ptr<context::data>> context::m_store;

}
