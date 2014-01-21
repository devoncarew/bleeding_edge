// Copyright (c) 2014, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

import 'dart:async';
import 'dart:html' as html;

import '../entrypoint.dart';
import '../io.dart';
import '../log.dart' as log;
import '../source/hosted.dart';
import '../path_rep.dart';
import '../wrap/system_cache_wrap.dart';

typedef void LogFunction(String line, String level);

/// Gets the dependencies for the current project. The project is specified by
/// the working directory [entry].
Future getDependencies(html.DirectoryEntry entry, [LogFunction extraLog]) {
  // Turn on the maximum level of logging, and hook up any extra log function.
  log.showAll();
  if (extraLog != null) log.addLoggerFunction(extraLog);

  // Store [entry] as the working directory.
  FileSystem.workingDir = new Directory(entry);

  return SystemCache.withSources(FileSystem.workingDirPath().join("cache")))
      ..catchError((e) => log.error("Could not create system cache", e))
      .then((cache) => Entrypoint.load(FileSystem.workingDirPath(), cache))
      .then((entrypoint) => entrypoint.acquireDependencies())
      .then((_) => log.fine("Got dependencies!"));
}

Future<List<String>> getAvailablePackageList() {
  return HostedSource.getHostedPackages();
}