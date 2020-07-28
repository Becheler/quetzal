//                 Expressive library
//          Copyright Ambre Marques 2016 - 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAQUETTE_EXPRESSIVE_H
#define MAQUETTE_EXPRESSIVE_H

#include <functional>
#include <utility>

#include <type_traits>
#include <iostream>
#include <string>


#include <tuple>
#include <type_traits>

namespace std {

//common_type<tuple<T...>, tuple<U...>> -> tuple<common_type<T, U>...>
template <typename...T, typename...U>
struct common_type<std::tuple<T...>, std::tuple<U...>> {
	using type = std::tuple<typename std::common_type<T, U>::type...>;
};

} // end of namespace std::

namespace meta {

template <bool b, bool... bools>
inline constexpr bool all() {return b && all<bools...>();}

template<> constexpr bool all<true>() {return true;}
template<> constexpr bool all<false>() {return false;}

} // end of namespace meta::

namespace quetzal {
namespace expressive {

// general case: Callable being a class, use operator() properties
template <typename Callable>
struct Callable_traits : public Callable_traits<decltype(&Callable::operator())> {};

// free function pointer
template <typename Ret, typename... Args>
struct Callable_traits<Ret(*)(Args...)> {
	using return_type    = Ret;
	using arguments_type = std::tuple<Args...>;
	static constexpr std::size_t arguments_count = sizeof...(Args);
};

// function type
template <typename Ret, typename... Args>
struct Callable_traits<Ret(Args...)> {
	using return_type    = Ret;
	using arguments_type = std::tuple<Args...>;
	static constexpr std::size_t arguments_count = sizeof...(Args);
};

// member function type
template <typename Ret, typename Class, typename... Args>
struct Callable_traits<Ret(Class::*)(Args...) const> {
	using return_type    = Ret;
	using arguments_type = std::tuple<Args...>;
	static constexpr std::size_t arguments_count = sizeof...(Args);
};


template <typename Callable>
using return_type = typename Callable_traits<Callable>::return_type;

template <typename Callable>
using arguments_type = typename Callable_traits<Callable>::arguments_type;


template <typename Callable, std::size_t I>
using argument_type = typename std::tuple_element<I, typename Callable_traits<Callable>::arguments_type>::type;

template <typename Callable>
constexpr auto arguments_count() {return Callable_traits<Callable>::arguments_count;}




template <typename Functor, typename Interface>
using functor_has_interface = std::is_convertible< Interface, typename Callable_traits<Functor>::arguments_type >;

template<typename Functor, typename... Args>
using callable_with = std::is_convertible< arguments_type<void(Args...)>, arguments_type<Functor> >;



/*
support base class for functors delagating their operator() to other functor(s).
*/
template <typename... Functors>
struct composite_functor {
	using functors_type = std::tuple<Functors...>;

	template <std::size_t I>
	using return_type = return_type< typename std::tuple_element<I, functors_type>::type >;

	template <std::size_t I>
	static constexpr std::size_t arguments_count() {return Callable_traits<typename std::tuple_element<I, functors_type>::type>::arguments_count;}

	using arguments_type = typename std::common_type< typename Callable_traits<Functors>::arguments_type... >::type;

	constexpr composite_functor(Functors const&... functors) : functors(functors...) {
		// all Functors shall have the same call arguments (which will be those of the resulting functor)
		static_assert( meta::all< functor_has_interface<Functors, arguments_type>::value... >(),
			"incompatible functors (arguments differs)"
		);
	}

	//callability check
	template<typename... Args>
	static constexpr bool call_check() {
		//shall we check against interface or against each functors
		return meta::all< callable_with<Functors, Args...>::value... >();
	}

	template <std::size_t I>
	constexpr auto const& f() const {return std::get<I>(functors);}

	functors_type functors;
};

//unary compositing specialization
template <typename F>
struct composite_functor<F> {
	using return_type = typename Callable_traits<F>::return_type;
	using arguments_type = typename Callable_traits<F>::arguments_type;

