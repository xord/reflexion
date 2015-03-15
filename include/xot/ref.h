// -*- c++ -*-
#pragma once
#ifndef __XOT_REF_H__
#define __XOT_REF_H__


#ifdef check // by OSX's AssertMacros.h
#undef check
#endif

#include <limits.h>
#include <assert.h>
#include <typeinfo>
#include <boost/noncopyable.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <xot/defs.h>
#include <xot/exception.h>
#include <xot/debug.h>


//#define XOT_REF_DEBUG 1


namespace Xot
{


	class EmptyClass {};


	template <typename SuperClass = EmptyClass>
	class RefCountable : public SuperClass, public boost::noncopyable
	{

		public:

			virtual void retain (void* data = NULL) const
			{
				refc_update_count(+1);

				#ifdef XOT_REF_DEBUG
					doutln(
						"%s: %d -> %d",
						typeid(this).name(), refc_count() - 1, refc_count());
				#endif
			}

			virtual void release (void* data = NULL) const
			{
				assert(refc_count() >= 0);
				bool del = !refc_retained() || refc_update_count(-1) == 0;

				#ifdef XOT_REF_DEBUG
					doutln(
						"%s: %d -> %d, refcount:%s, delete:%s",
						typeid(this).name(), refc_count() + 1, refc_count(),
						refc_retained() ? "yes" : "no", del ? "yes" : "no");
				#endif

				if (del) delete this;
			}

			virtual void* rucy_value () const
			{
				return NULL;
			}

			virtual bool rucy_disable_override () const
			{
				return false;
			}

		protected:

			RefCountable ()
			{
			}

			virtual ~RefCountable ()
			{
			}

			virtual int refc_count () const
			{
				return refc.count;
			}

			virtual bool refc_retained () const
			{
				return refc.aux & 0x1;
			}

			virtual int refc_update_count (int add) const
			{
				assert(add != 0);
				if (add >= 0) refc.aux |= 0x1;// bit for retained flag.

				int c = refc.count + add;
				if (c < 0)
					invalid_state_error(__FILE__, __LINE__);
				if (c > USHRT_MAX)
					xot_error(__FILE__, __LINE__, "refc.count overflow.");

				return refc.count = c;
			}

			virtual ushort refc_aux () const
			{
				return refc.aux >> 1;
			}

			virtual void refc_set_aux (ushort aux) const
			{
				if ((0x1 << 15) & aux)
					argument_error(__FILE__, __LINE__);

				refc.aux = (refc.aux & 0x1) | (aux << 1);
			}

		private:

			mutable struct Data
			{

				ushort count, aux;

				Data () : count(0), aux(0) {}

			} refc;

	};// RefCountable


	template <typename T, typename = void>
	class Ref
	{

		typedef Ref<T> This;

		typedef       T       Value;

		typedef const T  ConstValue;

		typedef       T&      Reference;

		typedef const T& ConstReference;

		typedef       T*      Pointer;

		typedef const T* ConstPointer;

		public:

			Ref (Pointer ptr = NULL)
			:	ptr(ptr)
			{
				if (ptr) ptr->retain();
			}

			Ref (const This& obj)
			:	ptr(obj.ptr)
			{
				if (ptr) ptr->retain();
			}

			This& operator = (Pointer ptr)
			{
				reset(ptr);
				return *this;
			}

			This& operator = (const This& obj)
			{
				if (&obj == this) return *this;
				reset(obj.ptr);
				return *this;
			}

			~Ref ()
			{
				if (ptr) ptr->release();
			}

			void reset (Pointer ptr = NULL)
			{
				if (this->ptr == ptr) return;
				if (this->ptr) this->ptr->release();
				this->ptr = ptr;
				if (this->ptr) this->ptr->retain();
			}

			     Pointer get ()       {return ptr;}

			ConstPointer get () const {return ptr;}

			     Pointer operator -> ()       {return get();}

			ConstPointer operator -> () const {return get();}

			     Reference operator * ()       {return *get();}

			ConstReference operator * () const {return *get();}

			operator      Pointer ()       {return get();}

			operator ConstPointer () const {return get();}

			bool operator == (Pointer ptr) const {return this->ptr == ptr;}

			bool operator != (Pointer ptr) const {return !operator==(ptr);}

			bool operator == (ConstPointer ptr) const {return this->ptr == ptr;}

			bool operator != (ConstPointer ptr) const {return !operator==(ptr);}

			bool operator == (const This& obj) const {return ptr == obj.ptr;}

			bool operator != (const This& obj) const {return !operator==(obj);}

			bool operator <  (const This& obj) const {return ptr < obj.ptr;}

			operator bool () const {return ptr != NULL;}

			bool operator ! () const {return !operator bool();}

		private:

			Pointer ptr;

	};// Ref


	template <typename T>
	class Ref<T, typename boost::enable_if<boost::is_const<T> >::type>
	{

		typedef Ref<T> This;

		typedef typename boost::remove_const<T>::type Value;

		typedef                              T   ConstValue;

		typedef       Value&      Reference;

		typedef const Value& ConstReference;

		typedef       Value*      Pointer;

		typedef const Value* ConstPointer;

		public:

			Ref (ConstPointer ptr = NULL)
			:	ptr(ptr)
			{
				if (ptr) ptr->retain();
			}

			Ref (const This& obj)
			:	ptr(obj.ptr)
			{
				if (ptr) ptr->retain();
			}

			This& operator = (ConstPointer ptr)
			{
				reset(ptr);
				return *this;
			}

			This& operator = (const This& obj)
			{
				if (&obj == this) return *this;
				reset(obj.ptr);
				return *this;
			}

			~Ref ()
			{
				if (ptr) ptr->release();
			}

			void reset (ConstPointer ptr = NULL)
			{
				if (this->ptr == ptr) return;
				if (this->ptr) this->ptr->release();
				this->ptr = ptr;
				if (this->ptr) this->ptr->retain();
			}

			ConstPointer get () const {return ptr;}

			ConstPointer operator -> () const {return get();}

			ConstReference operator * () const {return *get();}

			operator ConstPointer () const {return get();}

			bool operator == (ConstPointer ptr) const {return this->ptr == ptr;}

			bool operator != (ConstPointer ptr) const {return !operator==(ptr);}

			bool operator == (const This& obj) const {return ptr == obj.ptr;}

			bool operator != (const This& obj) const {return !operator==(obj);}

			bool operator <  (const This& obj) const {return ptr < obj.ptr;}

			operator bool () const {return ptr != NULL;}

			bool operator ! () const {return !operator bool();}

		private:

			ConstPointer ptr;

	};// Ref


}// Xot


#endif//EOH
