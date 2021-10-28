/*
 *  pyUtils.hpp
 *  Copyright 2021 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#ifndef PYSMCE_PYUTILS_HPP
#define PYSMCE_PYUTILS_HPP

#include <filesystem>
#include <optional>
#include <system_error>
#include <type_traits>
#include <boost/python.hpp>
#include <pyerrors.h>

namespace bpy = boost::python;

namespace pysmce {

inline void handle_errc(std::error_code ec) {
    if (ec) {
        auto msg = ec.message();
        PyErr_SetString(PyExc_RuntimeError, msg.c_str());
    }
}

template <auto mv, class C, class R, bool disable_specialisations = false>
struct MemVarWrapper {
    template <class To>
    constexpr static auto cvt_get() noexcept -> To (*)(C*) {
        return +[](C* self) -> To { return To{self->*mv}; };
    }
    template <class From>
    constexpr static auto cvt_set() noexcept -> void (*)(C*, const From&) {
        return +[](C* self, const From& value) -> void { (self->*mv) = value; };
    }
};

template <auto mv, class C, class R>
struct MemVarWrapper<mv, C, std::optional<R>, false> : MemVarWrapper<mv, C, std::optional<R>, true> {
    constexpr static auto opt_get() noexcept -> bpy::object (*)(C*) {
        return +[](C* self) { return self->*mv ? bpy::object{*(self->*mv)} : bpy::object{}; };
    }
    constexpr static auto opt_set() noexcept -> void (*)(C*, const bpy::object&) {
        return +[](C* self, const bpy::object& obj) {
            if (obj.is_none())
                (self->*mv) = std::nullopt;
            else
                (self->*mv) = bpy::extract<R>{obj}();
        };
    }
};

template <auto mv>
constexpr MemVarWrapper wrap_memvar = []<class C, class R>(R C::*) { return MemVarWrapper<mv, C, R>{}; }(mv);
template <auto mv>
using WrapMemVar = decltype(wrap_memvar<mv>);

template <auto mf, bool disable_specialisations, class C, class R, class... Args>
struct MemFnWrapper {};

template <auto mf, class C, class... Args>
struct MemFnWrapper<mf, false, C, std::error_code, Args...> : MemFnWrapper<mf, true, C, std::error_code, Args...> {
    constexpr static auto ret_errc() noexcept {
        return +[](C* self, Args... args) -> void { handle_errc((self->*mf)(std::forward<decltype(args)>(args)...)); };
    }
};

template <auto mf>
constexpr MemFnWrapper wrap_memfn = []<class C, class R, class... Args>(R (C::*)(Args...)) {
    return MemFnWrapper<mf, false, C, R, Args...>{};
}
(mf);
template <auto mf>
using WrapMemFn = decltype(wrap_memfn<mf>);

template <class T>
std::vector<T> pyseq_copycollect_vec(const bpy::object& pyseq) {
    std::vector<T> ret;
    if (!pyseq.is_none()) {
        bpy::stl_input_iterator<T> b{pyseq}, e;
        std::copy(b, e, std::back_inserter(ret));
    }
    return ret;
}

} // namespace pysmce

#endif // PYSMCE_PYUTILS_HPP
