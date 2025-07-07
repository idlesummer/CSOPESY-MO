#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy::scheduler {
  using list = vector<str>;
  
  list get_all() {
    return {
      // make_fcfs_strategy(),
      // make_rr_strategy(),
    };
  }
}