	static constexpr std::size_t arguments_count() {return Callable_traits<F>::arguments_count;}

	constexpr composite_functor(F const& f) : f(f) {}

	//callability check
	template<typename... Args>
	static constexpr bool call_check() {
		return callable_with<F, Args...>::value;
	}

	F f;
};


// ****** transforming functions into operator friendly classes ****** //

template <typename F>
struct make_expression_t;

template <typename F>
struct Callable_traits<make_expression_t<F>> : public Callable_traits<F>{};

template <typename F>
struct make_expression_t: public composite_functor<F> {
	using base_t = composite_functor<F>;
	using value_type = typename base_t::return_type;

	constexpr make_expression_t(F f): base_t(f) {}

	template <typename... Args>
	constexpr
	typename std::enable_if< base_t::template call_check<Args...>(), value_type>::type
	operator()(Args... args) const {
		return base_t::f(args...);
	}
};

template <typename F>
constexpr auto expression(F f) {return make_expression_t<F>(f);}

/*!
 * \brief Transforms functions into operator friendly classes
 *
 * Transforms a function object into operator friendly class, allowing it
 * to be composed
 * into a more complex expression using mathematical
 * operators with others expressive objects having compatible callable interface.
 *
 * \param f a function object
 * \ingroup expressive
 * \section Example
 * \snippet expressive/test/expressive_test.cpp Example
 * \section Output
 * \include expressive/test/expressive_test.output
 */
template <typename F>
constexpr auto use(F f) {return make_expression_t<F>(f);}

template <typename F>
constexpr auto function(F f) {return make_expression_t<F>(f);}

// ****** literal generation ****** //

template <typename T, typename... Args>
struct literal_t {
	const T value;
	constexpr literal_t(T const& value): value(value) {}
	constexpr T operator()(Args...) const {return value;}
};

/*!
 * \brief Transforms <a href="https://stackoverflow.com/documentation/c%2b%2b/7836/literals#t=201709240552046831589">literals</a> into operator friendly classes
 *
 * Transforms a <a href="https://stackoverflow.com/documentation/c%2b%2b/7836/literals#t=201709240552046831589">literal</a>
 * object into operator friendly class, allowing it
 * to be composed into a more complex expression using mathematical
 * operators with others expressive objects having compatible callable interface.
 *
 * \tparam T the literal type
 * \tparam Args the callable interface arguments to which the literal should be transformed
 * \ingroup expressive
 * \section Example
 * \snippet expressive/test/expressive_test.cpp Example
 * \section Output
 * \include expressive/test/expressive_test.output
 */
 template <typename... Args, typename T>
constexpr auto literal(T t) {return literal_t<T, Args...>{t};}


/*!
 * \brief Transforms <a href="https://stackoverflow.com/documentation/c%2b%2b/7836/literals#t=201709240552046831589">literals</a> into operator friendly classes
 *
 * Transforms a <a href="https://stackoverflow.com/documentation/c%2b%2b/7836/literals#t=201709240552046831589">literal</a>
 * object into operator friendly class, allowing it
 * to be composed into a more complex expression using mathematical
 * operators with others expressive objects having compatible callable interface.
 *
 * \tparam T the literal type
 * \tparam Args the callable interface arguments to which the literal should be transformed
 * \ingroup expressive
 * \section Example
 * \snippet expressive/test/expressive_test.cpp Example
 * \section Output
 * \include expressive/test/expressive_test.output
 */
template <typename... Args>
struct literal_factory {
	template <typename T>
	constexpr auto operator()(T t) {return literal<Args...>(t);}
};


// ****** operator overloading ****** //

template <typename Operator, typename T>
using applied_unary_type = decltype( std::declval<Operator>()(std::declval<T>()) );

template <typename Op, typename T>
constexpr auto is_applicable_unary_impl(int) -> decltype( std::declval<applied_unary_type<Op,T>>(), bool() ) {
    return true;
}

template <typename Op, typename T>
constexpr bool is_applicable_unary_impl(...) {return false;}



template <typename A, typename Operator, typename B>
using applied_binary_type = decltype( std::declval<Operator>()(std::declval<A>(), std::declval<B>()) );

template <typename A, typename Op, typename B>
constexpr auto is_applicable_binary_impl(int) -> decltype( std::declval<applied_binary_type<A,Op,B>>(), bool() ) {
    return true;
}

template <typename A, typename Op, typename B>
constexpr bool is_applicable_binary_impl(...) {return false;}


//Operator is expected to be std::plus<void> or so

template <typename Operator, typename T>
constexpr bool is_applicable_unary() {return is_applicable_unary_impl<Operator, T>(0);}

template <typename A, typename Operator, typename B>
constexpr bool is_applicable_binary() {return is_applicable_binary_impl<A, Operator, B>(0);}


//Operator is expected to be something like std::plus<void>, which has a template operator()

template <typename Operator, typename F>
struct unop_t;

template <typename Operator, typename F>
struct Callable_traits<unop_t<Operator,F>> {
	using return_type    = typename unop_t<Operator,F>::value_type;
	using arguments_type = typename unop_t<Operator,F>::arguments_type;
	static constexpr std::size_t arguments_count = Callable_traits<F>::arguments_count;
};


template <typename Operator, typename F>
struct unop_t: public composite_functor<F> {
	using base_t = composite_functor<F>;
	using value_type = applied_unary_type<Operator, typename base_t::return_type>;

