// Copyright (c) 2013, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#ifndef VM_JSON_STREAM_H_
#define VM_JSON_STREAM_H_

#include "platform/json.h"
#include "vm/allocation.h"

namespace dart {

class Field;
class Instance;
class JSONArray;
class JSONObject;
class Object;

class JSONStream : ValueObject {
 public:
  explicit JSONStream(intptr_t buf_size = 256);
  ~JSONStream();

  TextBuffer* buffer() { return &buffer_; }
  const char* ToCString() { return buffer_.buf(); }

  void SetArguments(const char** arguments, intptr_t num_arguments);
  void SetOptions(const char** option_keys, const char** option_values,
                  intptr_t num_options);

  intptr_t num_arguments() const { return num_arguments_; }
  const char* GetArgument(intptr_t i) const {
    return arguments_[i];
  }
  intptr_t num_options() const { return num_options_; }
  const char* GetOptionKey(intptr_t i) const {
    return option_keys_[i];
  }
  const char* GetOptionValue(intptr_t i) const {
    return option_values_[i];
  }

 private:
  void Clear();

  void OpenObject(const char* property_name = NULL);
  void CloseObject();

  void OpenArray(const char* property_name = NULL);
  void CloseArray();

  void PrintValueBool(bool b);
  void PrintValue(intptr_t i);
  void PrintValue(double d);
  void PrintValue(const char* s);
  void PrintfValue(const char* format, ...) PRINTF_ATTRIBUTE(2, 3);
  void PrintValue(const Object& o, bool ref = true);
  void PrintValue(const Field& f, const Instance& instance, bool ref = true);

  void PrintPropertyBool(const char* name, bool b);
  void PrintProperty(const char* name, intptr_t i);
  void PrintProperty(const char* name, double d);
  void PrintProperty(const char* name, const char* s);
  void PrintfProperty(const char* name, const char* format, ...)
  PRINTF_ATTRIBUTE(3, 4);
  void PrintProperty(const char* name, const Object& o, bool ref = true);

  void PrintPropertyName(const char* name);
  void PrintCommaIfNeeded();
  bool NeedComma();

  intptr_t nesting_level() const { return open_objects_; }

  intptr_t open_objects_;
  TextBuffer buffer_;
  const char** arguments_;
  intptr_t num_arguments_;
  const char** option_keys_;
  const char** option_values_;
  intptr_t num_options_;

  friend class JSONObject;
  friend class JSONArray;
};


class JSONObject : public ValueObject {
 public:
  explicit JSONObject(JSONStream* stream) : stream_(stream) {
    stream_->OpenObject();
  }
  JSONObject(const JSONObject* obj, const char* name) : stream_(obj->stream_) {
    stream_->OpenObject(name);
  }
  explicit JSONObject(const JSONArray* arr);

  ~JSONObject() {
    stream_->CloseObject();
  }

  void AddProperty(const char* name, bool b) const {
    stream_->PrintPropertyBool(name, b);
  }
  void AddProperty(const char* name, intptr_t i) const {
    stream_->PrintProperty(name, i);
  }
  void AddProperty(const char* name, double d) const {
    stream_->PrintProperty(name, d);
  }
  void AddProperty(const char* name, const char* s) const {
    stream_->PrintProperty(name, s);
  }
  void AddProperty(const char* name, const Object& obj, bool ref = true) const {
    stream_->PrintProperty(name, obj, ref);
  }
  void AddPropertyF(const char* name, const char* format, ...) const
      PRINTF_ATTRIBUTE(3, 4);

 private:
  JSONStream* stream_;

  friend class JSONArray;

  DISALLOW_ALLOCATION();
  DISALLOW_COPY_AND_ASSIGN(JSONObject);
};


class JSONArray : public ValueObject {
 public:
  explicit JSONArray(JSONStream* stream) : stream_(stream) {
    stream_->OpenArray();
  }
  JSONArray(const JSONObject* obj, const char* name) : stream_(obj->stream_) {
    stream_->OpenArray(name);
  }
  explicit JSONArray(const JSONArray* arr) : stream_(arr->stream_) {
    stream_->OpenArray();
  }
  ~JSONArray() {
    stream_->CloseArray();
  }

  void AddValue(bool b) const { stream_->PrintValueBool(b); }
  void AddValue(intptr_t i) const { stream_->PrintValue(i); }
  void AddValue(double d) const { stream_->PrintValue(d); }
  void AddValue(const char* s) const { stream_->PrintValue(s); }
  void AddValue(const Object& obj, bool ref = true) const {
    stream_->PrintValue(obj, ref);
  }
  // Print a field bound to a value.  Value is looked up from 'instance'.
  void AddValue(const Field& field,
                const Instance& instance,
                bool ref = true) const {
    stream_->PrintValue(field, instance, ref);
  }
  void AddValueF(const char* format, ...) const PRINTF_ATTRIBUTE(2, 3);

 private:
  JSONStream* stream_;

  friend class JSONObject;

  DISALLOW_ALLOCATION();
  DISALLOW_COPY_AND_ASSIGN(JSONArray);
};

}  // namespace dart

#endif  // VM_JSON_STREAM_H_
