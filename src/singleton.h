/**
 ******************************************************************************
 *                _                                        _
 *             __| | ___ _ __         __ _ _ __ __ _ _ __ | |__/\   /\
 *            / _` |/ _ \ '_ \ _____ / _` | '__/ _` | '_ \| '_ \ \ / /
 *           | (_| |  __/ |_) |_____| (_| | | | (_| | |_) | | | \ V /
 *            \__,_|\___| .__/       \__, |_|  \__,_| .__/|_| |_|\_/
 *                      |_|          |___/          |_|
 *
 ******************************************************************************
 *
 * singleton.h
 *
 * This source file is part of dep-graphV - An useful tool to analize header
 * dependendencies via graphs.
 *
 * This software is distributed under the MIT License:
 *
 * Copyright (c) 2013 - 2015 Francesco Guastella aka romeoxbm
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef SINGLETON_H
#define SINGLETON_H

#ifndef BUILDSETTINGS_H
#	include "buildsettings.h"
#endif

#ifndef DEPGRAPHV_ENABLE_PCH_SUPPORT
#	include "depgraphv_pch.h"
#endif

namespace depgraphV
{
	template<typename T>
	class Singleton
	{
	public:
		static T& instance()
		{
			static_assert( std::is_base_of<Singleton<T>, T>::value, "Invalid singleton usage!" );
			Q_ASSERT( _instance );
			return *_instance;
		}

		static T* instancePtr()
		{
			static_assert( std::is_base_of<Singleton<T>, T>::value, "Invalid singleton usage!" );
			return _instance;
		}

	protected:
		Singleton()
		{
			Q_ASSERT( !_instance );
			_instance = static_cast<T*>( this );
		}

		~Singleton()
		{
			_instance = 0;
		}

	private:
		static T* _instance;

		Q_DISABLE_COPY( Singleton )
	};

	template<typename T>
	T* Singleton<T>::_instance = 0;
}

#endif // SINGLETON_H
