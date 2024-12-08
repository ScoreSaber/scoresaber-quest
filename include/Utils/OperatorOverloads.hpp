#include <concepts>  // IWYU pragma: keep

#define CREATE_BINARY_OPERATOR(symbol, name) \
    template <typename T, typename U> \
    concept HasOp##name = requires(T a, U b) { \
        T::op_##name(a, b); \
    }; \
    template <typename T, typename U> \
    requires HasOp##name<T, U> \
    auto operator symbol(const T& lhs, const U& rhs) { \
        return T::op_##name(lhs, rhs); \
    } \
    template <typename T, typename U> \
    requires HasOp##name<T, U> && (!std::is_same_v<T, U>) \
    auto operator symbol(const U& lhs, const T& rhs) { \
        return T::op_##name(lhs, rhs); \
    }

#define CREATE_UNARY_OPERATOR(symbol, name) \
    template <typename T> \
    concept HasOp##name = requires(T a) { \
        T::op_##name(a); \
    }; \
    template <typename T> \
    requires HasOp##name<T> \
    auto operator symbol(const T& obj) { \
        return T::op_##name(obj); \
    }

CREATE_BINARY_OPERATOR(==, Equality)
CREATE_BINARY_OPERATOR(!=, Inequality)
CREATE_BINARY_OPERATOR(>, GreaterThan)
CREATE_BINARY_OPERATOR(>=, GreaterThanOrEqual)
CREATE_BINARY_OPERATOR(<, LessThan)
CREATE_BINARY_OPERATOR(<=, LessThanOrEqual)

CREATE_BINARY_OPERATOR(+, Addition)
CREATE_BINARY_OPERATOR(-, Subtraction)
CREATE_BINARY_OPERATOR(*, Multiply)
CREATE_BINARY_OPERATOR(/, Division)

CREATE_UNARY_OPERATOR(-, UnaryNegation)