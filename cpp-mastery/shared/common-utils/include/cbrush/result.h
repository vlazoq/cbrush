#pragma once
// cbrush::Result<T, E> — a tiny expected<T,E> replacement for C++20 code.
// Prefer std::expected when you can bump to C++23.

#include <string>
#include <type_traits>
#include <utility>
#include <variant>

namespace cbrush {

struct Error {
  std::string message;
  int code = 0;
  Error() = default;
  Error(std::string m, int c = 0) : message(std::move(m)), code(c) {}
};

template <class T, class E = Error>
class Result {
 public:
  Result(const T& v) : v_(v) {}
  Result(T&& v) : v_(std::move(v)) {}
  Result(const E& e) : v_(e) {}
  Result(E&& e) : v_(std::move(e)) {}

  [[nodiscard]] bool ok() const noexcept { return v_.index() == 0; }
  explicit operator bool() const noexcept { return ok(); }

  T& value() & { return std::get<0>(v_); }
  const T& value() const& { return std::get<0>(v_); }
  T&& value() && { return std::get<0>(std::move(v_)); }

  const E& error() const& { return std::get<1>(v_); }
  E&& error() && { return std::get<1>(std::move(v_)); }

  template <class F>
  auto map(F&& f) const -> Result<std::invoke_result_t<F, const T&>, E> {
    using R = std::invoke_result_t<F, const T&>;
    if (ok()) return Result<R, E>{f(value())};
    return Result<R, E>{error()};
  }

 private:
  std::variant<T, E> v_;
};

template <class E>
class Result<void, E> {
 public:
  Result() : err_{}, has_err_(false) {}
  Result(E e) : err_(std::move(e)), has_err_(true) {}
  [[nodiscard]] bool ok() const noexcept { return !has_err_; }
  explicit operator bool() const noexcept { return ok(); }
  const E& error() const { return err_; }

 private:
  E err_;
  bool has_err_;
};

inline Result<void> Ok() { return Result<void>{}; }
inline Error Err(std::string msg, int code = 0) { return Error{std::move(msg), code}; }

}  // namespace cbrush
