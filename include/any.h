#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <utility>

namespace cl {
// Forward declaration of the main Any class
class Any;

// --- 1. Base Class for Type Erasure (The Interface) ---
class AnyHolderBase {
 public:
  virtual ~AnyHolderBase() = default;
  // Pure virtual method for deep copy (used in copy constructor)
  virtual AnyHolderBase* clone() const = 0;
  // Method to return the type information
  virtual const std::type_info& type() const noexcept = 0;
};

// --- 2. Template Holder Class (The Data Container) ---
template <typename T>
class AnyHolder : public AnyHolderBase {
 public:
  // Stored value
  T value_;

  // Constructor: Uses perfect forwarding (C++11) to construct T
  template <typename... Args>
  AnyHolder(Args&&... args) : value_(std::forward<Args>(args)...)
  {
  }

  // Implements polymorphic copy
  AnyHolderBase* clone() const override
  {
    // Creates a new holder with a copy of the stored value
    return new AnyHolder(value_);
  }

  // Returns the actual type of the stored data
  const std::type_info& type() const noexcept override { return typeid(T); }
};

// --- 3. The Main Any Class ---
class Any {
 private:
  // Uses C++11's unique_ptr for memory management and ownership
  std::unique_ptr<AnyHolderBase> content_;

  // Friend declaration for the any_cast function
  template <typename T>
  friend T* any_cast(Any* operand) noexcept;

 public:
  // --- Constructors and Assignment ---

  // Default constructor: Creates an empty Any
  Any() noexcept = default;

  // Value constructor (stores a copy/move of any type T)
  template <typename T,
            // SFINAE check: prevent construction from another Any
            typename = typename std::enable_if<
                !std::is_same<typename std::decay<T>::type, Any>::value>::type>
  Any(T&& value)
      : content_(
            new AnyHolder<typename std::decay<T>::type>(std::forward<T>(value)))
  {
  }

  // Copy constructor (deep copy)
  Any(const Any& other)
      : content_(other.content_ ? other.content_->clone() : nullptr)
  {
  }

  // Move constructor (transfers ownership)
  Any(Any&& other) noexcept : content_(std::move(other.content_)) {}

  // Copy assignment
  Any& operator=(const Any& other)
  {
    // Standard copy-and-swap (using move for efficiency)
    if (this != &other) {
      Any temp(other);
      *this = std::move(temp);
    }
    return *this;
  }

  // Move assignment
  Any& operator=(Any&& other) noexcept
  {
    if (this != &other) {
      content_ = std::move(other.content_);
    }
    return *this;
  }

  // --- Member Functions ---

  // Clears the stored value
  void reset() noexcept { content_.reset(); }

  // Swaps contents with another Any object
  void swap(Any& other) noexcept { std::swap(content_, other.content_); }

  // Checks if the container holds a value
  bool has_value() const noexcept { return content_ != nullptr; }

  // Returns the type_info object of the stored value, or typeid(void) if empty
  const std::type_info& type() const noexcept
  {
    return has_value() ? content_->type() : typeid(void);
  }
};

// --- 4. Free Function Accessor (any_cast) ---

// Non-throwing version for pointer access
template <typename T>
T* any_cast(Any* operand) noexcept
{
  // T must be non-reference (e.g., int, not int& or const int&)
  typedef typename std::remove_reference<T>::type NonRefT;

  if (!operand || !operand->content_) {
    return nullptr;
  }

  // Check if types match using C++11's typeid comparison
  if (operand->content_->type() == typeid(NonRefT)) {
    // Use static_cast after the type check to cast the base pointer
    // to the correct derived type, then return the address of the value.
    AnyHolder<NonRefT>* derived =
        static_cast<AnyHolder<NonRefT>*>(operand->content_.get());
    return &(derived->value_);
  }
  return nullptr;
}

// Throwing version for value access (const reference)
template <typename T>
const T& any_cast(const Any& operand)
{
  auto ptr =
      any_cast<typename std::remove_const<T>::type>(&const_cast<Any&>(operand));
  if (!ptr) {
    throw std::bad_cast();  // Use a standard exception if available
  }
  return *ptr;
}

// Throwing version for value access (non-const reference)
template <typename T>
T& any_cast(Any& operand)
{
  auto ptr = any_cast<T>(&operand);
  if (!ptr) {
    throw std::bad_cast();
  }
  return *ptr;
}

// Throwing version for value access (by value)
template <typename T>
T any_cast(Any&& operand)
{
  // This performs a move out of the Any.
  // Check type first
  if (operand.type() != typeid(T)) {
    throw std::bad_cast();
  }
  // Perform the move and reset the Any
  T value = std::move(*any_cast<T>(&operand));
  operand.reset();
  return value;
}
}  // namespace cl
