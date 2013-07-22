// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

library pub_tests;

import 'dart:io';

import '../descriptor.dart' as d;
import '../test_pub.dart';

main() {
  initConfig();

  forBothPubInstallAndUpdate((command) {
    integration("removes a dependency that's removed from the pubspec", () {
      servePackages([
        packageMap("foo", "1.0.0"),
        packageMap("bar", "1.0.0")
      ]);

      d.appDir({"foo": "any", "bar": "any"}).create();

      pubCommand(command);

      d.packagesDir({
        "foo": "1.0.0",
        "bar": "1.0.0"
      }).validate();

      d.appDir({"foo": "any"}).create();

      pubCommand(command);

      d.packagesDir({
        "foo": "1.0.0",
        "bar": null
      }).validate();
    });
  });
}