	unop_t(F f) : base_t(f) {
		static_assert( is_applicable_unary<Operator, typename base_t::return_type>(), "incompatible functor value" );
	}

	template <typename... Args>
	constexpr
	value_type operator()(Args... args) const {
		static_assert(base_t::template call_check<Args...>(), "wrong arguments types or count");
		//use a sub function to shut up compiler, we already have an assert
		return impl(args...);
	}

	template <typename... Args>
	constexpr
	value_type impl(Args... args) const {return Operator{}(base_t::f(args...));}
};

/* symetric binop over functors */

template <typename FL, typename Operator, typename FR>
struct symetric_binop_t;

template <typename FL, typename Operator, typename FR>
struct Callable_traits<symetric_binop_t<FL,Operator,FR>> {
	using return_type    = typename symetric_binop_t<FL,Operator,FR>::value_type;
	using arguments_type = typename symetric_binop_t<FL,Operator,FR>::arguments_type;
	static constexpr std::size_t arguments_count = std::tuple_size<arguments_type>::arguments_count;
};

template <typename FL, typename Operator, typename FR>
struct symetric_binop_t : composite_functor<FL, FR>, private Operator {
	using base_t = composite_functor<FL, FR>;

	using FL_return_type = typename base_t::template return_type<0>;
	using FR_return_type = typename base_t::template return_type<1>;

	using value_type = applied_binary_type<FL_return_type, Operator, FR_return_type>;

	constexpr symetric_binop_t(FL l, FR r): base_t(l, r) {
		static_assert(
			is_applicable_binary<FL_return_type, Operator, FR_return_type>(),
			"incompatible functors value types"
		);
	}

	template <typename... Args>
	constexpr
	value_type
	operator()(Args... args) const {
		static_assert(base_t::template call_check<Args...>(), "wrong arguments types or count");

		return Operator::operator()( base_t::template f<0>()(args...), base_t::template f<1>()(args...) );
	}

