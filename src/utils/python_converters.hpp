/***************************************************************************\
 *
 *
 *         ___           ___           ___           ___
 *        /  /\         /  /\         /  /\         /  /\
 *       /  /::\       /  /:/_       /  /:/_       /  /::\
 *      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
 *     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
 *    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
 *    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
 *     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
 *      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
 *       \  \:\        \  \::/        /__/:/       \  \:\
 *        \__\/         \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of ReSP.
 *
 *   TRAP is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
\***************************************************************************/


#ifndef PYTHON_CONVERTERS
#define PYTHON_CONVERTERS

#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/to_python_converter.hpp>

namespace resp{
    template <typename ContainerType> struct to_tuple{
        static PyObject* convert(ContainerType const& a){
            boost::python::list result;
            typedef typename ContainerType::const_iterator const_iter;
            for(const_iter p=a.begin();p!=a.end();p++) {
                result.append(boost::python::object(*p));
            }
            return boost::python::incref(boost::python::tuple(result).ptr());
        }

        static const PyTypeObject* get_pytype() { return &PyTuple_Type; }
    };

    struct default_policy{
        static bool check_convertibility_per_element() { return false; }

        template <typename ContainerType>
        static bool check_size(boost::type<ContainerType>, std::size_t){
            return true;
        }

        template <typename ContainerType>
        static void assert_size(boost::type<ContainerType>, std::size_t) {}

        template <typename ContainerType>
        static void reserve(ContainerType& a, std::size_t sz) {}
    };

    struct fixed_size_policy{
        static bool check_convertibility_per_element() { return true; }

        template <typename ContainerType>
        static bool check_size(boost::type<ContainerType>, std::size_t sz){
            return ContainerType::size() == sz;
        }

        template <typename ContainerType>
        static void assert_size(boost::type<ContainerType>, std::size_t sz){
            if (!check_size(boost::type<ContainerType>(), sz)) {
                PyErr_SetString(PyExc_RuntimeError, "Insufficient elements for fixed-size array.");
                boost::python::throw_error_already_set();
            }
        }

        template <typename ContainerType>
        static void reserve(ContainerType&, std::size_t sz){
            if (sz > ContainerType::size()) {
                PyErr_SetString(PyExc_RuntimeError, "Too many elements for fixed-size array.");
                boost::python::throw_error_already_set();
            }
        }

        template <typename ContainerType, typename ValueType>
        static void set_value(ContainerType& a, std::size_t i, ValueType const& v){
            reserve(a, i+1);
            a[i] = v;
        }
    };

    struct variable_capacity_policy : default_policy{
        template <typename ContainerType>
        static void reserve(ContainerType& a, std::size_t sz){
            a.reserve(sz);
        }

        template <typename ContainerType, typename ValueType>
        static void set_value(ContainerType& a, std::size_t, ValueType const& v){
                a.push_back(v);
        }
    };

    struct fixed_capacity_policy : variable_capacity_policy{
        template <typename ContainerType>
        static bool check_size(boost::type<ContainerType>, std::size_t sz){
            return ContainerType::max_size() >= sz;
        }
    };

    struct linked_list_policy : default_policy{
        template <typename ContainerType, typename ValueType>
        static void set_value(ContainerType& a, std::size_t, ValueType const& v){
            a.push_back(v);
        }
    };

    struct set_policy : default_policy{
        template <typename ContainerType, typename ValueType>
        static void set_value(ContainerType& a, std::size_t, ValueType const& v){
            a.insert(v);
        }
    };

    template <typename ContainerType, typename ConversionPolicy>
    struct from_python_sequence{
        typedef typename ContainerType::value_type container_element_type;

        from_python_sequence(){
            boost::python::converter::registry::push_back(&convertible, &construct,
                                                            boost::python::type_id<ContainerType>());
        }

