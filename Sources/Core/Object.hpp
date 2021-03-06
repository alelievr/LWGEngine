#pragma once

#include <iostream>
#include <string>

#include "Transform.hpp"

namespace LWGC
{
	class		Object
	{
		protected:
			std::string	_name;
			int			_flags;

		public:
			Object(void);
			Object(const Object&);
			virtual		~Object(void);

			Object &	operator=(Object const & src);

			std::string			GetName(void) const;
			virtual void		SetName(const std::string & newName);

			int					GetFlags(void) const;
			virtual void		SetFlags(int tmp);
	};

	std::ostream &	operator<<(std::ostream & o, Object const & r);
}
