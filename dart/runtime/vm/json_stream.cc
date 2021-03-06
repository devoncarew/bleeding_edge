// Copyright (c) 2013, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "platform/assert.h"
#include "vm/object.h"
#include "vm/json_stream.h"


namespace dart {

JSONStream::JSONStream(intptr_t buf_size) : buffer_(buf_size) {
  open_objects_ = 0;
  arguments_ = NULL;
  num_arguments_ = 0;
  option_keys_ = NULL;
  option_values_ = NULL;
  num_options_ = 0;
}


JSONStream::~JSONStream() {
}


void JSONStream::Clear() {
  buffer_.Clear();
  open_objects_ = 0;
}


void JSONStream::OpenObject(const char* property_name) {
  PrintCommaIfNeeded();
  open_objects_++;
  if (property_name != NULL) {
    PrintPropertyName(property_name);
  }
  buffer_.AddChar('{');
}


void JSONStream::CloseObject() {
  ASSERT(open_objects_ > 0);
  open_objects_--;
  buffer_.AddChar('}');
}


void JSONStream::OpenArray(const char* property_name) {
  PrintCommaIfNeeded();
  if (property_name != NULL) {
    PrintPropertyName(property_name);
  }
  open_objects_++;
  buffer_.AddChar('[');
}


void JSONStream::CloseArray() {
  ASSERT(open_objects_ > 0);
  open_objects_--;
  buffer_.AddChar(']');
}


void JSONStream::PrintValueBool(bool b) {
  PrintCommaIfNeeded();
  buffer_.Printf("%s", b ? "true" : "false");
}


void JSONStream::PrintValue(intptr_t i) {
  PrintCommaIfNeeded();
  buffer_.Printf("%" Pd "", i);
}


void JSONStream::PrintValue(double d) {
  PrintCommaIfNeeded();
  buffer_.Printf("%f", d);
}


void JSONStream::PrintValue(const char* s) {
  PrintCommaIfNeeded();
  buffer_.AddChar('"');
  buffer_.AddEscapedString(s);
  buffer_.AddChar('"');
}


void JSONStream::PrintfValue(const char* format, ...) {
  PrintCommaIfNeeded();

  va_list args;
  va_start(args, format);
  intptr_t len = OS::VSNPrint(NULL, 0, format, args);
  va_end(args);
  char* p = reinterpret_cast<char*>(malloc(len+1));
  va_start(args, format);
  intptr_t len2 = OS::VSNPrint(p, len+1, format, args);
  va_end(args);
  ASSERT(len == len2);
  buffer_.AddChar('"');
  buffer_.AddEscapedString(p);
  buffer_.AddChar('"');
  free(p);
}


void JSONStream::PrintValue(const Object& o, bool ref) {
  PrintCommaIfNeeded();
  o.PrintToJSONStream(this, ref);
}


void JSONStream::PrintValue(const Field& field,
                            const Instance& instance,
                            bool ref) {
  PrintCommaIfNeeded();
  field.PrintToJSONStreamWithInstance(this, instance, ref);
}


void JSONStream::PrintPropertyBool(const char* name, bool b) {
  PrintPropertyName(name);
  PrintValueBool(b);
}


void JSONStream::PrintProperty(const char* name, intptr_t i) {
  PrintPropertyName(name);
  PrintValue(i);
}


void JSONStream::PrintProperty(const char* name, double d) {
  PrintPropertyName(name);
  PrintValue(d);
}


void JSONStream::PrintProperty(const char* name, const char* s) {
  PrintPropertyName(name);
  PrintValue(s);
}


void JSONStream::PrintfProperty(const char* name, const char* format, ...) {
  PrintPropertyName(name);
  va_list args;
  va_start(args, format);
  intptr_t len = OS::VSNPrint(NULL, 0, format, args);
  va_end(args);
  char* p = reinterpret_cast<char*>(malloc(len+1));
  va_start(args, format);
  intptr_t len2 = OS::VSNPrint(p, len+1, format, args);
  va_end(args);
  ASSERT(len == len2);
  buffer_.AddChar('"');
  buffer_.AddEscapedString(p);
  buffer_.AddChar('"');
  free(p);
}


void JSONStream::SetArguments(const char** arguments, intptr_t num_arguments) {
  arguments_ = arguments;
  num_arguments_ = num_arguments;
}


void JSONStream::SetOptions(const char** option_keys,
                            const char** option_values,
                            intptr_t num_options) {
  option_keys_ = option_keys;
  option_values_ = option_values;
  num_options_ = num_options;
}


void JSONStream::PrintProperty(const char* name, const Object& o, bool ref) {
  PrintPropertyName(name);
  PrintValue(o, ref);
}


void JSONStream::PrintPropertyName(const char* name) {
  ASSERT(name != NULL);
  PrintCommaIfNeeded();
  buffer_.AddChar('"');
  buffer_.AddEscapedString(name);
  buffer_.AddChar('"');
  buffer_.AddChar(':');
}


void JSONStream::PrintCommaIfNeeded() {
  if (NeedComma()) {
    buffer_.AddChar(',');
  }
}


bool JSONStream::NeedComma() {
  const char* buffer = buffer_.buf();
  intptr_t length = buffer_.length();
  if (length == 0) {
    return false;
  }
  char ch = buffer[length-1];
  return (ch != '[') && (ch != '{') && (ch != ':') && (ch != ',');
}


JSONObject::JSONObject(const JSONArray* arr) : stream_(arr->stream_) {
  stream_->OpenObject();
}


void JSONObject::AddPropertyF(const char* name,
                              const char* format, ...) const {
  stream_->PrintPropertyName(name);
  va_list args;
  va_start(args, format);
  intptr_t len = OS::VSNPrint(NULL, 0, format, args);
  va_end(args);
  char* p = reinterpret_cast<char*>(malloc(len+1));
  va_start(args, format);
  intptr_t len2 = OS::VSNPrint(p, len+1, format, args);
  va_end(args);
  ASSERT(len == len2);
  stream_->buffer_.AddChar('"');
  stream_->buffer_.AddEscapedString(p);
  stream_->buffer_.AddChar('"');
  free(p);
}


void JSONArray::AddValueF(const char* format, ...) const {
  stream_->PrintCommaIfNeeded();
  va_list args;
  va_start(args, format);
  intptr_t len = OS::VSNPrint(NULL, 0, format, args);
  va_end(args);
  char* p = reinterpret_cast<char*>(malloc(len+1));
  va_start(args, format);
  intptr_t len2 = OS::VSNPrint(p, len+1, format, args);
  va_end(args);
  ASSERT(len == len2);
  stream_->buffer_.AddChar('"');
  stream_->buffer_.AddEscapedString(p);
  stream_->buffer_.AddChar('"');
  free(p);
}

}  // namespace dart
