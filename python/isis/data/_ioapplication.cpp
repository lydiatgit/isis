#include "_ioapplication.hpp"


namespace isis
{

namespace python
{

namespace data
{

_IOApplication::_IOApplication( PyObject *p, const char name[], const bool &input, const bool &output )
	: isis::data::IOApplication( name, input, output ),  
	boost::python::wrapper<isis::data::IOApplication>(), 
	self( p ) 
{
}


_IOApplication::_IOApplication( PyObject *p, const isis::data::IOApplication &base )
	: isis::data::IOApplication( base ),  
	boost::python::wrapper<isis::data::IOApplication>(), 
	self( p )
{
}



}
}
} //end namespace