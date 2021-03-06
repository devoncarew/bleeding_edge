// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

library multiple_timer_test;

import 'dart:isolate';
import 'dart:async';
import '../../../pkg/unittest/lib/unittest.dart';

const Duration TIMEOUT = const Duration(milliseconds: 100);

// Some browsers (Firefox and IE so far) can trigger too early. Add a safety
// margin. We use identical(1, 1.0) as an easy way to know if the test is
// compiled by dart2js.
int get safetyMargin => identical(1, 1.0) ? 100 : 0;

createTimer(replyTo) {
  new Timer(TIMEOUT, () {
    replyTo.send("timer_fired");
  });
}

main() {
  test("timer in isolate", () {
    int startTime;
    int endTime;

    ReceivePort port = new ReceivePort();

    port.first.then(expectAsync1((msg) {
      expect("timer_fired", msg);
      int endTime = (new DateTime.now()).millisecondsSinceEpoch;
      expect(endTime - startTime + safetyMargin, greaterThanOrEqualTo(TIMEOUT.inMilliseconds));
    }));

    startTime = (new DateTime.now()).millisecondsSinceEpoch;
    var remote = Isolate.spawn(createTimer, port.sendPort);
  });
}