	template <typename... Args>
	constexpr
	value_type impl(Args... args) const {
		return Operator::operator()( base_t::template f<0>()(args...), base_t::template f<1>()(args...) );
	}
};

/* ****** math around functions ****** */
/*
template <typename F>
constexpr auto operator!(F f) {return unop_t<std::not<void>, F>{f};}
*/
template <typename F>
constexpr auto operator-(F f) {return unop_t<std::negate<void>, F>{f};}

template <typename F1, typename F2>
constexpr auto operator+(F1 f1, F2 f2) {return symetric_binop_t<F1, std::plus<void>, F2>{f1, f2};}

template <typename F1, typename F2>
constexpr auto operator-(F1 f1, F2 f2) {return symetric_binop_t<F1, std::minus<void>, F2>{f1, f2};}

template <typename F1, typename F2>
constexpr auto operator*(F1 f1, F2 f2) {return symetric_binop_t<F1, std::multiplies<void>, F2>{f1, f2};}

template <typename F1, typename F2>
constexpr auto operator/(F1 f1, F2 f2) {return symetric_binop_t<F1, std::divides<void>, F2>{f1, f2};}

template <typename F1, typename F2>
constexpr auto operator%(F1 f1, F2 f2) {return symetric_binop_t<F1, std::modulus<void>, F2>{f1, f2};}



/* ****** composing functions ****** */

template <typename Outer, typename... Inners>
struct compose_t;

template <typename Outer, typename... Inners>
struct Callable_traits<compose_t<Outer, Inners...>> {
	using return_type    = typename compose_t<Outer, Inners...>::value_type;
	using arguments_type = typename compose_t<Outer, Inners...>::arguments_type;
	static constexpr std::size_t arguments_count = std::tuple_size<arguments_type>::value;
};

template <typename Outer, typename... Inners>
struct compose_t: public composite_functor<Inners...> {
	using base_t = composite_functor<Inners...>;
	using value_type = return_type<Outer>;

	constexpr
	compose_t(Outer outer, Inners... inners) : base_t(inners...), outer(outer) {
		// there must be exactly as many tied functors as Outer arguments
		static_assert(
			sizeof...(Inners) == arguments_count<Outer>(),
			"wrong functors count"
		);

		// each tied functor must return a value compatible with the matching Outer's argument
		static_assert(
			composable_check(std::index_sequence_for<Inners...>{}),
			"uncomposable functors"
		);
	}

	template <typename... Args>
	constexpr
	value_type operator()(Args... args) const {
		static_assert(base_t::template call_check<Args...>(), "wrong arguments type or count");

		return impl( std::index_sequence_for<Inners...>{} , args...);
	}

	// *** call details *** //
	//actual call
	template<std::size_t... Is, typename... Args>
	constexpr
	value_type impl(std::index_sequence<Is...>, Args... args) const {
		return outer( base_t::template f<Is>()(args...)... );
	}

	// *** build details *** //
	template<std::size_t... Is>
	static constexpr
	bool composable_check(std::index_sequence<Is...>){
		return meta::all<std::is_convertible< return_type<Inners>, argument_type<Outer, Is> >::value...>();
	}

	Outer outer;
};


//spécialisation pour le cas où il n'y a qu'une fonction interne (car composite_functor est spécialisé).
template <typename Outer, typename Inner>
struct compose_t<Outer, Inner>: public composite_functor<Inner> {
	using base_t = composite_functor<Inner>;
	using value_type = return_type<Outer>;

	constexpr
	compose_t(Outer outer, Inner inner) : base_t(inner), outer(outer) {
		static_assert(arguments_count<Outer>() == 1, "wrong functors count");

		// tied functor must return a value compatible with the argument of Outer
		static_assert(
			std::is_convertible< return_type<Inner>, argument_type<Outer, 0> >::value,
			"uncomposable functors"
		);
	}

	template <typename... Args>
	constexpr
	value_type operator()(Args... args) const {
		static_assert(base_t::template call_check<Args...>(), "wrong arguments type or count");
		return impl(args...);
	}

	template <typename... Args>
	constexpr value_type impl(Args... args) const {
		return outer( base_t::f(args...) );
	}

	Outer outer;
};

//spécialisation pour le cas où Inner est vide.

template <typename Outer>
struct compose_t<Outer>: public composite_functor<Outer> {
	using base_t = composite_functor<Outer>;
	using value_type = typename base_t::return_type;

	constexpr compose_t(Outer f): base_t(f) {}

	template <typename... Args>
	constexpr value_type
	operator()(Args... args) const {
		static_assert(base_t::template call_check<Args...>(), "wrong arguments type or count");
		return impl(args...);
	}

