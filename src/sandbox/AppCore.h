#pragma once
#include "core/Logging.h"

#define SAFE_DELETE(p) if ((p) != NULL) { delete (p); (p) = NULL; }
#define SAFE_DELETE_ARR(p) if ((p) != NULL) { delete[] (p); (p) = NULL; }
#define SAFE_DELETE_STL_CONTAINER(vec) for(auto& p : vec) SAFE_DELETE(p);
#define ASSERT_TEMPLATE_INHERITANCE(_parent_, _t_, _message_) static_assert(std::is_base_of<_parent_, _t_>::value == true, _message_);
