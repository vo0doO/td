//
// Copyright Aliaksei Levin (levlam@telegram.org), Arseny Smirnov (arseny30@gmail.com) 2014-2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "td/telegram/DelayDispatcher.h"

#include "td/telegram/Global.h"
#include "td/telegram/net/NetQueryDispatcher.h"

namespace td {
void DelayDispatcher::send_with_callback(NetQueryPtr query, ActorShared<NetQueryCallback> callback) {
  queue_.push({std::move(query), std::move(callback)});
  loop();
}

void DelayDispatcher::loop() {
  if (!wakeup_at_.is_in_past()) {
    set_timeout_at(wakeup_at_.at());
    return;
  }

  if (queue_.empty()) {
    return;
  }

  auto query = std::move(queue_.front());
  queue_.pop();
  G()->net_query_dispatcher().dispatch_with_callback(std::move(query.net_query), std::move(query.callback));

  wakeup_at_ = Timestamp::in(DELAY);

  if (queue_.empty()) {
    return;
  }

  set_timeout_at(wakeup_at_.at());
}
}  // namespace td
