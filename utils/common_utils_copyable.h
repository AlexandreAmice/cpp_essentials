#pragma once

/**
 * This file is copied from Drakes's drake/common/drake_copyable.h with
 * appropriate modifications.
 */
// ============================================================================
// N.B. The spelling of the macro names between doc/Doxyfile_CXX.in and this
// file must be kept in sync!
// ============================================================================

/** @file
Provides careful macros to selectively enable or disable the special member
functions for copy-construction, copy-assignment, move-construction, and
move-assignment.

http://en.cppreference.com/w/cpp/language/member_functions#Special_member_functions

When enabled via these macros, the `= default` implementation is provided.
Code that needs custom copy or move functions should not use these macros.
*/

/** COMMON_UTILS_NO_COPY_NO_MOVE_NO_ASSIGN deletes the special member functions
for copy-construction, copy-assignment, move-construction, and move-assignment.
COMMON_UTILS's Doxygen is customized to render the deletions in detail, with
appropriate comments.  Invoke this macro in the public section of the class
declaration, e.g.:
<pre>
class Foo {
 public:
  COMMON_UTILS_NO_COPY_NO_MOVE_NO_ASSIGN(Foo);

  // ...
};
</pre>
*/
#define COMMON_UTILS_NO_COPY_NO_MOVE_NO_ASSIGN(Classname) \
  Classname(const Classname&) = delete;                   \
  void operator=(const Classname&) = delete;              \
  Classname(Classname&&) = delete;                        \
  void operator=(Classname&&) = delete;

/** COMMON_UTILS_DEFAULT_COPY_AND_MOVE_AND_ASSIGN defaults the special member
functions for copy-construction, copy-assignment, move-construction, and
move-assignment.  This macro should be used only when copy-construction and
copy-assignment defaults are well-formed.  Note that the defaulted move
functions could conceivably still be ill-formed, in which case they will
effectively not be declared or used -- but because the copy constructor exists
the type will still be MoveConstructible.  COMMON_UTILS's Doxygen is customized
to render the functions in detail, with appropriate comments.  Typically, you
should invoke this macro in the public section of the class declaration, e.g.:
<pre>
class Foo {
 public:
  COMMON_UTILS_DEFAULT_COPY_AND_MOVE_AND_ASSIGN(Foo);

  // ...
};
</pre>

However, if Foo has a virtual destructor (i.e., is subclassable), then
typically you should use either COMMON_UTILS_NO_COPY_NO_MOVE_NO_ASSIGN in the
public section or else COMMON_UTILS_DEFAULT_COPY_AND_MOVE_AND_ASSIGN in the
protected section, to prevent
<a href="https://en.wikipedia.org/wiki/Object_slicing">object slicing</a>.

The macro contains a built-in self-check that copy-assignment is well-formed.
This self-check proves that the Classname is CopyConstructible, CopyAssignable,
MoveConstructible, and MoveAssignable (in all but the most arcane cases where a
member of the Classname is somehow CopyAssignable but not CopyConstructible).
Therefore, classes that use this macro typically will not need to have any unit
tests that check for the presence nor correctness of these functions.

However, the self-check does not provide any checks of the runtime efficiency
of the functions.  If it is important for performance that the move functions
actually move (instead of making a copy), then you should consider capturing
that in a unit test.
*/
#define COMMON_UTILS_DEFAULT_COPY_AND_MOVE_AND_ASSIGN(Classname)         \
  Classname(const Classname&) = default;                                 \
  Classname& operator=(const Classname&) = default;                      \
  Classname(Classname&&) = default;                                      \
  Classname& operator=(Classname&&) = default;                           \
  /* Fails at compile-time if copy-assign doesn't compile. */            \
  /* Note that we do not test the copy-ctor here, because  */            \
  /* it will not exist when Classname is abstract.         */            \
  static void CommonUtilsDefaultCopyAndMoveAndAssign_DoAssign(           \
      Classname* a, const Classname& b) {                                \
    *a = b;                                                              \
  }                                                                      \
  static_assert(&CommonUtilsDefaultCopyAndMoveAndAssign_DoAssign ==      \
                    &CommonUtilsDefaultCopyAndMoveAndAssign_DoAssign,    \
                "This assertion is never false; its only purpose is to " \
                "generate 'use of deleted function: operator=' errors "  \
                "when Classname is a template.");

/** COMMON_UTILS_DECLARE_COPY_AND_MOVE_AND_ASSIGN declares the special member
functions for copy-construction, copy-assignment, move-construction, and
move-assignment. Use this macro when these functions are available, but require
non-default implementations. (Use COMMON_UTILS_DEFAULT_COPY_AND_MOVE_AND_ASSIGN
instead if you can use the compiler-generated default implementations.)
CommonUtils's Doxygen is customized to render the declarations in detail, with
appropriate comments assuming _unsurprising_ behavior of your hand-written
functions. Invoke this macro in the public section of the class declaration,
e.g.: <pre> class Foo { public: DECLARE_COPY_AND_MOVE_AND_ASSIGN(Foo);

  // ...
};
</pre>
Then the matching definitions should be placed in the associated .cc file.
*/
#define COMMON_UTILS_DECLARE_COPY_AND_MOVE_AND_ASSIGN(Classname) \
  Classname(const Classname&);                                   \
  Classname& operator=(const Classname&);                        \
  Classname(Classname&&);                                        \
  Classname& operator=(Classname&&);