        static void* convertible(PyObject* obj_ptr){
            if (!(   PyList_Check(obj_ptr)
                    || PyTuple_Check(obj_ptr)
                    || PyIter_Check(obj_ptr)
                    || PyRange_Check(obj_ptr)
                    || (   !PyString_Check(obj_ptr)
                    && !PyUnicode_Check(obj_ptr)
                    && (   obj_ptr->ob_type == 0
                    || obj_ptr->ob_type->ob_type == 0
                    || obj_ptr->ob_type->ob_type->tp_name == 0
                    || std::strcmp(
                         obj_ptr->ob_type->ob_type->tp_name,
                         "Boost.Python.class") != 0)
                    && PyObject_HasAttrString(obj_ptr, "__len__")
                    && PyObject_HasAttrString(obj_ptr, "__getitem__")))) return 0;
            boost::python::handle<> obj_iter(boost::python::allow_null(PyObject_GetIter(obj_ptr)));
            if (!obj_iter.get()) {
                PyErr_Clear();
                return 0;
            }
            if (ConversionPolicy::check_convertibility_per_element()) {
                int obj_size = PyObject_Length(obj_ptr);
                if (obj_size < 0) {
                    PyErr_Clear();
                    return 0;
                }
                if (!ConversionPolicy::check_size(
                    boost::type<ContainerType>(), obj_size))
                        return 0;
                    bool is_range = PyRange_Check(obj_ptr);
                    std::size_t i=0;
                    if (!all_elements_convertible(obj_iter, is_range, i))
                        return 0;
                    if (!is_range){
                        if(i != (unsigned int)obj_size)
                            return 0;
                    }
            }
            return obj_ptr;
        }

        static bool all_elements_convertible(boost::python::handle<>& obj_iter, bool is_range, std::size_t& i){
            for(;;i++) {
                boost::python::handle<> py_elem_hdl(
                boost::python::allow_null(PyIter_Next(obj_iter.get())));
                if (PyErr_Occurred()) {
                    PyErr_Clear();
                    return false;
                }
                if (!py_elem_hdl.get())
                    break;
                boost::python::object py_elem_obj(py_elem_hdl);
                boost::python::extract<container_element_type> elem_proxy(py_elem_obj);
                if (!elem_proxy.check())
                    return false;
                if (is_range)
                    break;
            }
            return true;
        }

        static void construct(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data){
            boost::python::handle<> obj_iter(PyObject_GetIter(obj_ptr));
            void* storage = (
                (boost::python::converter::rvalue_from_python_storage<ContainerType>*)
                data)->storage.bytes;
            new (storage) ContainerType();
            data->convertible = storage;
            ContainerType& result = *((ContainerType*)storage);
            std::size_t i=0;
            for(;;i++) {
                boost::python::handle<> py_elem_hdl(boost::python::allow_null(PyIter_Next(obj_iter.get())));
                if (PyErr_Occurred())
                    boost::python::throw_error_already_set();
                if (!py_elem_hdl.get())
                    break;
                boost::python::object py_elem_obj(py_elem_hdl);
                boost::python::extract<container_element_type> elem_proxy(py_elem_obj);
                ConversionPolicy::set_value(result, i, elem_proxy());
            }
            ConversionPolicy::assert_size(boost::type<ContainerType>(), i);
        }
    };

    template <typename ContainerType> struct to_tuple_mapping{
        to_tuple_mapping() {
            boost::python::to_python_converter<
            ContainerType, to_tuple<ContainerType>
            #ifdef BOOST_PYTHON_SUPPORTS_PY_SIGNATURES
            , true
            #endif
            >();
        }
    };

    template <typename ContainerType, typename ConversionPolicy>
    struct tuple_mapping : to_tuple_mapping<ContainerType>{
        tuple_mapping() {
        from_python_sequence<ContainerType, ConversionPolicy>();
        }
    };

    template <typename ContainerType> struct tuple_mapping_fixed_size{
        tuple_mapping_fixed_size() {
            tuple_mapping<ContainerType,fixed_size_policy>();
        }
    };