	template <typename... Args>
	constexpr value_type impl(Args... args) const {return f(args...);}
};


/*!
 * \brief Mathematical <a href="https://en.wikipedia.org/wiki/Function_composition">function composition</a>
 *
 * Forms an expression in which the outputs of the inner functions become the input of the outer function.
 *
 * \param f the outer function
 * \param fs the inner functions
 * \ingroup expressive
 * \section Example
 * \snippet expressive/test/expressive_test.cpp Example
 * \section Output
 * \include expressive/test/expressive_test.output
 */
template<typename F, typename... Fs>
constexpr auto compose(F f, Fs... fs) {return compose_t<F, Fs...>(f, fs...);}

/* ****** chaining functions ****** */

template<typename F>
constexpr auto chain(F f) {return expression(f);}

/*!
 * \brief Mathematical <a href="https://en.wikipedia.org/wiki/Function_composition">function composition</a>
 *
 * Forms an expression in which the output of the inner function becomes the input of the outer function.
 *
 * \param inner the inner function
 * \param outer the inner functions
 * \ingroup expressive
 * \section Example
 * \snippet expressive/test/expressive_test.cpp Example
 * \section Output
 * \include expressive/test/expressive_test.output
 */
template<typename Inner, typename Outer>
constexpr auto operator>>(Inner inner, Outer outer) {return compose(outer, inner);}

/*!
 * \brief Mathematical <a href="https://en.wikipedia.org/wiki/Function_composition">function composition</a>
 *
 * Forms an expression in which the output of the inner function becomes the input of the outer function.
 *
 * \param outer the outer function
 * \param inner the inner function
 * \ingroup expressive
 * \section Example
 * \snippet expressive/test/expressive_test.cpp Example
 * \section Output
 * \include expressive/test/expressive_test.output
 */
template<typename Inner, typename Outer>
constexpr auto operator<<(Outer outer, Inner inner) {return compose(outer, inner);}

/*!
 * \brief Mathematical <a href="https://en.wikipedia.org/wiki/Function_composition">function composition</a>
 *
 * Forms an expression in which the outputs of the inner functions become the input of the outer function.
 *
 * \param f1 the outer function
 * \param f2 the inner function: its output is the input of f1
 * \param fs the inner functions: their output are the input of f2
 * \ingroup expressive
 * \section Example
 * \snippet expressive/test/expressive_test.cpp Example
 * \section Output
 * \include expressive/test/expressive_test.output
 */
template<typename F1, typename F2, typename... Fs>
constexpr auto chain(F1 f1, F2 f2, Fs... fs) {return f1 >> chain(f2, fs...);}



/* ****** ostreaming functions ****** */

/* suppose que les foncteurs soient affichables, ce qui n'est pas le cas de base. */

template <typename T>
std::ostream& operator<<(std::ostream& os, std::negate<T> const&) {return os << '-';}

template <typename T>
std::ostream& operator<<(std::ostream& os, std::plus<T> const&) {return os << '+';}
template <typename T>
std::ostream& operator<<(std::ostream& os, std::minus<T> const&) {return os << '-';}
template <typename T>
std::ostream& operator<<(std::ostream& os, std::multiplies<T> const&) {return os << '*';}
template <typename T>
std::ostream& operator<<(std::ostream& os, std::divides<T> const&) {return os << '/';}
template <typename T>
std::ostream& operator<<(std::ostream& os, std::modulus<T> const&) {return os << '%';}



template <typename T, typename... Args>
std::ostream& operator<<(std::ostream& os, literal_t<T, Args...> const& l) {
	return os << l.value;
}

template <typename Operator, typename F>
std::ostream& operator<<(std::ostream& os, unop_t<Operator, F> const& f) {
	return os << Operator{} << f.f;
}

template <typename F1, typename Operator, typename F2>
std::ostream& operator<<(std::ostream& os, symetric_binop_t<F1, Operator, F2> const& f) {
	return os << f.f1 << Operator{} << f.f2;
}

} // end of namespace math::expressive::
} // end of namespace math::

#endif
