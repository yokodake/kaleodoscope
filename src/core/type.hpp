#pragma once
#include <prelude.hpp>
#include <variant>
#include <vector>

#include "name.hpp"

namespace mangekyou {

using name::Id;
// using TyVar = Var;

/** Kinds */
struct Kind;

/// maybe rename to Type?
struct KStar {
  bool is_equal(const KStar& _) const noexcept { return true; }
  std::string to_string() const;
};

struct KArr {
  Rc<Kind> lhs;
  Rc<Kind> rhs;

  KArr(const Rc<Kind>& lhs, const Rc<Kind>& rhs)
      : lhs(lhs)
      , rhs(rhs) {}

  bool is_equal(const KArr& rhs) const;
  std::string to_string() const;
};

struct Kind : std::variant<KStar, KArr> {
  using std::variant<KStar, KArr>::variant;

  // static const Kind STAR = Star{};

  Kind Arrow(const Rc<Kind>& lhs, const Rc<Kind>& rhs);
  Kind Star();

  template <typename T>
  const bool is() const {
    return std::holds_alternative<T>(*this);
  }

  bool operator==(const Kind& rhs);
  bool operator!=(const Kind& rhs);

  std::string to_string() {
    // @FIXME std::visit, for some arcane reason, just won't work no matter what
    // I try. Which is fun, because it completely defeats the point of using
    // std::variant!!!!! Yes, I'm mad.
    if (this->is<KStar>()) {
      return std::get<KStar>(*this).to_string();
    } else {
      return std::get<KArr>(*this).to_string();
    }
  }
};

/** Types */
struct Type;

struct TyVar {
  Id id;
  Kind _kind;

  TyVar(Id id, Kind _kind)
      : id(id)
      , _kind(_kind) {}

  bool operator==(const TyVar& other);
  bool operator!=(const TyVar& other);
  bool operator<(const TyVar& other);

  Kind kind();
  std::string to_string();
};

struct TyCon {
  Id id;
  Kind _kind;

  TyCon(Id id, Kind _kind)
      : id(id)
      , _kind(_kind) {}

  Kind kind();
  std::string to_string();
};

// @FIXME
struct TyApp {
  Rc<Type> lhs;
  Rc<Type> rhs;

  TyApp(const Rc<Type>& lhs, const Rc<Type>& rhs)
      : lhs(lhs)
      , rhs(rhs) {}

  Kind kind();
  std::string to_string();
};
/// generic/quantified type variables
struct TyGen {
  i32 i;

  TyGen(i32 i)
      : i(i) {}

  Kind kind();
  std::string to_string();
};

#define FALLTHROUGH_TYPE(f)                                                    \
  if (this->is<TyVar>())                                                       \
    return std::get<TyVar>(*this).f();                                         \
  if (this->is<TyCon>())                                                       \
    return std::get<TyCon>(*this).f();                                         \
  if (this->is<TyApp>())                                                       \
    return std::get<TyApp>(*this).f();                                         \
  if (this->is<TyGen>())                                                       \
    return std::get<TyGen>(*this).f();

namespace tc {
struct Subst;
}
struct Type : std::variant<TyVar, TyCon, TyApp, TyGen> {
  using std::variant<TyVar, TyCon, TyApp, TyGen>::variant;

  static Type Var(const Id& i, const Kind& k) { return TyVar(i, k); }
  static Type Con(const Id& i, const Kind& k) { return TyCon(i, k); }
  static Type App(const Rc<Type>& lhs, Rc<Type>& rhs) {
    return TyApp(lhs, rhs);
  }
  static Type Gen(i32 i) { return TyGen(i); }

  Kind kind() {
    // @FIXME std::visit, for some arcane reason, just won't work no matter
    // what
    // I try. Which is fun, because it completely defeats the point of using
    // std::variant!!!!! Yes, I'm mad.
    FALLTHROUGH_TYPE(kind);
  }
  std::string to_string() { FALLTHROUGH_TYPE(to_string); }
  template <typename T>
  bool is() {
    return std::holds_alternative<T>(*this);
  }

  // return Rc<Type> instead?
  /// apply a substitution
  Type apply(const tc::Subst& s);
  /// retrieve all type variables
  std::vector<TyVar> tv();
  static std::vector<TyVar> tv(std::vector<Rc<Type>>);
};

using KindOrType = Type;

/// type scheme
/// @IMPROVEMENT assumes no explicit foralls,
/// assumes `Type::Gen`'s kind is `Scheme::kinds[Type::Gen::i]`.
/// even the paper admits it sucks
/*
struct Scheme {
std::vector<Kind> kinds;
// @TODO
Qual<Type> ty;
};
*/

} // namespace mangekyou
