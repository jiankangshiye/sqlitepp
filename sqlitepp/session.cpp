//////////////////////////////////////////////////////////////////////////////
// session.cpp
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <sqlite3.h>

#include "session.hpp"
#include "exception.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

namespace { // implementation details

//////////////////////////////////////////////////////////////////////////////

string_t last_error_msg(sqlite3* impl)
{
	string_t result;
	if ( impl )
	{
		result = reinterpret_cast<char_t const*>
			(aux::select(::sqlite3_errmsg, ::sqlite3_errmsg16)(impl));
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////////

} // namespace { // implementation details

//////////////////////////////////////////////////////////////////////////////

	// Create an empty session.
session::session() 
	: impl_(0)
	, active_txn_(0)
{
}
//----------------------------------------------------------------------------

session::session(string_t const& file_name)
	: impl_(0)
	, active_txn_(0)
{
	open(file_name);
}
//----------------------------------------------------------------------------

session::~session()
{
	try
	{
		close();
	}
	catch(...)
	{
	}
}
//----------------------------------------------------------------------------

void session::open(string_t const& file_name)
{
	// close previouse session
	close();

	int const r = aux::select(::sqlite3_open, ::sqlite3_open16)(file_name.c_str(), &impl_);
	if ( r != SQLITE_OK )
	{
		string_t const msg( last_error_msg(impl_) );
		int const err = ::sqlite3_errcode(impl_);
		close(); // session should be closed anyway
		throw exception(err, msg);
	}
}
//----------------------------------------------------------------------------

void session::close()
{
	if ( is_open() )
	{
		int const r = sqlite3_close(impl_);
		impl_ = 0;
		check_error(r);
	}
}
//----------------------------------------------------------------------------

bool session::is_autocommit() const // throw()
{
	return ::sqlite3_get_autocommit(impl_) != 0;
}
//----------------------------------------------------------------------------

void session::check_error(int code) const
{
	if ( code != SQLITE_OK && code != SQLITE_ROW && code != SQLITE_DONE )
	{
		throw exception(code, last_error_msg(impl_));
	}
}
//----------------------------------------------------------------------------

void session::check_last_error() const
{
	check_error(::sqlite3_errcode(impl_));
}

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////