    template <typename ContainerType> struct tuple_mapping_fixed_capacity{
        tuple_mapping_fixed_capacity() {
            tuple_mapping<ContainerType, fixed_capacity_policy>();
        }
    };

    template <typename ContainerType>struct tuple_mapping_variable_capacity{
        tuple_mapping_variable_capacity() {
            tuple_mapping<ContainerType,variable_capacity_policy>();
        }
    };

    template <typename ContainerType> struct tuple_mapping_set{
        tuple_mapping_set() {
            tuple_mapping<ContainerType,set_policy>();
        }
    };

    /*********** Now I start the code for the dictionary conversion ***********/

    template <typename MapType> struct to_dict{
        static PyObject* convert(MapType const& a){
            boost::python::dict result;
            typedef typename MapType::const_iterator const_iter;
            for(const_iter p=a.begin();p!=a.end();p++) {
                result[p->first] = p->second;
            }
            return boost::python::incref(boost::python::dict(result).ptr());
        }

        static const PyTypeObject* get_pytype() { return &PyDict_Type; }
    };

    template <typename MapType>
    struct from_python_dict{
        typedef typename MapType::key_type map_key_type;
        typedef typename MapType::mapped_type map_val_type;

        from_python_dict(){
            boost::python::converter::registry::push_back(&convertible, &construct,
                                                            boost::python::type_id<MapType>());
        }

        static void* convertible(PyObject* obj_ptr){
            if (!PyDict_Check(obj_ptr))
                return 0;

            boost::python::handle<> obj_hdl(boost::python::borrowed(obj_ptr));
            PyObject *key, *value;
            Py_ssize_t pos = 0;
            while (PyDict_Next(obj_hdl.get(), &pos, &key, &value)) {
                boost::python::handle<> py_key_hdl(boost::python::borrowed(key));
                boost::python::object py_key_obj(py_key_hdl);
                boost::python::handle<> py_val_hdl(boost::python::borrowed(value));
                boost::python::object py_val_obj(py_val_hdl);

                boost::python::extract<map_key_type> key_proxy(key);
                boost::python::extract<map_val_type> val_proxy(value);

                if (!key_proxy.check()){
                    PyErr_Clear();
                    return 0;
                }

                if (!val_proxy.check()){
                    PyErr_Clear();
                    return 0;
                }
            }
            return obj_ptr;
        }

        static void construct(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data){
            if (!PyDict_Check(obj_ptr))
                return;

            void* storage = (
                (boost::python::converter::rvalue_from_python_storage<MapType>*)
                data)->storage.bytes;
            new (storage) MapType();
            data->convertible = storage;
            MapType& result = *((MapType*)storage);

            boost::python::handle<> obj_hdl(boost::python::borrowed(obj_ptr));
            PyObject *key, *value;
            Py_ssize_t pos = 0;
            while (PyDict_Next(obj_ptr, &pos, &key, &value)) {
                boost::python::handle<> py_key_hdl(boost::python::borrowed(key));
                boost::python::object py_key_obj(py_key_hdl);
                boost::python::handle<> py_val_hdl(boost::python::borrowed(value));
                boost::python::object py_val_obj(py_val_hdl);

                boost::python::extract<map_key_type> key_proxy(py_key_obj);
                boost::python::extract<map_val_type> val_proxy(py_val_obj);

                if (!key_proxy.check()){
                    break;
                }

                if (!val_proxy.check()){
                    break;
                }
                result.insert(std::pair<map_key_type,map_val_type>(key_proxy(),val_proxy()));
            }
        }
    };

    template <typename MapType> struct dict_mapping{
        dict_mapping() {
            boost::python::to_python_converter<
            MapType, to_dict<MapType>
            #ifdef BOOST_PYTHON_SUPPORTS_PY_SIGNATURES
            , true
            #endif
            >();
            from_python_dict<MapType>();
        }
    };
}

#endif
